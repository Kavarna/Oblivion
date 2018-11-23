
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
	float4 g_lightAmbient;
};

Texture2D ObjTexture : register(t0);
Texture2D ObjBumpMap : register(t1);

Texture2D ObjShadowmap : register(t4);
SamplerState ObjWrapSampler : register(s0);
SamplerComparisonState ObjComparisonSampler : register(s1);

float CalcShadowMap(float3 projectedTexCoord)
{
	float lightDepth = projectedTexCoord.z;
	uint width, height;
	ObjShadowmap.GetDimensions(width, height);

	float dx = 1.0f / (float)width;
	float dy = 1.0f / (float)height;

	float2 offsets[9] =
	{
		float2(-dx,  -dy), float2(0.0f,  -dy), float2(dx,  -dy),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dy), float2(0.0f,  +dy), float2(dx,  +dy)
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

	float4 color = g_lightAmbient;

	if (saturate(projectedTexCoord.x) == projectedTexCoord.x &&
		saturate(projectedTexCoord.y) == projectedTexCoord.y)
	{
		float3 toLight = g_lightPosition.xyz - input.PosW;
		toLight = normalize(toLight);
		float howMuchlight = dot(normal, toLight);
		float percentLit = CalcShadowMap(projectedTexCoord);
		float4 litColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		if (howMuchlight > 0.0)
			litColor += howMuchlight * g_lightDiffuse;
		color += litColor * percentLit;
	}

	color = saturate(color);
	return matColor * color;
}