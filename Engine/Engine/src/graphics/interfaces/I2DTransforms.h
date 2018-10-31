#pragma once

#include "../../common/common.h"

class I2DTransforms
{
public:
	I2DTransforms() = default;
	I2DTransforms(float width, float height) :
		m_windowWidth(width), m_windowHeight(height) {};
	virtual ~I2DTransforms() = default;
public:
	void SetWindowInfo(float windowWidth, float windowHeight)
	{
		m_windowWidth = windowWidth;
		m_windowHeight = windowHeight;
	}

protected:
	float m_windowWidth;
	float m_windowHeight;
};