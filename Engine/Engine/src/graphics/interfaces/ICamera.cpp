#include "ICamera.h"
#include "../Helpers/DebugDraw.h"


inline void ICamera::RenderDebug() const
{
	if (g_isDeveloper)
	{
		auto debugDrawer = DebugDraw::Get();
		debugDrawer->RenderBoundingFrustum(m_viewFrustum);
	}
}
