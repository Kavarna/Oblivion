#include "BasicPixelShader.h"

BasicPixelShader::BasicPixelShader() :
	IPixelShader(L"Shaders/BasicPixelShader.cso")
{
}

PixelShaderEnum BasicPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderBasic;
}
