#pragma once

#include "../interfaces/Shader.h"
#include "../Helpers/Lights.h"
#include "OblivionObjects.h"

class TextureLightPixelShader :
	public IPixelShader, public Singletone<TextureLightPixelShader>
{
public:
	TextureLightPixelShader();

protected:
	TextureLightPixelShader(LPWSTR);

public:

	void SetLight(const Sun&);
	void bindLightBuffer();

	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override;

private:
	ConstantBufferHandle				m_lightBuffer;
};
