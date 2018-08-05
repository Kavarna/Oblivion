#pragma once

#include "../common/common.h"
#include "LuaBridge.h"


#define US_NS_LUA using namespace luabridge; using namespace LuaManager;


namespace LuaManager
{
	extern unique_delete_ptr<luabridge::lua_State> g_luaState;

	void InitializeLua();

	void LoadScriptFromFile(std::string const& file);
	void LoadScriptFromMemory(std::string const& script);
	luabridge::LuaRef getGlobal(const char* name);

	void PrintFromLua(const char*);

	template<class type>
	inline void AddFunction(const char * name, type func)
	{
		luabridge::getGlobalNamespace(g_luaState.get())
			.addFunction(name, func);
	}
};