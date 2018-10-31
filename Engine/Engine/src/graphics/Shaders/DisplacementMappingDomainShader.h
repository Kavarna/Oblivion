#pragma once

#include "../interfaces/Shader.h"

class DisplacementMappingDomainShader : public IDomainShader, public Singletone<DisplacementMappingDomainShader>
{
public:
	struct DSCameraBuffer
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

public:
	DisplacementMappingDomainShader() :
		IDomainShader(L"Shaders/DisplacementMappingDomainShader.cso")
	{
		m_cameraBuffer = BufferManager::Get()->CreateBuffer(
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, sizeof(DSCameraBuffer),
			D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
		);
	};

	void setCamera(const DSCameraBuffer& ds)
	{
		static auto bm = BufferManager::Get();
		bm->MapBuffer(m_cameraBuffer);
		bm->bindDSBuffer(0, m_cameraBuffer);
	}

	// Inherited via IDomainShader
	virtual DomainShaderEnum getDomainShaderType() const override
	{
		return DomainShaderEnum::DomainShaderDisplacement;
	}

private:
	ConstantBufferHandle m_cameraBuffer;

};