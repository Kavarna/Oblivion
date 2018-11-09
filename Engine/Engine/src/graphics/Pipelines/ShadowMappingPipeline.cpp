#include "ShadowMappingPipeline.h"
#include "../ShadowmapBuild.h"

void ShadowMappingPipeline::setShadowMap(const Texture * tex, DirectionalLightView * directionalLight)
{
	m_shadowmapTexture = tex;
	m_directionalLight = directionalLight;
}

void __vectorcall ShadowMappingPipeline::bind(ICamera * cam) const
{
	clearPipeline();
	if (m_directionalLight)
	{
		static auto bm = BufferManager::Get();
		static auto renderer = Direct3D11::Get();
		static auto vertexShader = InstancedVertexShader::Get();
		static auto hullShader = DisplacementMappingHullShader::Get();
		static auto domainShader = DisplacementMappingDomainShader::Get();
		static auto pixelShader = ShadowMappingPixelShader::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
				cam->GetView(),
				cam->GetProjection()
			});
		vertexShader->SetCameraAdditionalInfo({
				cam->GetPosition(),
				cam->GetFarZ(),
				cam->GetNearZ()
			});
		vertexShader->SetAdditionalCamera({
				m_directionalLight->GetView(),
				m_directionalLight->GetProjection()
			});

		ID3D11ShaderResourceView * shadowmap[] =
		{
			m_shadowmapTexture->GetTextureSRV()
		};

		m_d3d11Context->PSSetShaderResources(4, 1, shadowmap);
		m_d3d11Context->PSSetSamplers(1, 1, renderer->m_linearClampSampler.GetAddressOf());
		pixelShader->bind();

		if (useDisplacement())
		{
			ConstantBufferHandle cameraBuffer = vertexShader->GetCameraBuffer();
			bm->bindDSBuffer(0, cameraBuffer);
			hullShader->bind();
			domainShader->bind();
		}
		else
		{
			hullShader->unbind();
			domainShader->unbind();
		}


	}
	else
	{
		if (useDisplacement())
			DisplacementLightPipeline::Get()->bind(cam);
		else
			TextureLightPipeline::Get()->bind(cam);
	}
}

void __vectorcall ShadowMappingPipeline::bind(DirectX::FXMMATRIX & world, ICamera * cam) const
{
	clearPipeline();
	if (m_directionalLight)
	{
		static auto bm = BufferManager::Get();
		static auto renderer = Direct3D11::Get();
		static auto vertexShader = SimpleVertexShader::Get();
		static auto hullShader = DisplacementMappingHullShader::Get();
		static auto domainShader = DisplacementMappingDomainShader::Get();
		static auto pixelShader = ShadowMappingPixelShader::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
				world,
				cam->GetView(),
				cam->GetProjection()
			});
		vertexShader->SetCameraAdditionalInfo({
				cam->GetPosition(),
				cam->GetFarZ(),
				cam->GetNearZ()
			});
		vertexShader->SetAdditionalCamera({
				m_directionalLight->GetView(),
				m_directionalLight->GetProjection()
			});
		ID3D11ShaderResourceView * shadowmap[] =
		{
			m_shadowmapTexture->GetTextureSRV()
		};

		m_d3d11Context->PSSetShaderResources(4, 1, shadowmap);
		m_d3d11Context->PSSetSamplers(1, 1, renderer->m_linearClampSampler.GetAddressOf());
		m_d3d11Context->PSSetSamplers(2, 1, renderer->m_comparisonLinearClampSampler.GetAddressOf());
		pixelShader->bind();

		if (useDisplacement())
		{
			domainShader->setCamera({
				cam->GetView(),
				cam->GetProjection()
				});
			hullShader->bind();
			domainShader->bind();
		}
		else
		{
			hullShader->unbind();
			domainShader->unbind();
		}


	}
	else
	{
		if (useDisplacement())
			DisplacementLightPipeline::Get()->bind(cam);
		else
			TextureLightPipeline::Get()->bind(cam);
	}
}
