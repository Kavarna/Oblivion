#pragma once


#include "Pipeline.h"

#include "../Shaders/SimpleVertexShader.h"
#include "../Shaders/InstancedVertexShader.h"
#include "../Shaders/RejectPixelShader.h"
#include "../Shaders/DisplacementMappingHullShader.h"
#include "../Shaders/DisplacementMappingDomainShader.h"



class DepthmapPipeline : public IPipeline, public Singletone<DepthmapPipeline>
{
public:
	DepthmapPipeline()
	{
		m_usedStages = (uint32_t)Shader::ShaderType::eVertex |
			(uint32_t)Shader::ShaderType::eHull | (uint32_t)Shader::ShaderType::eDomain;
	}

private:
	bool m_useDisplacement = false;

public:

	void ToggleDisplacement() { m_useDisplacement = !m_useDisplacement; };
	void EnableDisplacement() { m_useDisplacement = true; };
	void DisableDisplacement() { m_useDisplacement = false; };

	bool useDisplacement() { return m_useDisplacement; };

	// Inherited via IPipeline
	virtual void __vectorcall bind(ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = InstancedVertexShader::Get();
		static auto pixelShader = RejectPixelShader::Get();
		static auto domainShader = DisplacementMappingDomainShader::Get();
		static auto hullShader = DisplacementMappingHullShader::Get();
		static auto bm = BufferManager::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
		if (m_useDisplacement)
		{
			domainShader->bind();
			ConstantBufferHandle cameraBuffer = vertexShader->GetCameraBuffer();
			bm->bindDSBuffer(0, cameraBuffer);
			hullShader->bind();
		}
		else
		{
			domainShader->unbind();
			hullShader->unbind();
		}
	}

	virtual void __vectorcall bind(DirectX::FXMMATRIX & world, ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = SimpleVertexShader::Get();
		static auto pixelShader = RejectPixelShader::Get();
		static auto domainShader = DisplacementMappingDomainShader::Get();
		static auto hullShader = DisplacementMappingHullShader::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
			world,
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
		if (m_useDisplacement)
		{
			domainShader->bind();
			domainShader->setCamera({
				cam->GetView(),
				cam->GetProjection()
				});
			hullShader->bind();
		}
		else
		{
			domainShader->unbind();
			hullShader->unbind();
		}
	}
	
	void setUserInfo(ICamera * cam)
	{
		static auto vertexShader = SimpleVertexShader::Get();
		vertexShader->SetCameraAdditionalInfo(
			{
				cam->GetPosition(),
				cam->GetFarZ(),
				cam->GetNearZ()
			}
		);
	}

};