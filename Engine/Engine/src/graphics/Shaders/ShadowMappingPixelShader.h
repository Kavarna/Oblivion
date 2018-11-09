#pragma once

#include "../interfaces/Shader.h"

class ShadowMappingPixelShader : public IPixelShader, public Singletone<ShadowMappingPixelShader>
{
public:
	ShadowMappingPixelShader();


	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override;

};