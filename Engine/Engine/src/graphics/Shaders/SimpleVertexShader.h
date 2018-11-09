#pragma once

#include "../interfaces/Shader.h"

class SimpleVertexShader sealed : public IVertexShader, public Singletone<SimpleVertexShader>
{
public:
	SimpleVertexShader();

private:
	ConstantBufferHandle	m_cameraBuffer;
	ConstantBufferHandle	m_perCameraBuffer;
	ConstantBufferHandle	m_additionalCameraBuffer;

public:
	struct SCameraInfo
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

	struct SAdditionalCameraInfo
	{
		DirectX::XMFLOAT3 g_eyePos;
		float g_minTessDistance;
		float g_maxTessDistance;
		DirectX::XMFLOAT3 pad;
	};

	struct SCameraInfo2
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

public:
	void __vectorcall					SetCamera(const SCameraInfo &);
	void								SetCameraAdditionalInfo(const SAdditionalCameraInfo&);
	void __vectorcall					SetAdditionalCamera(const SCameraInfo2&);

public:

	// Inherited via Singletone<SimpleVertexShader>
	void Create() override;

protected:
	// Inherited via IVertexShader
	virtual void createInputLayout() override;
	virtual VertexShaderEnum getVertexShaderType() const final override;
};