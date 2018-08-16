#pragma once

#include "LuaManager.h"
#include "Script.h"

class GameScript
{
public:
	GameScript() = default;
	~GameScript() = default;

public:
	void SetScript(std::string const& tablename);

public: // Callbacks
	void OnLoad();
	void OnUpdate(float frametime);
	void OnRender();
	void OnSize();
	void OnCleanup();

private:
	std::unique_ptr<luabridge::LuaRef> m_onLoadCallback;
	std::unique_ptr<luabridge::LuaRef> m_onUpdateCallback;
	std::unique_ptr<luabridge::LuaRef> m_onRenderCallback;
	std::unique_ptr<luabridge::LuaRef> m_onSizeCallback;
	std::unique_ptr<luabridge::LuaRef> m_onCleanupCallback;
};