#pragma once


#include "interfaces/GameObject.h"
#include "Direct3D11.h"
#include "BasicShader.h"

template <class Shader>
class Model : public IGameObject<Shader>
{
	struct SVertex
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 Texture;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT3 Binormal;
		SVertex() = default;
	};
public:
	Model();
	~Model();

public:
	void Update(float frameTime)
	{
	};

public:
	void Create(LPWSTR lpPath);
	void Render(ICamera * cam, int instanceCount = 1) const;
	void Destroy();

public:
	inline uint32_t GetIndexCount() const;
	inline uint32_t GetVertexCount() const;


private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	MicrosoftPointer<ID3D11Buffer>	m_indexBuffer;
	
	std::vector<uint32_t>			m_startIndices;
	std::vector<SVertex>			m_vertices;
	std::vector<uint32_t>			m_indices;
};

template<class Shader>
Model<Shader>::Model()
{
	auto renderer = Direct3D11::GetInstance();
	m_d3d11Device = renderer->getDevice();
	m_d3d11Context = renderer->getContext();
}

template<class Shader>
Model<Shader>::~Model()
{
	Destroy();
}

template<class Shader>
void Model<Shader>::Destroy()
{
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}

template <class Shader>
inline uint32_t Model<Shader>::GetIndexCount() const
{
	return m_indices.size();
}

template <class Shader>
inline uint32_t Model<Shader>::GetVertexCount() const
{
	return m_vertices.size();
}

template <class Shader>
void Model<Shader>::Create(LPWSTR lpPath)
{
	if (lpPath == L"")
	{
		m_vertices.insert(m_vertices.begin(),
			{
				// No tangent / binormal
				{ { 0.0f,0.5f,0.5f,1.0f },{ 0.5f, 0.0f },{ 0.0f,0.0f,-1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } },
			{ { 0.5f,-0.5f,0.5f,1.0f },{ 1.0f, 1.0f },{ 0.0f,0.0f,-1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } },
			{ { -0.5f,-0.5f,0.5f,1.0f },{ 0.0f, 1.0f },{ 0.0f,0.0f,-1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } },
			});

		m_indices.insert(m_indices.begin(), { 0,1,2 });
	}

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_vertexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(SVertex) * 3, 0, &m_vertices[0]);

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(decltype(m_indices[0])) * 3, 0, &m_indices[0]);

	m_objectWorld = DirectX::XMMatrixIdentity();
}

template <class Shader>
void Model<Shader>::Render(ICamera * cam, int instanceCount) const
{
	if constexpr (std::is_same<Shader,BasicShader>::value)
	{
		BasicShader * shader = Shader::Get();
		shader->bind();
		static BasicShader::SCameraInfo camInfo;
		static DirectX::XMMATRIX WVP;
		WVP = m_objectWorld * cam->GetView() * cam->GetProjection();
		WVP = DirectX::XMMatrixTranspose(WVP);
		shader->SetCameraInformations({ WVP });
		
		uint32_t offset = 0;
		uint32_t stride = sizeof(SVertex);
		assert(stride >= 0);
		m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_d3d11Context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_d3d11Context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		m_d3d11Context->DrawIndexed(GetIndexCount(), 0, 0);
	}
}