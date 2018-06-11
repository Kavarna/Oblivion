
#include "Lights.hlsli"


Texture2D ObjTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbLight
{
	Sun g_sunLight;
};

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
	float4 matColor = ObjTexture.Sample(ObjWrapSampler, input.TexCoord);

	float4 color = g_sunLight.Ambient;

	color += g_sunLight.GetAmountOfLight(input.NormalW);

	float4 finalColor = color * matColor;
	return finalColor;
}