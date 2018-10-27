#include "GameObject.h"
#include "../Direct3D11.h"

using CommonTypes::Range;
using namespace Oblivion;

std::vector<SVertex>				IGameObject::m_staticVertices;
MicrosoftPointer<ID3D11Buffer>		IGameObject::m_staticVerticesBuffer;
std::vector<uint32_t>				IGameObject::m_staticIndices;
MicrosoftPointer<ID3D11Buffer>		IGameObject::m_staticIndicesBuffer;

IGameObject::IGameObject()
{
	static auto bufferManager = BufferManager::Get();
	m_materialBuffer = bufferManager->CreateBuffer(D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		sizeof(Shader::material_t), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
}

void IGameObject::Update(float frameTime)
{
}

uint32_t IGameObject::AddInstance(DirectX::FXMMATRIX const& mat)
{
	m_objectWorld.emplace_back(mat);

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_instanceBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(DirectX::XMMATRIX) * m_objectWorld.size(), 
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
		m_objectWorld.data());
	return (uint32_t)m_objectWorld.size() - 1;
}

uint32_t IGameObject::AddInstance(uint32_t number)
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

	return start;
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

int IGameObject::PrepareInstances(const std::function<bool(uint32_t)> & shouldRender,
	const std::function<DirectX::XMMATRIX(DirectX::FXMMATRIX)>& modifyWorld) const
{
	auto data = (DirectX::XMMATRIX*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	uint32_t renderInstances = 0;
	for (size_t i = 0; i < m_objectWorld.size(); ++i)
	{
		if (shouldRender((uint32_t)i))
		{
			if (modifyWorld)
				data[renderInstances++] = modifyWorld(m_objectWorld[i]);
			else
				data[renderInstances++] = m_objectWorld[i];
		}
	}
	
	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	return renderInstances;
}

void IGameObject::Render(ICamera * cam, const PipelineEnum& p) const
{
	if (!PrepareIA(p))
		return;
	switch (p)
	{
	case PipelineEnum::PipelineBasic:
		RenderBasic(cam);
		break;
	case PipelineEnum::PipelineTexture:
		RenderTexture(cam);
		break;
	case PipelineEnum::PipelineTextureLight:
		RenderTextureLight(cam);
		break;
	case PipelineEnum::PipelineDisplacementTextureLight:
		RenderDisplacementTextureLight(cam);
		break;
	default:
		return;
	}
	DrawIndexedInstanced(cam);
}

void IGameObject::RenderBasic(ICamera * cam) const
{
	m_bindMaterialToShader = (int)Shader::ShaderType::ePixel;
}

void IGameObject::RenderTexture(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();

	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_anisotropicWrapSampler.GetAddressOf());
	m_bindMaterialToShader = (int)Shader::ShaderType::ePixel;
}

void IGameObject::RenderTextureLight(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();

	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	m_bindMaterialToShader = (int)Shader::ShaderType::ePixel;
}

void IGameObject::RenderDisplacementTextureLight(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	m_d3d11Context->DSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	m_bindMaterialToShader  = (int)Shader::ShaderType::ePixel;
	m_bindMaterialToShader |= (int)Shader::ShaderType::eVertex;
	m_bindMaterialToShader |= (int)Shader::ShaderType::eDomain;
}

Range IGameObject::AddVertices(std::vector<SVertex> & vertices)
{
	return AddVertices(vertices, 0, (int)vertices.size());
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

CommonTypes::Range IGameObject::AddIndices(std::vector<uint32_t>& indices)
{
	return AddIndices(indices, 0, (int)indices.size());
}

CommonTypes::Range IGameObject::AddIndices(std::vector<uint32_t>& indices, int start, int end)
{
	auto renderer = Direct3D11::Get();
	Range res;
	res.begin = (uint32_t)m_staticIndices.size();
	m_staticIndices.reserve(m_staticIndices.size() + end - start);
	for (int i = start; i < end; ++i)
		m_staticIndices.push_back(indices[i]);
	res.end = (uint32_t)m_staticIndices.size();
	ShaderHelper::CreateBuffer(renderer->getDevice().Get(),
		&m_staticIndicesBuffer, D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(uint32_t) * m_staticIndices.size(), 0,
		m_staticIndices.data());
	return res;
}

void IGameObject::RemoveIndices(CommonTypes::Range const & range)
{
	m_staticIndices.erase(m_staticIndices.begin() + range.begin,
		m_staticIndices.begin() + range.end);
}

void IGameObject::BindMaterial(Rendering::material_t const & mat, int shader) const
{
	static auto bufferManager = BufferManager::Get();
	auto data = (Shader::material_t*)bufferManager->MapBuffer(m_materialBuffer);

	data->color			= mat.diffuseColor;
	data->hasTexture	= mat.hasTexture;
	data->hasBump		= mat.hasBumpMap;
	data->hasSpecular	= mat.hasSpecularMap;
	data->specular		= mat.specular;
	data->color			= mat.diffuseColor;
	data->tessMin		= mat.tessMin;
	data->tessMax		= mat.tessMax;
	data->tessScale		= mat.tessScale;

	bufferManager->UnmapBuffer(m_materialBuffer);

	ID3D11ShaderResourceView *resources[] =
	{
		mat.diffuseTexture ? mat.diffuseTexture->GetTextureSRV() : nullptr,
		mat.bumpMap ? mat.bumpMap->GetTextureSRV() : nullptr,
		mat.specularMap ? mat.specularMap->GetTextureSRV() : nullptr,
	};


	bufferManager->bindMaterial(shader, m_materialBuffer);
	ShaderHelper::BindResources(m_d3d11Context.Get(), 0, 3, resources, shader);
}

void IGameObject::BindStaticVertexBuffer()
{
	auto renderer = Direct3D11::Get();
	ID3D11Buffer * vertexBuffer[1] =
	{
		m_staticVerticesBuffer.Get()
	};
	UINT stride = sizeof(SVertex);
	UINT offset = 0;
	renderer->getContext()->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);
	renderer->getContext()->IASetIndexBuffer(m_staticIndicesBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
}
