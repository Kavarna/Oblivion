
Texture2D ObjOccluders : register(t0);
SamplerState ObjSampler : register(s0);
RWTexture2D<float4> ObjShadowMap : register(u0);

#define PI 3.14f

[numthreads(512, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float minDist = 1.0f;
	float width, height;
	ObjShadowMap.GetDimensions(height, width);

	float u = DTid.x / height;

	for (float y = 0; y < height; ++y)
	{
		// Rectangular to polar
		float2 norm = float2(u, y / height) * 2.0f - 1.0f;
		float theta = PI * 1.5f + norm.x * PI;
		float r = (1.0f + norm.y) * 0.5f;

		// Coord to sample
		float2 coord = float2(-r * sin(theta), -r * cos(theta)) / 2.0f + 0.5f;

		float4 data = ObjOccluders.SampleLevel(ObjSampler, coord, 0);

		if (data.a > 0.1f)
		{
			float dist = y / height;
			if (dist < minDist)
				minDist = dist;
		}
	}
	ObjShadowMap[DTid.xy] = float4(minDist, minDist, minDist, minDist);
}