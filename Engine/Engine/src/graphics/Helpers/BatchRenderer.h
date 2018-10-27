#pragma once


#include "../interfaces/Shader.h"
#include "../interfaces/GraphicsObject.h"
#include "../interfaces/ICamera.h"
#include "../Texture.h"

#include "../Pipelines/Pipeline.h"


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
	void Vertex(DirectX::XMFLOAT3 const& pos, DirectX::XMFLOAT4 const& color);
	//void End(ICamera *, D3D11_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	/// <summary>Shader should have everything set up</summary>
	template <class Pipeline>
	void End(ICamera *, D3D11_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
private:
	void ReconstructAndCopy(uint32_t newSize);

private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	uint32_t						m_numMaxVertices = 10000;
	uint32_t						m_currentIndex = 0;
	Oblivion::vertex_t*				m_bufferData = nullptr;
};

template <class Pipeline>
void BatchRenderer::End(ICamera * cam, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	static_assert(std::is_base_of<IPipeline, Pipeline>::value,
		"Generic argument for Rende must be a IShader based class");

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_vertexBuffer.Get());

	Pipeline::Get()->bind(DirectX::XMMatrixIdentity(), cam);

	if (m_currentIndex)
	{
		m_d3d11Context->IASetPrimitiveTopology(topology);
		static UINT stride = sizeof(Oblivion::vertex_t);
		static UINT offset = 0;
		m_d3d11Context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_d3d11Context->Draw(m_currentIndex, 0);
		if (g_isDeveloper)
			g_drawCalls++;
	}
}