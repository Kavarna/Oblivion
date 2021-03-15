#include "BasicPixelShader.h"

BasicPixelShader::BasicPixelShader() :
	IPixelShader((LPWSTR)L"Shaders/BasicPixelShader.cso")
{
}

PixelShaderEnum BasicPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderBasic;
}
