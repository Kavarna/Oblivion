
#include "Materials.hlsli"

Texture2D ObjTexture : register(t0);
Texture2D ObjBumpMap : register(t1);

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
	float4 PosH			: SV_POSITION;
	float3 PosW			: POSITION;
	float3 NormalW		: NORMAL;
	float3 TangentW		: TANGENT;
	float3 BinormalW	: BINORMAL;
	float2 Tex			: TEXCOORD;
};


float4 main(PSIn input) : SV_TARGET
{
	input.NormalW = normalize(input.NormalW);
	input.TangentW = normalize(input.TangentW);
	input.BinormalW = normalize(input.BinormalW);

	float4 matColor = g_material.GetColor(ObjTexture, ObjWrapSampler, input.Tex);

	float3 normal = g_material.GetNormal(ObjBumpMap, ObjWrapSampler, input.Tex,
		input.NormalW, input.TangentW, input.BinormalW);


	clip(matColor.a - 0.15f);

	float4 color = g_sunLight.Ambient;

	color += g_sunLight.GetAmountOfLight(normal);

	float4 finalColor = color * matColor;
	//return float4(input.NormalW, 1.0f) * 0.5f + 0.5f;
	return finalColor;
	
}