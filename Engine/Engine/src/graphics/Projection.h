#pragma once

#include "../input/Mouse.h"
#include "../input/Keyboard.h"
#include "interfaces/ICamera.h"
#include "interfaces/I2DTransforms.h"


class Projection :
	public ICamera, public AlignedObject, public I2DTransforms
{
public:
	Projection();
	~Projection() = default;

public:
	float m_width;
	float m_height;
	float m_nearZ;
	float m_farZ;

public:
	void Construct() override
	{
		m_projectionMatrix = DirectX::XMMatrixOrthographicLH(m_width, m_height, m_nearZ, m_farZ);
	}
	void ConstructTransform()
	{
		m_viewMatrix = DirectX::XMMatrixLookToLH(m_position, m_direction, m_up);
	}

	void TranslateTo(float x, float y);
	void Translate(float x, float y);
	void SetOrtho(bool yDown);

	DirectX::XMMATRIX& GetView() override { return m_viewMatrix; };
	DirectX::XMMATRIX& GetProjection() override { return m_projectionMatrix; };

	void GetPosition(DirectX::XMFLOAT3& pos) const
	{
		DirectX::XMStoreFloat3(&pos, m_position);
	}
	void GetDirection(DirectX::XMFLOAT3& dir) const
	{
		DirectX::XMStoreFloat3(&dir, m_direction);
	}
	void GetUpDirection(DirectX::XMFLOAT3& dir) const
	{
		DirectX::XMStoreFloat3(&dir, m_up);
	}
	
private:
	DirectX::XMMATRIX	m_projectionMatrix;
	DirectX::XMMATRIX	m_viewMatrix;

	DirectX::XMVECTOR	m_position;
	DirectX::XMVECTOR	m_direction;
	DirectX::XMVECTOR	m_up;
};

