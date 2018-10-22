
#include "Materials.hlsli"

// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
	float4 PosH			: SV_POSITION;
	float3 PosW			: POSITION;
	float3 NormalW		: NORMAL;
	float3 TangentW		: TANGENT;
	float3 BinormalW	: BINORMAL;
	float2 Tex			: TEXCOORD;
	float  TessFactor	: TESS;
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
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	//Output.EdgeTessFactor[0] = 1;
	//Output.EdgeTessFactor[1] = 1;
	//Output.EdgeTessFactor[2] = 1;
	//Output.InsideTessFactor = 1;
	//return Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = 0.5f * (ip[1].TessFactor + ip[2].TessFactor);
	Output.EdgeTessFactor[1] = 0.5f * (ip[2].TessFactor + ip[0].TessFactor);
	Output.EdgeTessFactor[2] = 0.5f * (ip[0].TessFactor + ip[1].TessFactor);
	Output.InsideTessFactor = ip[0].TessFactor;

	if (g_material.minTessFactor == g_material.maxTessFactor && g_material.minTessFactor == 1.0f)
	{
		Output.EdgeTessFactor[0] = 1;
		Output.EdgeTessFactor[1] = 1;
		Output.EdgeTessFactor[2] = 1;
		Output.InsideTessFactor = 1;
	}

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	Output.PosH = ip[i].PosH;
	Output.PosW = ip[i].PosW;
	Output.NormalW = ip[i].NormalW;
	Output.TangentW = ip[i].TangentW;
	Output.BinormalW = ip[i].BinormalW;
	Output.Tex = ip[i].Tex;

	return Output;
}
