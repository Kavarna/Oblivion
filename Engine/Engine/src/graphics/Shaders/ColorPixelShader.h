#pragma once

#include "../interfaces/Shader.h"


class ColorPixelShader : public IPixelShader, public Singletone<ColorPixelShader>
{
public:
	ColorPixelShader();


protected:
	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override;
};