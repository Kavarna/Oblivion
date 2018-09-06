
struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

#define PI 3.14f

Texture2D ObjShadowmap : register(t0);
SamplerState ObjSampler : register(s0);

cbuffer cbColor : register(b0)
{
	float4 g_lightColor;
};

float getSample(float2 coord, float r)
{
	return step(r, ObjShadowmap.Sample(ObjSampler, coord).r);
}

float4 main(PSIn input) : SV_TARGET
{
	float width,height;
	ObjShadowmap.GetDimensions(width, height);

	float2 norm = input.texCoord * 2.0f - 1.0f;

	float theta = atan2(norm.y, norm.x);
	float r = length(norm);
	float coord = (theta + PI) / (2.0f * PI);

	float2 tc = float2(coord, 0.0f);

	float center = getSample(tc, r);
	float blur = (1.0f / width) * lerp(0.f, 1.f, r);
	float sum = 0.0f;

	sum += getSample(float2(tc.x - 4.0f*blur, tc.y), r) * 0.05f;
	sum += getSample(float2(tc.x - 3.0f*blur, tc.y), r) * 0.09f;
	sum += getSample(float2(tc.x - 2.0f*blur, tc.y), r) * 0.12f;
	sum += getSample(float2(tc.x - 1.0f*blur, tc.y), r) * 0.15f;

	sum += center * 0.16f;

	sum += getSample(float2(tc.x + 1.0f*blur, tc.y), r) * 0.15f;
	sum += getSample(float2(tc.x + 2.0f*blur, tc.y), r) * 0.12f;
	sum += getSample(float2(tc.x + 3.0f*blur, tc.y), r) * 0.09f;
	sum += getSample(float2(tc.x + 4.0f*blur, tc.y), r) * 0.05f;


	//return float4(1.0f,1.0f,1.0f,1.0f);
	float lightIntensity = sum * lerp(1.0f, 0.0f, r);
	float4 color = g_lightColor * float4(lightIntensity, lightIntensity, lightIntensity, lightIntensity);
	clip(color.a - 0.01f);
	return color;

}