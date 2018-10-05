#include "commonmath.h"

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


template <>
btVector3 __vectorcall Math::GetTranslationFromMatrix<btVector3>(DirectX::FXMMATRIX& matrix)
{

	btVector3 translation;
	translation.setX(XMVectorGetX(matrix.r[3]));
	translation.setY(XMVectorGetY(matrix.r[3]));
	translation.setZ(XMVectorGetZ(matrix.r[3]));

	return translation;
}

template<>
DirectX::XMFLOAT3 __vectorcall Math::GetScalingFromMatrix(DirectX::FXMMATRIX & matrix)
{
	DirectX::XMFLOAT3 scaling;
	scaling.x = XMVectorGetX(matrix.r[0]);
	scaling.y = XMVectorGetY(matrix.r[1]);
	scaling.z = XMVectorGetZ(matrix.r[2]);
	
	return scaling;
}

template <>
DirectX::XMFLOAT3 __vectorcall Math::GetTranslationFromMatrix<DirectX::XMFLOAT3>(DirectX::FXMMATRIX& matrix)
{
	DirectX::XMFLOAT3 translation;
	translation.x = XMVectorGetX(matrix.r[3]);
	translation.y = XMVectorGetY(matrix.r[3]);
	translation.z = XMVectorGetZ(matrix.r[3]);
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
