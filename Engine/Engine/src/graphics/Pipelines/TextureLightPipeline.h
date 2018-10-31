#pragma once


#include "Pipeline.h"
#include "../Helpers/Lights.h"
#include "../Shaders/InstancedVertexShader.h"
#include "../Shaders/SimpleVertexShader.h"
#include "../Shaders/TextureLightPixelShader.h"

class TextureLightPipeline : public IPipeline
{
public:
	TextureLightPipeline()
	{
		m_usedStages = (uint32_t)Shader::ShaderType::ePixel |
			(uint32_t)Shader::ShaderType::eVertex;
	}

	MAKE_SINGLETONE(TextureLightPipeline);

public:

	void setSunLight(const Sun& s)
	{
		TextureLightPixelShader::Get()->SetLight(s);
	}

	// Inherited via IPipeline
	virtual void __vectorcall bind(ICamera * cam) const override
	{
		clearPipeline();
		auto vertexShader = InstancedVertexShader::Get();
		auto pixelShader = TextureLightPixelShader::Get();
		vertexShader->bind();
		vertexShader->SetCamera({
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
		pixelShader->bindLightBuffer();
	}
	virtual void __vectorcall bind(DirectX::FXMMATRIX & world, ICamera * cam) const override
	{
		clearPipeline();
		auto vertexShader = SimpleVertexShader::Get();
		auto pixelShader = TextureLightPixelShader::Get();
		vertexShader->bind();
		vertexShader->SetCamera({
			world,
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
		pixelShader->bindLightBuffer();
	}

};