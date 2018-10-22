
#include "Materials.hlsli"

cbuffer cbPerObject : register(b0)
{
	row_major float4x4 View;
	row_major float4x4 Projection;
}

cbuffer cbPerCamera : register(b1)
{
	float3 g_eyePos;
	float g_minTessDistance;
	float g_maxTessDistance;
}


struct VSOut
{
	float4 PosH			: SV_POSITION;
	float2 Tex			: TEXCOORD;
	float3 PosW			: POSITION;
	float3 NormalW		: NORMAL;
	float3 TangentW		: TANGENT;
	float3 BinormalW	: BINORMAL;
	float  TessFactor	: TESS;
};

VSOut main(float3 pos : POSITION, float2 tex : TEXCOORD,
	float3 nor : NORMAL, float3 tan : TANGENT, float3 bin : BINORMAL,
	row_major float4x4 world : WORLDMATRIX)
{
	VSOut output;

	float4x4 WVP = mul(world, View);
	WVP = mul(WVP, Projection);
	
	output.PosH = mul(float4(pos, 1.0f), WVP);


	output.Tex = tex;

	output.PosW			= mul(float4(pos,1.0f), world).xyz;
	output.NormalW		= mul(nor, (float3x3)world);
	output.TangentW		= mul(tan, (float3x3)world);
	output.BinormalW	= mul(bin, (float3x3)world);

	float d = distance(output.PosW, g_eyePos);
	
	float tess = saturate((g_minTessDistance - d) / (g_minTessDistance - g_maxTessDistance));

	output.TessFactor = g_material.minTessFactor + tess * (g_material.maxTessFactor * g_material.minTessFactor);

	return output;

}