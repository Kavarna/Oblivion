#include "ShadowMappingPixelShader.h"

ShadowMappingPixelShader::ShadowMappingPixelShader() :
	IPixelShader(L"Shaders/ShadowMappingPixelShader.cso")
{
	m_lightBuffer = BufferManager::Get()->CreateBuffer(
		D3D11_USAGE::D3D11_USAGE_DEFAULT, sizeof(SLightBuffer), 0);
}

PixelShaderEnum ShadowMappingPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderShadowMapping;
}

void ShadowMappingPixelShader::setLight(const DirectX::XMFLOAT4 & position, const DirectX::XMFLOAT4 & diffuse, const DirectX::XMFLOAT4 & ambient)
{
	static auto bm = BufferManager::Get();
	SLightBuffer buffer;
	buffer.position = position;
	buffer.diffuse = diffuse;
	buffer.ambient = ambient;
	bm->MapBuffer(m_lightBuffer, (void*)&buffer);
	bindLight();
}

void ShadowMappingPixelShader::bindLight() const
{
	static auto bm = BufferManager::Get();
	bm->bindPSBuffer(0, m_lightBuffer);
}
