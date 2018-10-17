#pragma once


#include "Pipeline.h"

#include "../Shaders/SimpleVertexShader.h"
#include "../Shaders/InstancedVertexShader.h"
#include "../Shaders/TexturePixelShader.h"



class TexturePipeline : public IPipeline, public Singletone<TexturePipeline>
{
public:
	TexturePipeline()
	{
		m_usedStages = (uint32_t)Shader::ShaderType::ePixel |
			(uint32_t)Shader::ShaderType::eVertex;
	}

public:

	void setAdditionalColor(const DirectX::XMFLOAT4& color) const
	{
		TexturePixelShader::Get()->SetAdditionalColor(color);
	}

	// Inherited via IPipeline
	virtual void __vectorcall bind(ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = InstancedVertexShader::Get();
		static auto pixelShader = TexturePixelShader::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
		pixelShader->bindColorBuffer();
	}

	virtual void __vectorcall bind(DirectX::FXMMATRIX & world, ICamera * cam) const override final
	{
		clearPipeline();
		static auto vertexShader = SimpleVertexShader::Get();
		static auto pixelShader = TexturePixelShader::Get();

		vertexShader->bind();
		vertexShader->SetCamera({
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
	}

};