
#include "Materials.hlsli"

Texture2D ObjTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbLight : register(b2)
{
	Material g_material;
}

struct PSIn
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
	float4 matColor = g_material.GetColor(ObjTexture, ObjWrapSampler, input.TexCoord);
	return matColor;
}