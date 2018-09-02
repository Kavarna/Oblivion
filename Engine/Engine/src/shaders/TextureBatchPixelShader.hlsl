
Texture2D ObjTexture : register(t0);
SamplerState ObjSampler : register(s0);

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
	float4 color = ObjTexture.Sample(ObjSampler, input.texCoord);
	clip(color.a - 0.15f);
	return color;
}