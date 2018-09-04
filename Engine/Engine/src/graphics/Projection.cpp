#include "Projection.h"

Projection::Projection()
{
	m_viewMatrix = DirectX::XMMatrixIdentity();
}

void Projection::TranslateTo(float X, float Y)
{
	//float halfWidth = (float)m_width;
	//float halfHeight = (float)m_height;
	//X += halfWidth;
	//Y += halfHeight;
	float NewX = (float(m_windowWidth / 2) * -1 + X);
	float NewY = (float(m_windowHeight / 2) - Y);
	Translate(NewX, NewY);
}

void Projection::Translate(float x, float y)
{
	m_position = DirectX::XMVectorSet(x, y, 0.0f, 1.0f);
	//DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(x, y, 0.0f, 1.0f));
}

void Projection::SetOrtho(bool yDown)
{
	if (yDown)
	{
		m_up = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);
		m_direction = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else
	{
		m_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
		m_direction = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	}
	m_position = DirectX::XMVectorSet(m_width / 2.0f, m_height / 2.0f, 0.0f, 1.0f);
}


