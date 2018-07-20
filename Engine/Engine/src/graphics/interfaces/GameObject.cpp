#include "GameObject.h"
#include "../Direct3D11.h"

using CommonTypes::Range;
using namespace Oblivion;

std::vector<SVertex>				IGameObject::m_staticVertices;
MicrosoftPointer<ID3D11Buffer>		IGameObject::m_staticVerticesBuffer;

IGameObject::IGameObject()
{
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_materialBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(Shader::material_t), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
}

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
	uint32_t start = (uint32_t)m_objectWorld.size();
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
	res.begin = (uint32_t)m_staticVertices.size();
	m_staticVertices.reserve(m_staticVertices.size() + vertices.size());
	m_staticVertices.insert(m_staticVertices.end(), vertices.begin(),
		vertices.end());
	res.end = (uint32_t)m_staticVertices.size();
	ShaderHelper::CreateBuffer(renderer->getDevice().Get(),
		&m_staticVerticesBuffer, D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(SVertex) * m_staticVertices.size(), 0,
		m_staticVertices.data());
	return res;
}

CommonTypes::Range IGameObject::AddVertices(std::vector<Oblivion::SVertex>& vertices, int start, int end)
{
	assert(end > start);
	auto renderer = Direct3D11::Get();
	Range res;
	res.begin = (uint32_t)m_staticVertices.size();
	m_staticVertices.reserve(m_staticVertices.size() + end - start);
	for (int i = start; i < end; ++i)
		m_staticVertices.push_back(vertices[i]);
	res.end = (uint32_t)m_staticVertices.size();
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

void IGameObject::BindMaterial(Rendering::material_t const & mat, int shader) const
{
	auto data = (Shader::material_t*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_materialBuffer.Get());

	data->color = mat.diffuseColor;
	data->hasTexture = mat.hasTexture;
	data->hasBump = mat.hasBumpMap;
	data->hasSpecular = mat.hasSpecularMap;
	data->specular = mat.specular;
	data->color = mat.diffuseColor;

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_materialBuffer.Get());

	ID3D11ShaderResourceView *resources[] =
	{
		mat.diffuseTexture ? mat.diffuseTexture->GetTextureSRV() : nullptr,
		mat.bumpMap ? mat.bumpMap->GetTextureSRV() : nullptr,
		mat.specularMap ? mat.specularMap->GetTextureSRV() : nullptr,
	};

	if (shader & (int)Shader::ShaderType::eVertex)
	{
		m_d3d11Context->VSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->VSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::eHull)
	{
		m_d3d11Context->HSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->HSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::eDomain)
	{
		m_d3d11Context->DSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->DSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::eGeometry)
	{
		m_d3d11Context->GSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->GSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::ePixel)
	{
		m_d3d11Context->PSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->PSSetShaderResources(0, 3, resources);
	}
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
