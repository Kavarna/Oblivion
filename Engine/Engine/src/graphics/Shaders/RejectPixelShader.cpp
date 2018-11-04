#include "RejectPixelShader.h"

RejectPixelShader::RejectPixelShader() :
	IPixelShader(L"Shaders/RejectPixelShader.cso")
{
}

PixelShaderEnum RejectPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderReject;
}
