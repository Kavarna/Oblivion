#pragma once

#include <DirectXMath.h>

struct Sun
{
	DirectX::XMFLOAT4 m_direction;
	DirectX::XMFLOAT4 m_diffuseColor;
	DirectX::XMFLOAT4 m_ambientColor;

	Sun() = default;
	
	void setDirection(float x, float y, float z)
	{
		m_direction.x = x;
		m_direction.y = y;
		m_direction.z = z;
	};

	void setDiffuseColor(float r, float g, float b)
	{
		m_diffuseColor.x = r;
		m_diffuseColor.y = g;
		m_diffuseColor.z = b;
	}

	void setAmbientColor(float r, float g, float b)
	{
		m_ambientColor.x = r;
		m_ambientColor.y = g;
		m_ambientColor.z = b;
	}
};
