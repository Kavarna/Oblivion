#include "BatchRenderer.h"
#include "../../scripting/LuaManager.h"
#include "../../scripting/Entity.h"

float kPointList = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
float kLineList = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
float kLineStrip = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
float kTriangleList = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
float kTriangleStrip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

void BatchRenderer::LuaRegister()
{
	US_NS_LUA;
	getGlobalNamespace(g_luaState.get())
		.beginNamespace("Oblivion")
			.beginClass<BatchRenderer>("BatchRenderer")
				.addConstructor<void(*)()>()
				.addFunction("Create", &BatchRenderer::Create)
				.addFunction("Begin", &BatchRenderer::Begin)
				.addFunction("End", &BatchRenderer::LuaEnd)
				.addFunction("Point", &BatchRenderer::Point)
				.addFunction("Reconstruct", &BatchRenderer::Reconstruct)
			.endClass()
			.addVariable("TopologyPointList", &kPointList,false)
			.addVariable("TopologyLineList", &kLineList, false)
			.addVariable("TopologyLineStrip", &kLineStrip, false)
			.addVariable("TopologyTriangleList", &kTriangleList, false)
			.addVariable("TopologyTriangleStrip", &kTriangleStrip, false)
		.endNamespace();
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
		sizeof(BatchShader::SVertex) * m_numMaxVertices, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
}

void BatchRenderer::Begin()
{
	m_bufferData = (BatchShader::SVertex*) ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_vertexBuffer.Get());
	m_currentIndex = 0;
}

void BatchRenderer::Vertex(BatchShader::SVertex const & vertex)
{
	Vertex(vertex.Position, vertex.Color);
}

void BatchRenderer::Vertex(DirectX::XMFLOAT3 const & pos, DirectX::XMFLOAT4 const & color)
{
	m_bufferData[m_currentIndex].Position = pos;
	m_bufferData[m_currentIndex].Color = color;
	m_currentIndex++;
	assert(m_currentIndex < m_numMaxVertices);
}

void BatchRenderer::Point(DirectX::XMFLOAT3 const & pos, DirectX::XMFLOAT4 const & color)
{
	Vertex(pos, color);
}

void BatchRenderer::End(ICamera * cam, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	auto shader = BatchShader::Get();

	shader->bind();

	DirectX::XMMATRIX VP = cam->GetView() * cam->GetProjection();
	VP = DirectX::XMMatrixTranspose(VP);
	shader->SetCamera(VP);

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

void BatchRenderer::LuaEnd(float camera, float topology)
{
	int iCam = (int)camera;
	CameraType cam = (CameraType)iCam;
	int iTopology = (int)topology;
	D3D11_PRIMITIVE_TOPOLOGY top = (D3D11_PRIMITIVE_TOPOLOGY)iTopology;

	switch (cam)
	{
	case World:
		End(g_camera.get(), top);
		break;
	case Screen:
		End(g_screen.get(), top);
		break;
	default:
		break;
	}
}

