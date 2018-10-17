#pragma once

#include "../interfaces/Shader.h"

class EmptyVertexShader : public IVertexShader, public Singletone<EmptyVertexShader>
{
public:
	EmptyVertexShader() = default;

protected:

	// Inherited via IVertexShader
	virtual void createInputLayout() override {}
	virtual VertexShaderEnum getVertexShaderType() const final override
	{
		return VertexShaderEnum::VertexShaderNone;
	}
};

class EmptyPixelShader : public IPixelShader, public Singletone<EmptyPixelShader>
{
public:
	EmptyPixelShader() = default;

public:
	virtual PixelShaderEnum getPixelShaderType() const override
	{
		return PixelShaderEnum::PixelShaderNone;
	}
};

class EmptyHullShader : public IHullShader, public Singletone<EmptyHullShader>
{
public:
	EmptyHullShader() = default;

public:
	

	// Inherited via IHullShader
	virtual HullShaderEnum getHullShaderType() const override
	{
		return HullShaderEnum::HullShaderNone;
	}

};

class EmptyDomainShader : public IDomainShader, public Singletone<EmptyDomainShader>
{
public:
	EmptyDomainShader() = default;

public:

	
	// Inherited via IDomainShader
	virtual DomainShaderEnum getDomainShaderType() const override
	{
		return DomainShaderEnum::DomainShaderNone;
	}

};

class EmptyGeometryShader : public IGeometryShader, public Singletone<EmptyGeometryShader>
{
public:
	EmptyGeometryShader() = default;

public:



	// Inherited via IGeometryShader
	virtual GeometryShaderEnum getGeometryShaderType() const override
	{
		return GeometryShaderEnum::GeometryShaderNone;
	}

};