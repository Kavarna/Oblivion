struct Material
{
	float4 color; // 16
	float specular; // 4
	int hasTexture; // 4
	int hasBump; // 4
	int hasSpecular; // 4
	float minTessFactor; // 4
	float maxTessFactor; // 4
	float2 pad; // 8

	float4 GetColor(Texture2D tex, SamplerState sam, float2 texCoords)
	{
		if (hasTexture)
			return tex.Sample(sam, texCoords);
		else
			return color;
	}
};