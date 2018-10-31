#pragma once

#include "../interfaces/Shader.h"

class InstancedVertexShader sealed : public IVertexShader, public Singletone<InstancedVertexShader>
{
public:
	InstancedVertexShader();

private:
	ConstantBufferHandle					m_cameraBuffer;
	ConstantBufferHandle					m_cameraAdditionalBuffer;

public:
	struct SCameraInfo
	{
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

public:
	void __vectorcall					SetCamera(const SCameraInfo &);
	void								SetCameraAdditionalInfo(const SAdditionalCameraInfo&);

	ConstantBufferHandle				GetCameraBuffer() const { return m_cameraBuffer; }
public:

	// Inherited via Singletone<SimpleVertexShader>
	void Create() override;

protected:
	// Inherited via IVertexShader
	virtual void createInputLayout() override;
	virtual VertexShaderEnum getVertexShaderType() const final override;
};