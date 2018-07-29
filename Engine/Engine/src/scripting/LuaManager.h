#pragma once

#include "../common/common.h"
#include "LuaBridge.h"


namespace LuaManager
{
	extern unique_delete_ptr<luabridge::lua_State> g_luaState;

	void InitializeLua();

	void LoadScript(std::string const& file);
	luabridge::LuaRef getGlobal(const char* name);



	template<class type>
	inline void AddFunction(const char * name, type func)
	{
		luabridge::getGlobalNamespace(g_luaState.get())
			.addFunction(name, func);
	}
};