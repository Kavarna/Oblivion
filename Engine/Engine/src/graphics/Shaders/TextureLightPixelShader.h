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
	MicrosoftPointer<ID3D11Buffer>				m_lightBuffer;
};
