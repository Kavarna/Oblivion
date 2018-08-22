#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "common.h"

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
	};
	inline bool isPointInFrustum(DirectX::XMFLOAT3 const& point,
		std::array<DirectX::XMFLOAT4, 6> const& frustum)
	{
		using namespace DirectX;
		for (UINT i = 0; i < frustum.size(); ++i)
		{
			XMVECTOR planeNormal = XMVectorSet(frustum[i].x, frustum[i].y, frustum[i].z, 0.0f);
			float planeConstant = frustum[i].w;
			XMVECTOR Point = XMLoadFloat3(&point);
			if (XMVectorGetX(XMVector3Dot(planeNormal, Point)) + planeConstant < 0.0f)
				return false;
		}
		return true;
	}
	inline bool isAABBInFrustumCompletely(DirectX::XMFLOAT3 const& minAABB,
		DirectX::XMFLOAT3 const& maxAABB, std::array<DirectX::XMFLOAT4, 6> const& frustum)
	{
		using namespace DirectX;
		static std::array<XMVECTOR, 8> Points;
		Points[0] = XMVectorSet(minAABB.x, minAABB.y, minAABB.z, 1.0f);
		Points[1] = XMVectorSet(minAABB.x, minAABB.y, maxAABB.z, 1.0f);
		Points[2] = XMVectorSet(minAABB.x, maxAABB.y, minAABB.z, 1.0f);
		Points[3] = XMVectorSet(maxAABB.x, minAABB.y, minAABB.z, 1.0f);
		Points[4] = XMVectorSet(maxAABB.x, maxAABB.y, minAABB.z, 1.0f);
		Points[5] = XMVectorSet(maxAABB.x, minAABB.y, maxAABB.z, 1.0f);
		Points[6] = XMVectorSet(minAABB.x, maxAABB.y, maxAABB.z, 1.0f);
		Points[7] = XMVectorSet(maxAABB.x, maxAABB.y, maxAABB.z, 1.0f);
		for (UINT i = 0; i < frustum.size(); ++i)
		{
			XMVECTOR planeNormal = XMVectorSet(frustum[i].x, frustum[i].y, frustum[i].z, 0.0f);
			float planeConstant = frustum[i].w;
			for (unsigned int i = 0; i < Points.size(); ++i)
				if (XMVectorGetX(XMVector3Dot(Points[i], planeNormal)) + planeConstant < 0)
					return false;
		}
		return true;
	}
	inline bool isAABBInFrustum(DirectX::XMFLOAT3 const& minAABB,
		DirectX::XMFLOAT3 const& maxAABB, std::array<DirectX::XMFLOAT4, 6> const& frustum)
	{
		if (isPointInFrustum(DirectX::XMFLOAT3(minAABB.x, minAABB.y, minAABB.z),frustum))
			return true;
		if (isPointInFrustum(DirectX::XMFLOAT3(minAABB.x, minAABB.y, maxAABB.z),frustum))
			return true;
		if (isPointInFrustum(DirectX::XMFLOAT3(minAABB.x, maxAABB.y, minAABB.z),frustum))
			return true;
		if (isPointInFrustum(DirectX::XMFLOAT3(maxAABB.x, minAABB.y, minAABB.z),frustum))
			return true;
		if (isPointInFrustum(DirectX::XMFLOAT3(maxAABB.x, maxAABB.y, minAABB.z),frustum))
			return true;
		if (isPointInFrustum(DirectX::XMFLOAT3(maxAABB.x, minAABB.y, maxAABB.z),frustum))
			return true;
		if (isPointInFrustum(DirectX::XMFLOAT3(minAABB.x, maxAABB.y, maxAABB.z),frustum))
			return true;
		if (isPointInFrustum(DirectX::XMFLOAT3(maxAABB.x, maxAABB.y, maxAABB.z),frustum))
			return true;
		return false;
	}
	btMatrix3x3			__vectorcall	Matrix3x3FromXMMatrix(DirectX::FXMMATRIX& matrix);
	btVector3			__vectorcall	GetTranslationFromMatrix(DirectX::FXMMATRIX& matrix);
	DirectX::XMMATRIX					XMMatrixFromMatrix3x3(btMatrix3x3& matrix);
	void LuaRegister();
}