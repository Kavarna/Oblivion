cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
};


struct VSOut
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VSOut main(float3 pos : POSITION, float2 tex : TEXCOORD)
{
	VSOut output = (VSOut)0;

	output.PositionH = mul(float4(pos, 1.0f), WVP);
	output.TexCoord = tex;

	return output;
}