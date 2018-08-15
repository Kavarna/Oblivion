#pragma once


#include "../common/common.h"
#include "../graphics/interfaces/GameObject.h"

#include "../graphics/Camera.h"
#include "../graphics/Projection.h"

#include "LuaManager.h"
#include "Script.h"

enum CameraType
{
	World = 1, Screen = 2
};

class Entity
{
	friend class IGameObject;
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
	void Identity(float instanceID);
	void Translate(float instanceID, float x, float y, float z);
	void RotateX(float instanceID, float theta);
	void RotateY(float instanceID, float theta);
	void RotateZ(float instanceID, float theta);
	void Scale1(float instanceID, float S);
	void Scale3(float instanceID, float Sx, float Sy, float Sz);
	void SetCamera(float cam);
	void SetPipeline(float pipeline);

public:
	void SetGameObject(IGameObject * object, const CommonTypes::Range& range);


private:
	uint32_t ClampInstance(uint32_t instanceID);

private:
	IGameObject * m_object;
	CommonTypes::Range m_instanceRange;

	ICamera * m_cameraToUse;
	Pipeline m_pipelineToUse;

};