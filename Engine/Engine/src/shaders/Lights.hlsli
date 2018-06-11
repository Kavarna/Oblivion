struct Sun
{
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;

	float GetAmountOfLight(in float3 normal)
	{
		float howMuchLight = dot(normal, -Direction);
		[flatten]
		if (howMuchLight > 0.0f)
		{
			return howMuchLight * Diffuse;
		}
		return 0.0f;
	}
};