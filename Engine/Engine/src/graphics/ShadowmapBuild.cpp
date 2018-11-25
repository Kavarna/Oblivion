#include "ShadowmapBuild.h"
#include "Pipelines/DepthmapPipeline.h"

ShadowmapBuild::ShadowmapBuild(DirectionalLightView * light, INT depthBias, FLOAT slopeScaledDepthBias, FLOAT depthBiasClamp)
{
	SetLightView(light);
	Create((uint32_t)light->m_width, depthBias, slopeScaledDepthBias, depthBiasClamp);
}

ShadowmapBuild::ShadowmapBuild(uint32_t shadowMapsize, INT depthBias, FLOAT slopeScaledDepthBias, FLOAT depthBiasClamp)
{
	Create(shadowMapsize, depthBias, slopeScaledDepthBias, depthBiasClamp);
}

void ShadowmapBuild::Create(uint32_t shadowmapSize, INT depthBias, FLOAT slopeScaledDepthBias, FLOAT depthBiasClamp)
{
	ZeroMemoryAndDeclare(D3D11_RASTERIZER_DESC, rastDesc);
	rastDesc.AntialiasedLineEnable = FALSE;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.DepthBias = depthBias;
	rastDesc.DepthBiasClamp = depthBiasClamp;
	rastDesc.SlopeScaledDepthBias = slopeScaledDepthBias;
	rastDesc.DepthClipEnable = TRUE;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE; // TODO: Make it dynamically changeable
	ThrowIfFailed(
		m_d3d11Device->CreateRasterizerState(&rastDesc,&m_rasterizer)
	);

	m_renderTexture = std::make_unique<RenderTexture>(shadowmapSize, shadowmapSize,
		(RenderTextureFlags)(RenderTextureFlags::DepthReadEnable | RenderTextureFlags::DepthWriteEnable));
}

void ShadowmapBuild::Build()
{
	static auto renderer = Direct3D11::Get();
	ID3D11ShaderResourceView * nullSRV[] =
	{
		nullptr
	};
	m_d3d11Context->PSSetShaderResources(4, 1, nullSRV);

	renderer->RSCustomState(m_rasterizer.Get());

	m_renderTexture->SetRenderTarget();
	m_renderTexture->Clear();

	for (const auto & gameObject : m_occluders)
	{
		gameObject->Render<DepthmapPipeline>(m_lightView);
	}
	renderer->RSLastState();
}

void ShadowmapBuild::AddGameObject(const IGameObject * gameObject)
{
	m_occluders.push_back(gameObject);
}
