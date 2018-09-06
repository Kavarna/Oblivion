
struct VSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

cbuffer cbCamera : register(b0)
{
	float4x4 WVP;
};

VSOut main(float3 position : POSITION, float2 texCoord : TEXCOORD)
{
	VSOut output;
	output.position = mul(float4(position, 1.0f), WVP);
	output.texCoord = texCoord;
	return output;
}