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