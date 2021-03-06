#include "ColorPixelShader.h"

ColorPixelShader::ColorPixelShader() : 
	IPixelShader((LPWSTR)L"Shaders/ColorPixelShader.cso")
{
}

PixelShaderEnum ColorPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderColor;
}
