
#include "Materials.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 View;
	float4x4 Projection;
	float4x4 World;
}

cbuffer cbPerCamera : register(b1)
{
	float3 g_eyePos;
	float g_minTessDistance;
	float g_maxTessDistance;
}

cbuffer cbMaterial : register(b2)
{
	Material g_material;
}

struct VSOut
{
	float4 PosH			: SV_POSITION;
	float3 PosW			: POSITION;
	float3 NormalW		: NORMAL;
	float3 TangentW		: TANGENT;
	float3 BinormalW	: BINORMAL;
	float2 Tex			: TEXCOORD;
	float  TessFactor	: TESS;
};

VSOut main(float3 pos : POSITION, float2 tex : TEXCOORD,
	float3 nor : NORMAL, float3 tan : TANGENT, float3 bin : BINORMAL)
{
	VSOut output;

	float4x4 WVP = mul(World, View);
	WVP = mul(WVP, Projection);

	output.PosH = mul(float4(pos, 1.0f), WVP);


	output.Tex = tex;

	output.PosW = mul(float4(pos, 1.0f), World).xyz;
	output.NormalW = mul(nor, (float3x3)World);
	output.TangentW = mul(tan, (float3x3)World);
	output.BinormalW = mul(bin, (float3x3)World);

	float d = distance(output.PosW, g_eyePos);

	float tess = saturate((g_minTessDistance - d) / (g_minTessDistance - g_maxTessDistance));

	output.TessFactor = g_material.minTessFactor + tess * (g_material.maxTessFactor * g_material.minTessFactor);

	return output;

}