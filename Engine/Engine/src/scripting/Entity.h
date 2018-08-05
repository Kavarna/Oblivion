#pragma once


#include "../common/common.h"
#include "../graphics/interfaces/GameObject.h"
#include "LuaManager.h"
#include "Script.h"

class Entity
{
public:
	Entity();
	~Entity();

public:
	static void LuaRegister();

private:
	std::unique_ptr<luabridge::LuaRef> m_onLoadCallback;
	std::unique_ptr<luabridge::LuaRef> m_onUpdateCallback;
	std::unique_ptr<luabridge::LuaRef> m_onRenderCallback;
	std::unique_ptr<luabridge::LuaRef> m_onRenderCallCallback;
	std::unique_ptr<luabridge::LuaRef> m_onPickCallback;
	std::unique_ptr<luabridge::LuaRef> m_onCleanupCallback;

public:
	void SetScript(std::string const& tablename);

public: // Callbacks
	void OnLoad();
	void OnUpdate(float frametime);
	bool OnRender();
	void OnRenderCall();
	void OnPick();
	void OnCleanup();

public:
	void Identity();
	void Translate(float x, float y, float z);
	void RotateX(float theta);
	void RotateY(float theta);
	void RotateZ(float theta);
	void Scale1(float S);
	void Scale3(float Sx, float Sy, float Sz);

public:
	void SetGameObject(IGameObject * object, uint32_t instanceID);

private:
	IGameObject * m_object;
	uint32_t m_instanceID;

};