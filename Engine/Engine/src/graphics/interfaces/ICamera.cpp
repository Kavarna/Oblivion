#include "ICamera.h"
#include "../Helpers/DebugDraw.h"
#include "../../scripting/LuaManager.h"

inline void ICamera::RenderDebug() const
{
	if (g_isDeveloper)
	{
		auto debugDrawer = DebugDraw::Get();
		debugDrawer->RenderBoundingFrustum(m_viewFrustum);
	}
}

void ICamera::LuaRegister()
{
	US_NS_LUA;

	getGlobalNamespace(g_luaState.get())
		.beginNamespace("Oblivion")
			.beginClass<ICamera>("ICamera")
				.addFunction("RenderDebug",&ICamera::RenderDebug)
				.addFunction("GetNearZ",&ICamera::GetNearZ)
				.addFunction("GetFarZ", &ICamera::GetFarZ)
				.addFunction("GetDirection", &ICamera::GetDirection)
				.addFunction("GetPosition", &ICamera::GetPosition)
				.addFunction("GetView", &ICamera::GetView)
				.addFunction("GetProjection", &ICamera::GetProjection)
			.endClass()
		.endNamespace();
}
