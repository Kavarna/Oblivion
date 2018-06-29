#include "GameObject.h"
#include "../Direct3D11.h"

using CommonTypes::Range;

std::vector<IGameObject::SVertex>	IGameObject::m_staticVertices;
MicrosoftPointer<ID3D11Buffer>		IGameObject::m_staticVerticesBuffer;

DirectX::XMMATRIX& IGameObject::AddInstance(DirectX::FXMMATRIX const& mat)
{
	m_objectWorld.emplace_back(mat);

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_instanceBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(DirectX::XMMATRIX) * m_objectWorld.size(), 
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
		m_objectWorld.data());
	return m_objectWorld[m_objectWorld.size() - 1];
}

DirectX::XMMATRIX* IGameObject::AddInstance(uint32_t number)
{
	uint32_t start = m_objectWorld.size();
	m_objectWorld.reserve(start + number);
	
	for (uint32_t i = 0; i < number; ++i)
		m_objectWorld.emplace_back(DirectX::XMMatrixIdentity());

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_instanceBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(DirectX::XMMATRIX) * m_objectWorld.size(),
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
		m_objectWorld.data());

	return &m_objectWorld[start];
}

void IGameObject::RemoveInstance(int index)
{
	m_objectWorld.erase(m_objectWorld.begin() + index, m_objectWorld.begin() + index + 1);

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_instanceBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(DirectX::XMMATRIX) * m_objectWorld.size(), 0);
}

void IGameObject::RemoveInstance(CommonTypes::Range const & range)
{
	m_objectWorld.erase(m_objectWorld.begin() + range.begin,
		m_objectWorld.begin() + range.end);
}



Range IGameObject::AddVertices(std::vector<SVertex> & vertices)
{
	auto renderer = Direct3D11::Get();
	Range res;
	res.begin = m_staticVertices.size();
	m_staticVertices.reserve(m_staticVertices.size() + vertices.size());
	m_staticVertices.insert(m_staticVertices.end(), std::make_move_iterator(vertices.begin()),
		std::make_move_iterator(vertices.end()));
	vertices.clear();
	res.end = m_staticVertices.size();
	ShaderHelper::CreateBuffer(renderer->getDevice().Get(),
		&m_staticVerticesBuffer, D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(SVertex) * m_staticVertices.size(), 0,
		m_staticVertices.data());
	return res;
}

void IGameObject::RemoveVertices(Range const & range)
{
	m_staticVertices.erase(m_staticVertices.begin() + range.begin,
		m_staticVertices.begin() + range.end);
}

void IGameObject::BindStaticVertexBuffer()
{
	auto renderer = Direct3D11::Get();
	ID3D11Buffer * buffers[1] =
	{
		m_staticVerticesBuffer.Get()
	};
	UINT stride = sizeof(SVertex);
	UINT offset = 0;
	renderer->getContext()->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
}
