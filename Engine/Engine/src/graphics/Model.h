#pragma once


#include "interfaces/GameObject.h"


class Direct3D11;

template <class Shader>
class Model : public IGameObject<Shader>
{
	static_assert( std::is_base_of<IShader, Shader>::value,
		"A model has to be created with a valid shader" );
	struct SVertex
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 Texture;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT3 Binormal;
		SVertex() = default;
	};
	friend class Direct3D11;
public:
	Model() {};
	~Model()
	{
		m_vertexBuffer.Reset();
		m_indexBuffer.Reset();
	};

public:
	void Update(float frameTime)
	{
	};

protected:
	void Create(Shader* shader, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		m_shader = shader;

		m_vertices.insert(m_vertices.begin(),
		{
			// No tangent / binormal
			{ { 0.0f,0.5f,0.5f,1.0f },{ 0.5f, 0.0f },{ 0.0f,0.0f,-1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } },
			{ { 0.5f,-0.5f,0.5f,1.0f },{ 1.0f, 1.0f },{ 0.0f,0.0f,-1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } },
			{ { -0.5f,-0.5f,0.5f,1.0f },{ 0.0f, 1.0f },{ 0.0f,0.0f,-1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } },
		});

		m_indices.insert(m_indices.begin(), { 0,1,2 });

		ShaderHelper::CreateBuffer(device, &m_vertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof(SVertex) * 3, 0, &m_vertices[0]);

		ShaderHelper::CreateBuffer(device, &m_indexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			sizeof(decltype( m_indices[0] )) * 3, 0, &m_indices[0]);

		m_objectWorld = DirectX::XMMatrixIdentity();
	};
	void Render(ID3D11DeviceContext* context)
	{
		uint32_t offset = 0;
		uint32_t stride = sizeof(SVertex);
		assert(stride >= 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	};
	void Destroy()
	{
		m_vertexBuffer.Reset();
		m_indexBuffer.Reset();
	};
	uint32_t GetIndexCount() const
	{
		return m_indices.size();
	};
	uint32_t GetVertexCount() const
	{
		return m_vertices.size();
	};


private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	MicrosoftPointer<ID3D11Buffer>	m_indexBuffer;
	
	std::vector<SVertex>			m_vertices;
	std::vector<uint32_t>			m_indices;
};
