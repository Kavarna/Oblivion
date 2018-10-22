#pragma once


#include "../interfaces/Shader.h"


class TexturePixelShader : public IPixelShader, public Singletone<TexturePixelShader>
{
public:
	TexturePixelShader();

	void SetAdditionalColor(const DirectX::XMFLOAT4&);
	void bindColorBuffer();

	// Inherited via IPixelShader
	virtual PixelShaderEnum getPixelShaderType() const override;

private:
	ConstantBufferHandle					m_additionalColorBuffer;

};