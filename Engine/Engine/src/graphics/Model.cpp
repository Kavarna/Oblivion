#include "Model.h"
#include "Helpers/GeometryGenerator.h"


Model::Model()
{
	auto renderer = Direct3D11::Get();
	m_d3d11Device = renderer->getDevice();
	m_d3d11Context = renderer->getContext();
}

Model::~Model()
{
	Destroy();
}


void Model::Destroy()
{
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}

inline uint32_t Model::GetIndexCount() const
{
	return m_indices.size();
}

inline uint32_t Model::GetVertexCount() const
{
	return m_vertices.size();
}

void Model::Create(LPWSTR lpPath)
{
	return; // HANDLE OBJ LOADING
	if (lpPath == L"")
	{
		GeometryGenerator g;
		GeometryGenerator::MeshData data;
		g.CreateBox(1.0f, 1.0f, 1.0f, data);
		m_vertices = std::move(data.Vertices);
		m_indices = std::move(data.Indices);
		try
		{
			m_texture = std::make_unique<Texture>((LPWSTR)L"Resources/Marble.jpg", m_d3d11Device.Get(), m_d3d11Context.Get());
		}
		catch (...)
		{
			DX::OutputVDebugString(L"Can't open texture: \"Resources/Marble.jpg\"\n");
			throw std::exception("Can't find resources");
		}
	}

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_vertexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(SVertex) * m_vertices.size(), 0, &m_vertices[0]);

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(decltype(m_indices[0])) * m_indices.size(), 0, &m_indices[0]);

	m_objectWorld = DirectX::XMMatrixIdentity();
}

void Model::Create(EDefaultObject object)
{
	GeometryGenerator g;
	GeometryGenerator::MeshData data;
	if (object == EDefaultObject::Box)
		g.CreateBox(1.0f, 1.0f, 1.0f, data);
	else if (object == EDefaultObject::Cylinder)
		g.CreateCylinder(1.0f, 1.0f, 3.0f, 32, 32, data);
	else if (object == EDefaultObject::FullscreenQuad)
		g.CreateFullscreenQuad(data);
	else if (object == EDefaultObject::Geosphere)
		g.CreateGeosphere(1.0f, 12, data);
	else if (object == EDefaultObject::Grid)
		g.CreateGrid(100.0f, 100.0f, 20, 20, data);
	else if (object == EDefaultObject::Sphere)
		g.CreateSphere(1.0f, 32, 32, data);
	m_vertices = std::move(data.Vertices);
	m_indices = std::move(data.Indices);
	try
	{
		if (object != EDefaultObject::Grid)
			m_texture = std::make_unique<Texture>((LPWSTR)L"Resources/Marble.jpg", m_d3d11Device.Get(), m_d3d11Context.Get());
		else
			m_texture = std::make_unique<Texture>((LPWSTR)L"Resources/Grass.jpg", m_d3d11Device.Get(), m_d3d11Context.Get());
	}
	catch (...)
	{
		DX::OutputVDebugString(L"Can't open texture: \"Resources/Marble.jpg\"\n");
		throw std::exception("Can't find resources");
	}

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_vertexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(SVertex) * m_vertices.size(), 0, &m_vertices[0]);

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(decltype(m_indices[0])) * m_indices.size(), 0, &m_indices[0]);

	m_objectWorld = DirectX::XMMatrixIdentity();
}