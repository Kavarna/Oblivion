#pragma once


#include "TextureLightPixelShader.h"


class DisplacementMappingPixelShader : public IPixelShader, public Singletone<DisplacementMappingPixelShader>
{
public:
	DisplacementMappingPixelShader() :
		IPixelShader((LPWSTR)L"Shaders/DisplacementMappingPixelShader.cso")
	{}

public:

	void SetLight(const Sun& s)
	{
		TextureLightPixelShader::Get()->SetLight(s);
	}
	void bindLightBuffer() const
	{
		TextureLightPixelShader::Get()->bindLightBuffer();
	}

	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override
	{
		return PixelShaderEnum::PixelShaderDisplacement;
	}

};