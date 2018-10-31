#pragma once


#include "TextureLightPipeline.h"
#include "../Shaders/DisplacementMappingHullShader.h"
#include "../Shaders/DisplacementMappingDomainShader.h"
#include "../Shaders/DisplacementMappingPixelShader.h"

class DisplacementLightPipeline : public TextureLightPipeline
{
public:
	DisplacementLightPipeline()
	{
		m_usedStages = (uint32_t)Shader::ShaderType::ePixel |
			(uint32_t)Shader::ShaderType::eVertex |
			(uint32_t)Shader::ShaderType::eHull |
			(uint32_t)Shader::ShaderType::eDomain;
	}
	MAKE_SINGLETONE(DisplacementLightPipeline);
public:
	virtual void __vectorcall bind(ICamera * cam) const override
	{
		static auto bm = BufferManager::Get();
		clearPipeline();
		auto vertexShader = InstancedVertexShader::Get();
		auto pixelShader = DisplacementMappingPixelShader::Get();
		auto hullShader = DisplacementMappingHullShader::Get();
		auto domainShader = DisplacementMappingDomainShader::Get();
		ConstantBufferHandle cameraBuffer = vertexShader->GetCameraBuffer();
		bm->bindDSBuffer(0, cameraBuffer);
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
		pixelShader->bind();
		pixelShader->bindLightBuffer();
		hullShader->bind();
		domainShader->bind();
	}
	virtual void __vectorcall bind(DirectX::FXMMATRIX & world, ICamera * cam) const override
	{
		clearPipeline();
		auto vertexShader = SimpleVertexShader::Get();
		auto pixelShader = DisplacementMappingPixelShader::Get();
		auto hullShader = DisplacementMappingHullShader::Get();
		auto domainShader = DisplacementMappingDomainShader::Get();
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
		domainShader->setCamera({
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
		pixelShader->bindLightBuffer();
		hullShader->bind();
		domainShader->bind();
	}
};