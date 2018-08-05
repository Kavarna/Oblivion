#include "Script.h"

using namespace luabridge;
using namespace LuaManager;

DECLARE_SINGLETONE(ScriptManager);

Script::Script(std::filesystem::path const & path)
{
	LuaManager::LoadScriptFromFile(path.string());
}

Script::Script(std::string const & script)
{
	LuaManager::LoadScriptFromMemory(script);
}

void ScriptManager::Create()
{
}
