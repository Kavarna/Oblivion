#pragma once

#include "../interfaces/Shader.h"

class DisplacementMappingHullShader : public IHullShader, public Singletone<DisplacementMappingHullShader>
{
public:
	DisplacementMappingHullShader() :
		IHullShader((LPWSTR)L"Shaders/DisplacementMappingHullShader.cso")
	{}

public:

	// Inherited via IHullShader
	virtual HullShaderEnum getHullShaderType() const override final
	{
		return HullShaderEnum::HullShaderDisplacement;
	}

};