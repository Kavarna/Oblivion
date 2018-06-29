

cbuffer cbPerObject : register(b0)
{
	float4x4 View;
	float4x4 Projection;
};


struct VSOut
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalW : NORMAL;
};

VSOut main( float3 pos : POSITION, float2 tex : TEXCOORD, float3 nor : NORMAL, row_major float4x4 world : WORLDMATRIX )
{
	VSOut output = (VSOut)0;

	float4x4 WVP = mul(world, View);
	WVP = mul(WVP, Projection);
	output.PositionH = mul(float4(pos, 1.0f), WVP);

	output.PositionW = mul(pos, (float3x3)world);
	output.TexCoord = tex;
	output.NormalW = mul(nor, (float3x3)world);

	return output;
}