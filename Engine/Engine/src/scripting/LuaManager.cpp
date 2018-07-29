#include "LuaManager.h"

#define ADD_CLASS_FUNCTION(function, type) addFunction(#function, &type::function)
#define ADD_FUNCTION(function) addFunction(#function,function);

namespace LuaManager
{
	using namespace luabridge;

	unique_delete_ptr<lua_State> g_luaState;


	void LuaManager::InitializeLua()
	{
		g_luaState = unique_delete_ptr<lua_State>(luaL_newstate(),
			[](lua_State* l)
		{
			lua_close(l);
		});

		luaL_openlibs(g_luaState.get());
	}

	void LuaManager::LoadScript(std::string const & file)
	{
		if (luaL_dofile(g_luaState.get(), file.c_str()))
		{
			THROW_ERROR("Can't open script %s", file.c_str());
		}
	}

	luabridge::LuaRef getGlobal(const char* name)
	{
		return luabridge::getGlobal(g_luaState.get(), name);
	}


}