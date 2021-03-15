#include "TexturePixelShader.h"

TexturePixelShader::TexturePixelShader() :
	IPixelShader((LPWSTR)L"Shaders/TexturePixelShader.cso")
{
	static auto bufferManager = BufferManager::Get();
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	m_additionalColorBuffer = bufferManager->CreateBuffer(D3D11_USAGE::D3D11_USAGE_DEFAULT,
		sizeof(DirectX::XMFLOAT4), 0, &color);
}

void TexturePixelShader::SetAdditionalColor(const DirectX::XMFLOAT4 & col)
{
	static auto bufferManager = BufferManager::Get();
	bufferManager->MapBuffer(m_additionalColorBuffer, (void*)&col);
	bindColorBuffer();
}

void TexturePixelShader::bindColorBuffer()
{
	static auto bufferManager = BufferManager::Get();
	bufferManager->bindPSBuffer(13, m_additionalColorBuffer);
}

PixelShaderEnum TexturePixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderTexture;
}
