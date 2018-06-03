#pragma once

#include <DirectXMath.h>


struct Sun
{
	DirectX::XMFLOAT4 m_direction;
	DirectX::XMFLOAT4 m_diffuseColor;
	DirectX::XMFLOAT4 m_ambientColor;
};