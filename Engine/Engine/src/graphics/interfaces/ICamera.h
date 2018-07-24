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

	virtual DirectX::XMFLOAT3 GetPosition() const
	{
		return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	};
	virtual DirectX::XMFLOAT3 GetDirection() const
	{
		return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	virtual float GetNearZ() const
	{
		return 0.0f;
	}
	virtual float GetFarZ() const
	{
		return 0.0f;
	}
	inline void RenderDebug() const;


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