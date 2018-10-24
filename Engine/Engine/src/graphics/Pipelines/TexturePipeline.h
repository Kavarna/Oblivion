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
		m_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

private:
	mutable DirectX::XMFLOAT4 m_lastColor;
	mutable DirectX::XMFLOAT4 m_color;

public:

	void setAdditionalColor(const DirectX::XMFLOAT4& color) const
	{
		TexturePixelShader::Get()->SetAdditionalColor(color);
		memcpy((void*)&m_lastColor, &m_color, sizeof(DirectX::XMFLOAT4));
		memcpy((void*)&m_color, &color, sizeof(DirectX::XMFLOAT4));
	}

	void lastAdditionalColor() const
	{
		memcpy((void*)&m_color, &m_lastColor, sizeof(DirectX::XMFLOAT4));
		TexturePixelShader::Get()->SetAdditionalColor(m_color);
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
			world,
			cam->GetView(),
			cam->GetProjection()
			});
		pixelShader->bind();
	}

};