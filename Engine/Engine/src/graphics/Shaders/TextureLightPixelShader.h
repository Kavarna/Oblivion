#pragma once

#include "../interfaces/Shader.h"
#include "../Helpers/Lights.h"

class TextureLightPixelShader :
	public IPixelShader, public Singletone<TextureLightPixelShader>
{
public:
	TextureLightPixelShader();

public:

	void SetLight(const Sun&);
	void bindLightBuffer();

	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override final;

private:
	ConstantBufferHandle				m_lightBuffer;
};
