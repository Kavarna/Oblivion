#pragma once

#include "../interfaces/Shader.h"

class SimpleVertexShader sealed : public IVertexShader, public Singletone<SimpleVertexShader>
{
public:
	SimpleVertexShader();

public:

	// Inherited via Singletone<SimpleVertexShader>
	void Create() override;

protected:
	// Inherited via IVertexShader
	virtual void createInputLayout() override;
	virtual VertexShaderEnum getVertexShaderType() const final override;
};