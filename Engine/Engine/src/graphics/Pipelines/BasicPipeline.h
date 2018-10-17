#pragma once


#include "Pipeline.h"

#include "../Shaders/BasicPixelShader.h"
#include "../Shaders/SimpleVertexShader.h"
#include "../Shaders/InstancedVertexShader.h"

class BasicPipeline : public IPipeline, public Singletone<BasicPipeline>
{
public:
	BasicPipeline()
	{
		m_usedStages = (uint32_t)Shader::ShaderType::ePixel |
			(uint32_t)Shader::ShaderType::eVertex;
	}
public:
	virtual inline void __vectorcall bind(ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = InstancedVertexShader::Get();
		static auto pixelShader = BasicPixelShader::Get();
		vertexShader->SetCamera({
			cam->GetView(),
			cam->GetProjection()
			});
		vertexShader->bind();
		pixelShader->bind();
	}
	virtual inline void __vectorcall bind(DirectX::FXMMATRIX& world, ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = SimpleVertexShader::Get();
		static auto pixelShader = BasicPixelShader::Get();
		vertexShader->SetCamera({
			world,
			cam->GetView(),
			cam->GetProjection()
			});
		vertexShader->bind();
		pixelShader->bind();
		
	}
};