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
	m_indexBuffer.Reset();
}

inline uint32_t Model::GetIndexCount(int subObject) const
{
	return uint32_t(m_startIndices[subObject].end - m_startIndices[subObject].begin);
}

inline uint32_t Model::GetVertexCount() const
{
	return m_vertices.size();
}

bool Model::ShouldRenderInstance() const
{
	return true;
}

void Model::RenderBasicShader(ICamera* cam) const
{
	static BasicShader * shader = BasicShader::Get();
	static DirectX::XMMATRIX VP;
	VP = cam->GetView() * cam->GetProjection();
	VP = DirectX::XMMatrixTranspose(VP);
	shader->SetCameraInformations({ VP });
}

void Model::RenderTextureLightShader(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	static TextureLightShader * shader = TextureLightShader::Get();
	shader->SetCameraInformations({
		DirectX::XMMatrixTranspose(cam->GetView()),
		DirectX::XMMatrixTranspose(cam->GetProjection())
		});

	ID3D11ShaderResourceView * textures[1] =
	{
		m_texture->GetTextureSRV()
	};
	m_d3d11Context->PSSetShaderResources(0, 1, textures);
	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSamplerState.GetAddressOf());
}

void Model::DrawIndexedInstanced() const
{
	auto data = (DirectX::XMMATRIX*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	uint32_t renderInstances = 0;
	for (size_t i = 0; i < m_objectWorld.size(); ++i)
	{
		if (ShouldRenderInstance())
		{
			data[renderInstances++] = m_objectWorld[i];
		}
	}
	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	ID3D11Buffer * instances[] =
	{
		m_instanceBuffer.Get()
	};
	UINT stride = sizeof(DirectX::XMMATRIX);
	UINT offset = 0;
	m_d3d11Context->IASetVertexBuffers(1, 1, instances, &stride, &offset);
	for (size_t i = 0; i < m_startIndices.size(); ++i)
	{
		m_d3d11Context->DrawIndexedInstanced(GetIndexCount(i), renderInstances, 0, m_verticesRange.begin, 0);
	}
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


	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(decltype(m_indices[0])) * m_indices.size(), 0, &m_indices[0]);
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
		g.CreateSphere(1.0f, 20, 20, data);
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

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(decltype(m_indices[0])) * m_indices.size(), 0, &m_indices[0]);

	m_verticesRange = AddVertices(m_vertices);
	m_startIndices.emplace_back(0, m_indices.size());
}