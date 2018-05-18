#pragma once

#include <DirectXMath.h>

namespace Math
{
	inline bool isPointInTriangle(DirectX::XMFLOAT3 const& V1,
		DirectX::XMFLOAT3 const& V2, DirectX::XMFLOAT3 const& V3,
		DirectX::XMFLOAT3 const& Point)
	{
		using namespace DirectX;
		XMVECTOR A = XMLoadFloat3(&V1);
		XMVECTOR B = XMLoadFloat3(&V2);
		XMVECTOR C = XMLoadFloat3(&V3);
		XMVECTOR P = XMLoadFloat3(&Point);

		XMVECTOR v0 = C - A;
		XMVECTOR v1 = B - A;
		XMVECTOR v2 = P - A;

		float dot00 = XMVectorGetX(XMVector3Dot(v0, v0));
		float dot01 = XMVectorGetX(XMVector3Dot(v0, v1));
		float dot02 = XMVectorGetX(XMVector3Dot(v0, v2));
		float dot11 = XMVectorGetX(XMVector3Dot(v1, v1));
		float dot12 = XMVectorGetX(XMVector3Dot(v1, v2));

		float invDenom = 1.0f / ( dot00 * dot11 - dot01 * dot01 );
		float u, v;
		u = ( dot11 * dot02 - dot01 * dot12 ) * invDenom;
		v = ( dot00 * dot12 - dot01 * dot02 ) * invDenom;

		return u >= 0 && v >= 0 && u + v < 1;
	}
}