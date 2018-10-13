#pragma once

#include "../interfaces/Shader.h"

class BasicPixelShader : public IPixelShader, public Singletone<BasicPixelShader>
{
public:
	BasicPixelShader();


protected:
	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override;
};