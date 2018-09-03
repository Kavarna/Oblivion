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
