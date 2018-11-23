#pragma once

#include "../interfaces/Shader.h"

class ShadowMappingPixelShader : public IPixelShader, public Singletone<ShadowMappingPixelShader>
{
public:
	ShadowMappingPixelShader();


	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override;

public:
	void setLight(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& diffuse,
		const DirectX::XMFLOAT4& ambient);
	void bindLight() const;

public:
	struct SLightBuffer
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 ambient;
	};

private:
	ConstantBufferHandle		m_lightBuffer;
};