#pragma once

//#define BT_NO_SIMD_OPERATOR_OVERLOADS
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace Math
{

	inline bool isPointInTriangle(btVector3 const& A,
		btVector3 const& B, btVector3 const& C, btVector3 const& P)
	{
		btVector3 v0, v1, v2;
		v0 = C - A;
		v1 = B - A;
		v2 = P - A;

		float dot00 = v0.dot(v0);
		float dot01 = v0.dot(v1);
		float dot02 = v0.dot(v2);
		float dot11 = v1.dot(v1);
		float dot12 = v1.dot(v2);

		float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		float u, v;
		u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return u >= 0 && v >= 0 && u + v < 1;
	}
}