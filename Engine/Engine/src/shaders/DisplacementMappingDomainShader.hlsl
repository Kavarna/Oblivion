
#include "Materials.hlsli"

Texture2D ObjBumpMap : register(t1);

SamplerState ObjWrapSampler : register(s0);

struct DS_OUTPUT
{
	float4 PosH			: SV_POSITION;
	float3 PosW			: POSITION;
	float3 NormalW		: NORMAL;
	float3 TangentW		: TANGENT;
	float3 BinormalW	: BINORMAL;
	float2 Tex			: TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 PosH			: SV_POSITION;
	float3 PosW			: POSITION;
	float3 NormalW		: NORMAL;
	float3 TangentW		: TANGENT;
	float3 BinormalW	: BINORMAL;
	float2 Tex			: TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

#define NUM_CONTROL_POINTS 3

cbuffer cbCamera : register(b0)
{
	float4x4 View;
	float4x4 Projection;
}

cbuffer cbMaterial : register(b2)
{
	Material g_material;
}

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.PosW = domain.x * patch[0].PosW + domain.y * patch[1].PosW + domain.z * patch[2].PosW;
	Output.NormalW = domain.x * patch[0].NormalW + domain.y * patch[1].NormalW + domain.z * patch[2].NormalW;
	Output.TangentW = domain.x * patch[0].TangentW + domain.y * patch[1].TangentW + domain.z * patch[2].TangentW;
	Output.BinormalW = domain.x * patch[0].BinormalW + domain.y * patch[1].BinormalW + domain.z * patch[2].BinormalW;

	Output.Tex = domain.x * patch[0].Tex + domain.y * patch[1].Tex + domain.z * patch[2].Tex;

	Output.NormalW = normalize(Output.NormalW);

	if (g_material.hasBump)
	{
		float h = ObjBumpMap.SampleLevel(ObjWrapSampler, Output.Tex, 0).a;

		Output.PosW += (g_material.tessScale * (h - 1.0f)) * Output.NormalW;
	}

	float4x4 VP;
	VP = mul(View, Projection);
	Output.PosH = mul(float4(Output.PosW, 1.0f), VP);

	return Output;
}
