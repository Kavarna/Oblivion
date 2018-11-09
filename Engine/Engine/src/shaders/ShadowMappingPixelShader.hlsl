
#include "Materials.hlsli"
#include "Lights.hlsli"

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

cbuffer cbLight : register(b0)
{
	float4 g_lightPosition;
	float4 g_lightDiffuse;
};

Texture2D ObjTexture : register(t0);
Texture2D ObjBumpMap : register(t1);

Texture2D ObjShadowmap : register(t4);
SamplerState ObjWrapSampler : register(s0);
SamplerComparisonState ObjComparisonSampler : register(s1);

float CalcShadowMap(float3 projectedTexCoord)
{
	float lightDepth = projectedTexCoord.z;

	float dx = 1.0f / 2048.f;

	float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	float percentLit = 0.0f;
	for (int i = 0; i < 9; ++i)
	{
		percentLit += ObjShadowmap.SampleCmpLevelZero(ObjComparisonSampler,
			(float2)offsets[i] + (float2)projectedTexCoord.xy, (float1)lightDepth).r;
	}
	percentLit /= 9.0f;
	return percentLit;
}

float4 main(PSIn input) : SV_TARGET
{
	input.NormalW = normalize(input.NormalW);
	input.TangentW = normalize(input.TangentW);
	input.BinormalW = normalize(input.BinormalW);

	float4 matColor = g_material.GetColor(ObjTexture, ObjWrapSampler, input.Tex.xy);

	float3 normal = g_material.GetNormal(ObjBumpMap, ObjWrapSampler, input.Tex.xy,
		input.NormalW, input.TangentW, input.BinormalW);

	float3 projectedTexCoord;
	projectedTexCoord.x = input.LightPositionH.x / input.LightPositionH.w / 2.0f + 0.5f;
	projectedTexCoord.y = -input.LightPositionH.y / input.LightPositionH.w / 2.0f + 0.5f;
	projectedTexCoord.z = input.LightPositionH.z / input.LightPositionH.w - 0.001f;

	if (saturate(projectedTexCoord.x) == projectedTexCoord.x &&
		saturate(projectedTexCoord.y) == projectedTexCoord.y)
	{
		float percentLit = CalcShadowMap(projectedTexCoord) + 0.5f;
		return matColor * percentLit;
	}

	return matColor * 0.2f;
}