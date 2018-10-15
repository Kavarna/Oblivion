#include "TexturePixelShader.h"

TexturePixelShader::TexturePixelShader() :
	IPixelShader(L"Shaders/TexturePixelShader.cso")
{
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(),
		&m_additionalColorBuffer, D3D11_USAGE::D3D11_USAGE_DEFAULT,
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(DirectX::XMFLOAT4), 0, &color);
}

void TexturePixelShader::SetAdditionalColor(const DirectX::XMFLOAT4 & col)
{
	m_d3d11Context->UpdateSubresource(m_additionalColorBuffer.Get(), 0, nullptr,
		&col, 0, 0);
	bindColorBuffer();
}

void TexturePixelShader::bindColorBuffer()
{
	m_d3d11Context->PSSetConstantBuffers(13, 1, m_additionalColorBuffer.GetAddressOf());
}

PixelShaderEnum TexturePixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderTexture;
}
