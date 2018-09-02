
struct VSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 VP;
};

VSOut main(float4 pos : POSITION, float2 tex : TEXCOORD)
{
	VSOut output;
	pos.w = 1.0f;
	output.position = mul(pos, VP);
	output.texCoord = tex;
	return output;
}