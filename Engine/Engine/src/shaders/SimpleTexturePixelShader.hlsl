
Texture2D ObjTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbColor : register(b13)
{
	float4 additionalColor;
}

struct PSIn
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
	float4 matColor = ObjTexture.Sample(ObjWrapSampler, input.TexCoord);
	clip(matColor.a - 0.15f);
	return matColor * additionalColor;
}