#include "Projection.h"
#include "../scripting/LuaManager.h"

void Projection::LuaRegister()
{
	US_NS_LUA;

	getGlobalNamespace()
		.beginNamespace("Oblivion")
			.deriveClass<Projection,ICamera>("Projection")
			.endClass()
		.endNamespace();
}
