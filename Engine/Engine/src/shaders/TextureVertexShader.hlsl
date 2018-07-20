

cbuffer cbPerObject : register(b0)
{
	float4x4 View;
	float4x4 Projection;
};


struct VSOut
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VSOut main(float3 pos : POSITION, float2 tex : TEXCOORD, row_major float4x4 world : WORLDMATRIX)
{
	VSOut output = (VSOut)0;

	float4x4 WVP = mul(world, View);
	WVP = mul(WVP, Projection);
	output.PositionH = mul(float4(pos, 1.0f), WVP);
	output.TexCoord = tex;

	return output;
}