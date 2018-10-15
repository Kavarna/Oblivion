#pragma once

#include "../interfaces/Shader.h"

class SimpleVertexShader sealed : public IVertexShader, public Singletone<SimpleVertexShader>
{
public:
	SimpleVertexShader();

private:
	MicrosoftPointer<ID3D11Buffer>		m_cameraBuffer;

public:
	struct SCameraInfo
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

public:
	void __vectorcall					SetCamera(const SCameraInfo &);

public:

	// Inherited via Singletone<SimpleVertexShader>
	void Create() override;

protected:
	// Inherited via IVertexShader
	virtual void createInputLayout() override;
	virtual VertexShaderEnum getVertexShaderType() const final override;
};