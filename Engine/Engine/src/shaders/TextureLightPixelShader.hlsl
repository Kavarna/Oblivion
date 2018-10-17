
#include "Lights.hlsli"
#include "Materials.hlsli"


Texture2D ObjTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbLight : register(b0)
{
	Sun g_sunLight;
};

cbuffer cbMaterial : register(b2)
{
	Material g_material;
}

struct PSIn
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalW : NORMAL;
};

float4 main(PSIn input) : SV_TARGET
{
	input.NormalW = normalize(input.NormalW);
	float4 matColor = g_material.GetColor(ObjTexture, ObjWrapSampler, input.TexCoord);
	
	clip(matColor.a - 0.15f);

	float4 color = g_sunLight.Ambient;

	color += g_sunLight.GetAmountOfLight(input.NormalW);

	float4 finalColor = color * matColor;
	return finalColor;
}