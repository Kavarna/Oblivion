#ifndef _MATERIALS_HLSLI_
#define _MATERIALS_HLSLI_


#include "Lights.hlsli"

struct Material
{
	float4 color; // 16
	float specular; // 4
	int hasTexture; // 4
	int hasBump; // 4
	int hasSpecular; // 4
	float minTessFactor; // 4
	float maxTessFactor; // 4
	float tessScale; // 4
	float pad; // 4

	float4 GetColor(Texture2D tex, SamplerState sam, float2 texCoords)
	{
		if (hasTexture)
			return tex.Sample(sam, texCoords);
		else
			return color;
	}

	float3 GetNormal(Texture2D bumpMap, SamplerState sam, float2 texCoords,
		float3 UnitNormalW, float3 TangentW, float3 BinormalW)
	{
		if (hasBump)
		{
			float3 bumpMapSample = bumpMap.Sample(sam, texCoords).rgb;
			UnitNormalW = NormalSampleToWorldSpace(bumpMapSample, UnitNormalW, TangentW, BinormalW);
		}
		return UnitNormalW;
	}
};

#define MATERIAL_SLOT 2

cbuffer cbMaterial : register(b2)
{
	Material g_material;
}



#endif