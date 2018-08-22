#include "commonmath.h"
#include "../scripting/LuaManager.h"

using namespace DirectX;

btMatrix3x3 __vectorcall Math::Matrix3x3FromXMMatrix(DirectX::FXMMATRIX & matrix)
{
	btMatrix3x3 ret;

	ret[0][0] = XMVectorGetX(matrix.r[0]);
	ret[0][1] = XMVectorGetY(matrix.r[0]);
	ret[0][2] = XMVectorGetZ(matrix.r[0]);

	ret[1][0] = XMVectorGetX(matrix.r[1]);
	ret[1][1] = XMVectorGetY(matrix.r[1]);
	ret[1][2] = XMVectorGetZ(matrix.r[1]);

	ret[2][0] = XMVectorGetX(matrix.r[2]);
	ret[2][1] = XMVectorGetY(matrix.r[2]);
	ret[2][2] = XMVectorGetZ(matrix.r[2]);

	return ret;
}

btVector3 __vectorcall Math::GetTranslationFromMatrix(DirectX::FXMMATRIX& matrix)
{

	btVector3 translation;
	translation.setX(XMVectorGetX(matrix.r[3]));
	translation.setY(XMVectorGetY(matrix.r[3]));
	translation.setZ(XMVectorGetZ(matrix.r[3]));

	return translation;
}

XMMATRIX Math::XMMatrixFromMatrix3x3(btMatrix3x3 & matrix)
{
	XMMATRIX resultMatrix;
	XMVectorSetX(resultMatrix.r[0], matrix[0][0]);
	XMVectorSetY(resultMatrix.r[0], matrix[0][1]);
	XMVectorSetZ(resultMatrix.r[0], matrix[0][2]);

	XMVectorSetX(resultMatrix.r[1], matrix[1][0]);
	XMVectorSetY(resultMatrix.r[1], matrix[1][1]);
	XMVectorSetZ(resultMatrix.r[1], matrix[1][2]);

	XMVectorSetX(resultMatrix.r[2], matrix[2][0]);
	XMVectorSetY(resultMatrix.r[2], matrix[2][1]);
	XMVectorSetZ(resultMatrix.r[2], matrix[2][2]);

	return resultMatrix;
}

void Math::LuaRegister()
{
	US_NS_LUA;
	getGlobalNamespace(g_luaState.get())
		.beginNamespace("Oblivion")
			.beginClass<XMFLOAT2>("float2")
				.addData("x", &XMFLOAT2::x,true)
				.addData("y", &XMFLOAT2::y,true)
			.endClass()
			.beginClass<XMFLOAT3>("float3")
				.addData("x", &XMFLOAT3::x, true)
				.addData("y", &XMFLOAT3::y, true)
				.addData("z", &XMFLOAT3::z, true)
			.endClass()
			.beginClass<XMFLOAT4>("float4")
				.addData("x", &XMFLOAT4::x,true)
				.addData("y", &XMFLOAT4::y, true)
				.addData("z", &XMFLOAT4::z,true)
				.addData("w", &XMFLOAT4::w,true)
			.endClass()
		.endNamespace();
}
