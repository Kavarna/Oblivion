#include "GameObject.h"
#include "../../scripting/Entity.h"
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

void IGameObject::Update(float frameTime)
{
	if (isEntity())
	{
		m_entity->OnUpdate(frameTime);
	}
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

CommonTypes::Range IGameObject::MakeEntity(Entity * e, int numInstances)
{
	CommonTypes::Range range;
	range.begin = m_objectWorld.size();
	m_entity = e;
	AddInstance(numInstances);
	range.end = m_objectWorld.size();
	return range;
	//return (uint32_t)m_objectWorld.size() - 1;
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

int IGameObject::PrepareInstances(std::function<bool(uint32_t)> & shouldRender) const
{
	auto data = (DirectX::XMMATRIX*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	uint32_t renderInstances = 0;
	if (isEntity())
	{
		for (size_t i = 0; i < m_objectWorld.size(); ++i)
		{
			if (shouldRender((uint32_t)i))
			{
				if (m_entity->OnRender())
				{
					data[renderInstances++] = m_objectWorld[i];
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_objectWorld.size(); ++i)
		{
			if (shouldRender((uint32_t)i))
			{
				data[renderInstances++] = m_objectWorld[i];
			}
		}
	}
	
	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	return renderInstances;
}

void IGameObject::Render(ICamera * cam, const Pipeline& p) const
{
	if (!PrepareIA(p))
		return;
	switch (p)
	{
	case Pipeline::PipelineBasic:
		RenderBasic(cam);
		break;
	case Pipeline::PipelineTexture:
		RenderTexture(cam);
		break;
	case Pipeline::PipelineTextureLight:
		RenderTextureLight(cam);
		break;
	case Pipeline::PipelineDisplacementTextureLight:
		RenderDisplacementTextureLight(cam);
		break;
	default:
		return;
	}
	DrawIndexedInstanced(cam, p);
}

void IGameObject::Render() const
{
	if (isEntity())
	{
		Render(m_entity->m_cameraToUse, m_entity->m_pipelineToUse);
	}
}

bool IGameObject::isEntity() const
{
	return (m_entity != nullptr);
}

void IGameObject::RenderBasic(ICamera * cam) const
{
	static BasicShader * shader = BasicShader::Get();
	shader->bind();
	static DirectX::XMMATRIX VP;
	VP = cam->GetView() * cam->GetProjection();
	VP = DirectX::XMMatrixTranspose(VP);
	shader->SetCameraInformations({ VP });
	m_bindMaterialToShader = (int)Shader::ShaderType::ePixel;
}

void IGameObject::RenderTexture(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	static TextureShader * shader = TextureShader::Get();
	shader->bind();
	shader->SetCameraInformations({
		DirectX::XMMatrixTranspose(cam->GetView()),
		DirectX::XMMatrixTranspose(cam->GetProjection())
		});

	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	m_bindMaterialToShader = (int)Shader::ShaderType::ePixel;
}

void IGameObject::RenderTextureLight(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	static TextureLightShader * shader = TextureLightShader::Get();
	shader->bind();
	shader->SetCameraInformations({
		DirectX::XMMatrixTranspose(cam->GetView()),
		DirectX::XMMatrixTranspose(cam->GetProjection())
		});

	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	m_bindMaterialToShader = (int)Shader::ShaderType::ePixel;
}

void IGameObject::RenderDisplacementTextureLight(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	static DisplacementShader * shader = DisplacementShader::Get();
	shader->bind();
	shader->SetCameraInfo(cam);
	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	m_d3d11Context->DSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	m_bindMaterialToShader  = (int)Shader::ShaderType::ePixel;
	m_bindMaterialToShader |= (int)Shader::ShaderType::eVertex;
	m_bindMaterialToShader |= (int)Shader::ShaderType::eDomain;
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

	data->color			= mat.diffuseColor;
	data->hasTexture	= mat.hasTexture;
	data->hasBump		= mat.hasBumpMap;
	data->hasSpecular	= mat.hasSpecularMap;
	data->specular		= mat.specular;
	data->color			= mat.diffuseColor;
	data->tessMin		= mat.tessMin;
	data->tessMax		= mat.tessMax;
	data->tessScale		= mat.tessScale;

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_materialBuffer.Get());

	ID3D11ShaderResourceView *resources[] =
	{
		mat.diffuseTexture ? mat.diffuseTexture->GetTextureSRV() : nullptr,
		mat.bumpMap ? mat.bumpMap->GetTextureSRV() : nullptr,
		mat.specularMap ? mat.specularMap->GetTextureSRV() : nullptr,
	};


	ShaderHelper::BindConstantBuffer(m_d3d11Context.Get(), 2, 1, m_materialBuffer.GetAddressOf(), shader);
	ShaderHelper::BindResources(m_d3d11Context.Get(), 0, 3, resources, shader);
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
