
#include "Materials.hlsli"

Texture2D ObjTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbLight : register(b2)
{
	Material g_material;
}

cbuffer cbColor : register(b7)
{
	float4 additionalColor;
}

struct PSIn
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
	float4 matColor = g_material.GetColor(ObjTexture, ObjWrapSampler, input.TexCoord);
	clip(matColor.a - 0.15f);
	return matColor * additionalColor;
}