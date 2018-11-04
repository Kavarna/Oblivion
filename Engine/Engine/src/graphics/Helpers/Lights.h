#pragma once

#include <DirectXMath.h>
#include "../interfaces/ICamera.h"
#include "../../common/interfaces/AlignedObject.h"

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

namespace ProjectionTypes
{
	class Perspective {};
	class Ortographic {};
}

struct DirectionalLightView : public ICamera /*GetView(), GetProjection(), RenderFrustum()*/,
	public IAlignedObject
{
public:
	DirectX::XMFLOAT4 m_position;
	DirectX::XMFLOAT4 m_direction;
	DirectX::XMFLOAT4 m_up = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 m_diffuseColor;

	float m_nearZ, m_farZ;
	float m_width, m_height;
	float m_fov;

	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_projection;

public:
	DirectionalLightView() = default;

	void setPosition(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
		m_position.w = 1.0f;
	}

	void setDirection(float x, float y, float z)
	{
		m_direction.x = x;
		m_direction.y = y;
		m_direction.z = z;
		m_direction.w = 0.f;
		DirectX::XMStoreFloat4(&m_up, // Temporary solution; TODO: Change this
			DirectX::XMVector3Cross(
				DirectX::XMLoadFloat4(&m_direction),
				DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)
			));
	}

	void setDiffuseColor(float r, float g, float b)
	{
		m_diffuseColor.x = r;
		m_diffuseColor.y = g;
		m_diffuseColor.z = b;
		m_diffuseColor.w = 1.0f;
	}
	
	virtual DirectX::XMFLOAT3 GetPosition() const override
	{
		return { m_position.x, m_position.y, m_position.z };
	};
	virtual DirectX::XMFLOAT3 GetDirection() const override
	{
		return { m_direction.x, m_direction.y, m_direction.z };
	}
	virtual float GetNearZ() const override
	{
		return m_nearZ;
	}
	virtual float GetFarZ() const override
	{
		return m_farZ;
	}

	void setDimensions(float width, float height)
	{
		m_width = width;
		m_height = height;
	}

	void setNearZ(float z)
	{
		m_nearZ = z;
	}
	void setFarZ(float z)
	{
		m_farZ = z;
	}
	void setFov(float FOV)
	{
		m_fov = FOV;
	}

	template <class Q>
	typename std::enable_if<std::is_same<Q,
		ProjectionTypes::Perspective>::value>::type build()
	{
		m_view = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat4(&m_position),
			DirectX::XMLoadFloat4(&m_direction), DirectX::XMLoadFloat4(&m_up));
		m_projection = DirectX::XMMatrixPerspectiveFovLH(m_fov,
			m_width / m_height, GetNearZ(), GetFarZ());
		BuildViewFrustum();
	}
	template <class Q>
	typename std::enable_if<std::is_same<Q,
		ProjectionTypes::Ortographic>::value>::type build()
	{
		m_view = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&m_position),
			DirectX::XMLoadFloat4(&m_direction), DirectX::XMLoadFloat4(&m_up));
		m_projection = DirectX::XMMatrixOrthographicLH(m_width, m_height,
			GetNearZ(), GetFarZ());
		BuildViewFrustum();
	}

	// Inherited via ICamera
	virtual DirectX::XMMATRIX & GetView() override
	{
		return m_view;
	}

	virtual DirectX::XMMATRIX & GetProjection() override
	{
		return m_projection;
	}

};