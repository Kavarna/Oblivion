#include "TextureLightPixelShader.h"

TextureLightPixelShader::TextureLightPixelShader() : 
	IPixelShader(L"Shaders/TextureLightPixelShader.cso")
{
	Sun s;
	s.m_direction = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	s.m_diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	s.m_ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(),
		&m_lightBuffer, D3D11_USAGE::D3D11_USAGE_DEFAULT,
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(Sun), 0);
}

void TextureLightPixelShader::SetLight(const Sun & s)
{
	m_d3d11Context->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &s, 0, 0);
	bindLightBuffer();
}

void TextureLightPixelShader::bindLightBuffer()
{
	m_d3d11Context->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());
}

PixelShaderEnum TextureLightPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderTextureLight;
}
