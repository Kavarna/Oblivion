#pragma once


#include "Pipeline.h"

#include "../Shaders/SimpleVertexShader.h"
#include "../Shaders/InstancedVertexShader.h"
#include "../Shaders/ColorPixelShader.h"



class ColorPipeline : public IPipeline, public Singletone<ColorPipeline>
{
public:
	ColorPipeline()
	{
		m_usedStages = (uint32_t)Shader::ShaderType::ePixel |
			(uint32_t)Shader::ShaderType::eVertex;
	}

public:

	// Inherited via IPipeline
	virtual void __vectorcall bind(ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = InstancedVertexShader::Get();
		static auto pixelShader = ColorPixelShader::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
	}

	virtual void __vectorcall bind(DirectX::FXMMATRIX & world, ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = SimpleVertexShader::Get();
		static auto pixelShader = ColorPixelShader::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
			world,
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
	}

};