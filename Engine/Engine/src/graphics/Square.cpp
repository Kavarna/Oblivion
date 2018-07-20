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
	m_material.diffuseTexture = std::make_unique<Texture>((LPSTR)texture.c_str(),m_d3d11Device.Get(),m_d3d11Context.Get());
}

void Square::Create()
{
	m_material.hasTexture = FALSE;
	m_material.diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Square::Update(float frameTime)
{
}

void Square::Destroy()
{
}

uint32_t Square::GetIndexCount(int subObject) const
{
	return m_indices.size();
}

uint32_t Square::GetVertexCount(int subObject) const
{
	return m_vertexRange.end - m_vertexRange.begin;
}

void Square::RenderTexture(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	static TextureShader * shader = TextureShader::Get();
	
	DirectX::XMMATRIX view = cam->GetView();
	DirectX::XMMATRIX projection = cam->GetProjection();

	shader->SetCameraInformations({
		DirectX::XMMatrixTranspose(view),
		DirectX::XMMatrixTranspose(projection)
		});

	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSamplerState.GetAddressOf());
}

void Square::DrawIndexed(ICamera * cam) const
{
	auto data = (DirectX::XMMATRIX*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	uint32_t renderInstances = 0;
	for (size_t i = 0; i < m_objectWorld.size(); ++i)
	{
		//if (ShouldRenderInstance(cam, (uint32_t)i))
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
	if (renderInstances == 0)
		return;

	BindMaterial(m_material, (int)Shader::ShaderType::ePixel);
	m_d3d11Context->DrawIndexedInstanced(GetIndexCount(),
		renderInstances, 0, m_vertexRange.begin, 0);
	g_drawCalls++;
}
