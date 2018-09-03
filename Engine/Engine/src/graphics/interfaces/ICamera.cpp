#include "ICamera.h"
#include "../Helpers/GraphicsDebugDraw.h"

inline void ICamera::RenderDebug() const
{
	if (g_isDeveloper)
	{
		auto GraphicsDebugDrawer = GraphicsDebugDraw::Get();
		GraphicsDebugDrawer->RenderBoundingFrustum(m_viewFrustum);
	}
}