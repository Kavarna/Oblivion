
#include "Materials.hlsli"

Texture2D ObjTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbColor : register(b13)
{
	float4 additionalColor;
}

struct PSIn
{
	float4 PosH			: SV_POSITION;
	float3 PosW			: POSITION;
	float3 NormalW		: NORMAL;
	float3 TangentW		: TANGENT;
	float3 BinormalW	: BINORMAL;
	float2 Tex			: TEXCOORD;
};

void main(PSIn input)
{
	float4 matColor = g_material.GetColor(ObjTexture, ObjWrapSampler, input.Tex);
	clip(matColor.a - 0.15f);
}