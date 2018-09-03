#pragma once


#include "../interfaces/Shader.h"
#include "../interfaces/Object.h"
#include "../interfaces/ICamera.h"
#include "../Shaders/BatchShader.h"
#include "../Texture.h"


class BatchRenderer : public IObject
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
	void End(ICamera *, D3D11_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	uint32_t						m_numMaxVertices = 10000;
	uint32_t						m_currentIndex = 0;
	BatchShader::SVertex*			m_bufferData;
};

class TextureBatchRenderer : public IObject
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

private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	uint32_t						m_numMaxVertices = 10000;
	uint32_t						m_currentIndex = 0;
	TextureBatchShader::SVertex*	m_bufferData;
};