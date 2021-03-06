#pragma once

#include "../input/Mouse.h"
#include "../input/Keyboard.h"
#include "interfaces/ICamera.h"
#include "interfaces/I2DTransforms.h"


class ProjectionNDC :
	public ICamera, public IAlignedObject, public I2DTransforms
{
public:
	ProjectionNDC() {};
	~ProjectionNDC() = default;


public:
	void Construct() override
	{
	}

	DirectX::XMMATRIX& GetView() override { return m_viewMatrix; };
	DirectX::XMMATRIX& GetProjection() override { return m_projectionMatrix; };


private:
	DirectX::XMMATRIX	m_projectionMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX	m_viewMatrix = DirectX::XMMatrixIdentity();
};

