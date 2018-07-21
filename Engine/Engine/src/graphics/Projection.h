#pragma once

#include "../input/Mouse.h"
#include "../input/Keyboard.h"
#include "interfaces/Camera.h"


class Projection :
	public ICamera
{
public:
	Projection() = default;
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
		m_viewMatrix = DirectX::XMMatrixIdentity();
	}

	DirectX::XMMATRIX& GetView() override { return m_viewMatrix; };
	DirectX::XMMATRIX& GetProjection() override { return m_projectionMatrix; };

private:
	DirectX::XMMATRIX	m_projectionMatrix;
	DirectX::XMMATRIX	m_viewMatrix;
};

