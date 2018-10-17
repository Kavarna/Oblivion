#include "TextureLightPipeline.h"
#include "../Shaders/InstancedVertexShader.h"
#include "../Shaders/SimpleVertexShader.h"
#include "../Shaders/TextureLightPixelShader.h"


void TextureLightPipeline::setSunLight(const Sun & s)
{
	TextureLightPixelShader::Get()->SetLight(s);
}

void __vectorcall TextureLightPipeline::bind(ICamera * cam) const
{
	auto vertexShader = InstancedVertexShader::Get();
	auto pixelShader = TextureLightPixelShader::Get();
	vertexShader->bind();
	vertexShader->SetCamera({
		cam->GetView(),
		cam->GetProjection()
		});
	pixelShader->bind();
	pixelShader->bindLightBuffer();

}

void __vectorcall TextureLightPipeline::bind(DirectX::FXMMATRIX & world, ICamera * cam) const
{
	auto vertexShader = SimpleVertexShader::Get();
	auto pixelShader = TextureLightPixelShader::Get();
	vertexShader->bind();
	vertexShader->SetCamera({
		world,
		cam->GetView(),
		cam->GetProjection()
		});
	pixelShader->bind();
	pixelShader->bindLightBuffer();
}
