#include "ColorPixelShader.h"

ColorPixelShader::ColorPixelShader() : 
	IPixelShader(L"Shaders/ColorPixelShader.cso")
{
}

PixelShaderEnum ColorPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderColor;
}
