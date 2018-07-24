struct Sun
{
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;

	float4 GetAmountOfLight(in float3 normal)
	{
		float howMuchLight = dot(normal, -Direction.xyz);
		[flatten]
		if (howMuchLight > 0.0f)
		{
			return howMuchLight * Diffuse;
		}
		return 0.0f;
	}
};

float3 NormalSampleToWorldSpace(float3 Sample,
	float3 UnitNormalW, float3 TangentW, float3 BinormalW)
{
	float3 normalT = 2.0f * Sample - 1.0f;

	float3 N = UnitNormalW;
	float3 T = normalize(T - dot(TangentW, N) * N);
	float3 B = BinormalW;

	float3x3 TBN = float3x3(T, B, N);

	float3 bumpedNormalW = mul(UnitNormalW, TBN);

	return bumpedNormalW;
}