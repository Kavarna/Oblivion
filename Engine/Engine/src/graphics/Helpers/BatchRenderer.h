#pragma once


#include "../interfaces/Shader.h"
#include "../interfaces/GraphicsObject.h"
#include "../interfaces/ICamera.h"
#include "../Shaders/BatchShader.h"
#include "../Texture.h"


class BatchRenderer : public IGraphicsObject
{
public:
	BatchRenderer();
	~BatchRenderer();

public:
	void Create();

public:
	void Reconstruct(uint32_t newSize);
	void Begin();
	void Vertex(BatchShader::SVertex const& vertex);
	void Vertex(DirectX::XMFLOAT3 const& pos, DirectX::XMFLOAT4 const& color);
	void End(ICamera *, D3D11_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	/// <summary>Shader should have everything set up</summary>
	template <class Shader>
	void End(D3D11_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	uint32_t						m_numMaxVertices = 10000;
	uint32_t						m_currentIndex = 0;
	BatchShader::SVertex*			m_bufferData;
};

class TextureBatchRenderer : public IGraphicsObject
{
public:
	TextureBatchRenderer();
	~TextureBatchRenderer();

public:
	void Create();

public:
	void Reconstruct(uint32_t newSize);
	void Begin();
	void Vertex(DirectX::XMFLOAT3 const& pos, DirectX::XMFLOAT2 const& tex);
	void End(ICamera *, Texture *, D3D11_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/// <summary>Shader should have everything set up</summary>
	template <class Shader>
	void End(Texture *, D3D11_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	uint32_t						m_numMaxVertices = 10000;
	uint32_t						m_currentIndex = 0;
	TextureBatchShader::SVertex*	m_bufferData;
};

template <class Shader>
void BatchRenderer::End(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	static_assert(std::is_base_of<IShader, Shader>::value,
		"Generic argument for Rende must be a IShader based class");

	auto shader = Shader::Get();

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_vertexBuffer.Get());

	if (m_currentIndex)
	{
		m_d3d11Context->IASetPrimitiveTopology(topology);
		UINT stride = sizeof(BatchShader::SVertex);
		UINT offset = 0;
		m_d3d11Context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_d3d11Context->Draw(m_currentIndex, 0);
		if (g_isDeveloper)
			g_drawCalls++;
	}
}

template <class Shader>
void TextureBatchRenderer::End(Texture * tex, D3D11_PRIMITIVE_TOPOLOGY top)
{
	static_assert(std::is_base_of<IShader, Shader>::value,
		"Generic argument for Rende must be a IShader based class");

	auto shader = Shader::Get();

	shader->bind();

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_vertexBuffer.Get());

	if (m_currentIndex)
	{
		m_d3d11Context->IASetPrimitiveTopology(top);
		UINT stride = sizeof(TextureBatchShader::SVertex);
		UINT offset = 0;
		ID3D11ShaderResourceView * SRVs[] = { tex->GetTextureSRV() };
		m_d3d11Context->PSSetShaderResources(0, 1, SRVs);
		m_d3d11Context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_d3d11Context->Draw(m_currentIndex, 0);
		if (g_isDeveloper)
			g_drawCalls++;
	}
}