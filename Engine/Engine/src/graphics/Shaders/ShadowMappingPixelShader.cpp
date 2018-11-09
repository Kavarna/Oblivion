#include "ShadowMappingPixelShader.h"

ShadowMappingPixelShader::ShadowMappingPixelShader() :
	IPixelShader(L"Shaders/ShadowMappingPixelShader.cso")
{
}

PixelShaderEnum ShadowMappingPixelShader::getPixelShaderType() const
{
	return PixelShaderEnum::PixelShaderShadowMapping;
}
