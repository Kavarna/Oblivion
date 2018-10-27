#include "BatchRenderer.h"

BatchRenderer::BatchRenderer()
{
}


BatchRenderer::~BatchRenderer()
{
}

void BatchRenderer::Create()
{
	Reconstruct(m_numMaxVertices);
}

void BatchRenderer::Reconstruct(uint32_t newSize)
{
	m_numMaxVertices = newSize;
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_vertexBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(Oblivion::vertex_t) * m_numMaxVertices, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
}

void BatchRenderer::Begin()
{
	m_bufferData = (Oblivion::vertex_t*) ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_vertexBuffer.Get());
	m_currentIndex = 0;
}

void BatchRenderer::Vertex(DirectX::XMFLOAT3 const & pos, DirectX::XMFLOAT4 const & color)
{
	m_bufferData[m_currentIndex].Position = pos;
	m_bufferData[m_currentIndex].TexC = color;
	m_currentIndex++;
	if (m_currentIndex >= m_numMaxVertices)
		ReconstructAndCopy(m_numMaxVertices * 10);
}

void BatchRenderer::ReconstructAndCopy(uint32_t newSize)
{
	if (m_bufferData)
	{
		ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_vertexBuffer.Get());
		std::vector<Oblivion::vertex_t> vertices((Oblivion::vertex_t*)m_bufferData, (Oblivion::vertex_t*)m_bufferData + m_numMaxVertices);
		Reconstruct(newSize);
		m_bufferData = (Oblivion::vertex_t*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_vertexBuffer.Get());
		for (size_t i = 0; i < vertices.size(); ++i)
			m_bufferData[i] = vertices[i];
		
	}
	else
		Reconstruct(newSize);

}