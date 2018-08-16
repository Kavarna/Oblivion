#include "GameScript.h"

US_NS_LUA;

void GameScript::SetScript(std::string const & tablename)
{
	LuaRef defaultScript = getGlobal(tablename.c_str());

	m_onLoadCallback = std::make_unique<LuaRef>(defaultScript["onLoad"]);
	if (m_onLoadCallback->isNil())
		m_onLoadCallback.reset();
	m_onUpdateCallback = std::make_unique<LuaRef>(defaultScript["onUpdate"]);
	if (m_onUpdateCallback->isNil())
		m_onUpdateCallback.reset();
	m_onRenderCallback = std::make_unique<LuaRef>(defaultScript["onRender"]);
	if (m_onRenderCallback->isNil())
		m_onRenderCallback.reset();
	m_onCleanupCallback = std::make_unique<LuaRef>(defaultScript["onCleanup"]);
	if (m_onCleanupCallback->isNil())
		m_onCleanupCallback.reset();
	m_onSizeCallback = std::make_unique<LuaRef>(defaultScript["onSize"]);
	if (m_onSizeCallback->isNil())
		m_onSizeCallback.reset();

	OnLoad();
}

void GameScript::OnLoad()
{
	if (m_onLoadCallback)
		(*m_onLoadCallback)();
}

void GameScript::OnUpdate(float frametime)
{
	if (m_onUpdateCallback)
		(*m_onUpdateCallback)(frametime);
}

void GameScript::OnRender()
{
	if (m_onRenderCallback)
		(*m_onRenderCallback)();
}

void GameScript::OnSize()
{
	if (m_onSizeCallback)
		(*m_onSizeCallback)();
}

void GameScript::OnCleanup()
{
	if (m_onCleanupCallback)
		(*m_onCleanupCallback)();
}
