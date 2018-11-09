
#include "Materials.hlsli"

Texture2D ObjTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbColor : register(b13)
{
	float4 additionalColor;
}

struct PSIn
{
	float4 PosH				: SV_POSITION;
	float4 Tex				: TEXCOORD;
	float3 PosW				: POSITION;
	float3 NormalW			: NORMAL;
	float3 TangentW			: TANGENT;
	float3 BinormalW		: BINORMAL;
	float  TessFactor		: TESS;
	float4 LightPositionH	: POSITION1;
};

float4 main(PSIn input) : SV_TARGET
{
	float4 matColor = g_material.GetColor(ObjTexture, ObjWrapSampler, input.Tex.xy);
	clip(matColor.a - 0.15f);
	return matColor * additionalColor;
}