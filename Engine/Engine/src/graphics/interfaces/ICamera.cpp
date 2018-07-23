#include "ICamera.h"
#include "../Helpers/DebugDraw.h"


#if DEBUG || _DEBUG
inline void ICamera::RenderDebug() const
{
	auto debugDrawer = DebugDraw::Get();
	debugDrawer->RenderBoundingFrustum(m_viewFrustum);

}
#endif
