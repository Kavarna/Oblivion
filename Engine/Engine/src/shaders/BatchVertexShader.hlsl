
struct VSOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 VP;
};

VSOut main( float4 pos : POSITION, float4 col : COLOR )
{
	VSOut output;
	pos.w = 1.0f;
	output.position = mul(pos, VP);
	output.color = col;
	return output;
}