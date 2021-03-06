#include "TextureLightPixelShader.h"

TextureLightPixelShader::TextureLightPixelShader() : 
	IPixelShader((LPWSTR)L"Shaders/TextureLightPixelShader.cso")
{
	static auto bufferManager = BufferManager::Get();
	m_lightBuffer = bufferManager->CreateBuffer(
		D3D11_USAGE::D3D11_USAGE_DEFAULT, sizeof(Sun), 0);
}

TextureLightPixelShader::TextureLightPixelShader(LPWSTR path)
	: IPixelShader(path)
{
	static auto bufferManager = BufferManager::Get();
	m_lightBuffer = bufferManager->CreateBuffer(
		D3D11_USAGE::D3D11_USAGE_DEFAULT, sizeof(Sun), 0);
}

void TextureLightPixelShader::SetLight(const Sun & s)
{
	static auto bufferManager = BufferManager::Get();
	bufferManager->MapBuffer(m_lightBuffer, (void*)&s);
	bindLightBuffer();
}

void TextureLightPixelShader::bindLightBuffer()
{
	static auto bufferManager = BufferManager::Get();
	bufferManager->bindPSBuffer(0, m_lightBuffer);
}

PixelShaderEnum TextureLightPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderTextureLight;
}
