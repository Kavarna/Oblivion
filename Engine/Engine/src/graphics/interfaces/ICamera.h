#pragma once

#include "AlignedObject.h"


class ICamera : public AlignedObject
{
public:
	ICamera() = default;
	virtual ~ICamera() = default;

public:
	virtual void Construct() = 0;

	virtual DirectX::XMMATRIX& GetView() = 0;
	virtual DirectX::XMMATRIX& GetProjection() = 0;

	inline	DirectX::BoundingFrustum& GetFrustum()
	{
		return m_viewFrustum;
	}

#if DEBUG || _DEBUG
	inline void RenderDebug() const;
#endif

protected:
	void BuildViewFrustum()
	{
		using namespace DirectX;
		
		m_viewFrustum = BoundingFrustum(GetProjection());

		auto view = GetView();
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(view);
		auto invView = DirectX::XMMatrixInverse(&determinant, view);

		m_viewFrustum.Transform(m_viewFrustum, invView);
	}

protected:
	DirectX::BoundingFrustum m_viewFrustum;
};