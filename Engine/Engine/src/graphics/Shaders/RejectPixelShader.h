#pragma once


#include "../interfaces/Shader.h"


class RejectPixelShader : public IPixelShader, public Singletone<RejectPixelShader>
{
public:
	RejectPixelShader();

	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override;

};