#include "Square.h"
#include "Helpers/GeometryGenerator.h"


Square::Square()
{
	GeometryGenerator gg;
	GeometryGenerator::MeshData data;
	gg.CreateFullscreenQuad(data);

	m_indices = std::move(data.Indices);

	m_vertexRange = AddVertices(data.Vertices);

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(uint32_t) * m_indices.size(), 0, m_indices.data());
}


Square::~Square()
{
}

void Square::Create(std::string const & texture)
{
	m_material.hasTexture = TRUE;
	m_material.diffuseTexture = std::make_unique<Texture>((LPSTR)texture.c_str());
}

void Square::Create()
{
	m_material.hasTexture = FALSE;
	m_material.diffuseColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

void Square::Update(float frameTime)
{
	IGameObject::Update(frameTime);
}

void Square::Destroy()
{
}

uint32_t Square::GetIndexCount(int subObject) const
{
	return (uint32_t)m_indices.size();
}

uint32_t Square::GetVertexCount(int subObject) const
{
	return m_vertexRange.end - m_vertexRange.begin;
}

void Square::SetTexture(std::shared_ptr<Texture> tex)
{
	m_material.hasTexture = true;
	m_material.diffuseTexture = tex;
}

inline void Square::Scale(float Sx, float Sy, float Sz, int instanceID)
{
	m_width = Sx / 2.0f;
	m_height = Sy / 2.0f;
	m_objectWorld[instanceID] *= DirectX::XMMatrixScaling(m_width, m_height, Sz);
}

void Square::TranslateTo(float X, float Y, int InstanceID)
{
	Identity(InstanceID);
	float halfWidth = (float)m_width;
	float halfHeight = (float)m_height;
	X += halfWidth;
	Y += halfHeight;
	float NewX = (float(m_windowWidth / 2) * -1 + X);
	float NewY = (float(m_windowHeight / 2) - Y);
	m_objectWorld[InstanceID] *= DirectX::XMMatrixScaling(m_width, m_height, 1.0f);
	m_objectWorld[InstanceID] *= DirectX::XMMatrixTranslation(NewX, NewY, 0.0f);
}

void Square::DrawIndexedInstanced(ICamera * cam) const
{
	std::function<bool(uint32_t)> func = [](int) ->bool { return true; };
	int renderInstances = PrepareInstances(func);
	ID3D11Buffer * instances[] =
	{
		m_instanceBuffer.Get()
	};
	UINT stride = sizeof(DirectX::XMMATRIX);
	UINT offset = 0;
	m_d3d11Context->IASetVertexBuffers(1, 1, instances, &stride, &offset);
	if (renderInstances == 0)
		return;

	BindMaterial(m_material, m_bindMaterialToShader);
	m_d3d11Context->DrawIndexedInstanced(GetIndexCount(),
		renderInstances, 0, m_vertexRange.begin, 0);
	if (g_isDeveloper)
		g_drawCalls++;
}

bool Square::PrepareIA(const Pipeline & p) const
{
	if (p == Pipeline::PipelineTexture)
	{
		m_d3d11Context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return true;
	}
	// TODO: LOG
	return false;
}
