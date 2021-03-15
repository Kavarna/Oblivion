#include "RejectPixelShader.h"

RejectPixelShader::RejectPixelShader() :
	IPixelShader((LPWSTR)L"Shaders/RejectPixelShader.cso")
{
}

PixelShaderEnum RejectPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderReject;
}
