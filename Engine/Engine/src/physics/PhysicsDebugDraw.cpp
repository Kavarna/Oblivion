#include "PhysicsDebugDraw.h"


btDebugDraw::btDebugDraw()
{
	m_renderer = std::make_unique<BatchRenderer>();
	m_renderer->Create();
}

btDebugDraw::~btDebugDraw()
{
}

void btDebugDraw::Render()
{
	if (m_debugActivated)
		BulletWorld::Get()->m_world->debugDrawWorld();
}

void btDebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	drawLine(from, to, color, color);
}

void btDebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor)
{
	DirectX::XMFLOAT3 XMfromPoint = DirectX::XMFLOAT3(from.x(), from.y(), from.z());
	DirectX::XMFLOAT4 XMfromColor = DirectX::XMFLOAT4(fromColor.x(), fromColor.y(),
		fromColor.z(), from.w());
	m_renderer->Vertex(XMfromPoint, XMfromColor);
	DirectX::XMFLOAT3 XMtoPoint = DirectX::XMFLOAT3(to.x(), to.y(), to.z());
	DirectX::XMFLOAT4 XMtoColor = DirectX::XMFLOAT4(toColor.x(), toColor.y(),
		toColor.z(), toColor.w());
	m_renderer->Vertex(XMtoPoint, XMtoColor);
}

void btDebugDraw::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
{
	this->drawSphere(PointOnB, distance, color);
}

void btDebugDraw::reportErrorWarning(const char * warningString)
{
	DX::OutputVDebugString(L"%s", warningString);
}

void btDebugDraw::draw3dText(const btVector3 & location, const char * textString)
{
	DX::OutputVDebugString(L"3D Text\n");
}

void btDebugDraw::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

int btDebugDraw::getDebugMode() const
{
	return m_debugMode;
}

void btDebugDraw::clearLines()
{
	m_renderer->Begin();
}

void btDebugDraw::flushLines()
{
	m_renderer->End(reinterpret_cast<ICamera*>(g_camera.get()));
}

void btDebugDraw::EnableDebug()
{
	BulletWorld::Get()->m_world->setDebugDrawer(this);
	m_debugActivated = true;
}

void btDebugDraw::DisableDebug()
{
	BulletWorld::Get()->m_world->setDebugDrawer(nullptr);
	m_debugActivated = false;
}

void btDebugDraw::ToggleDebug()
{
	if (m_debugActivated)
		DisableDebug();
	else
		EnableDebug();
}
