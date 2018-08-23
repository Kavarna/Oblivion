#include "Entity.h"

using namespace luabridge;
using namespace LuaManager;

void Entity::LuaRegister()
{
	getGlobalNamespace(g_luaState.get())
		.beginClass<Entity>("Entity")
			.addFunction("Identity", &Entity::Identity)
			.addFunction("Translate",&Entity::Translate)
			.addFunction("RotateX", &Entity::RotateX)
			.addFunction("RotateY", &Entity::RotateY)
			.addFunction("RotateZ", &Entity::RotateZ)
			.addFunction("Scale1",&Entity::Scale1)
			.addFunction("Scale3", &Entity::Scale3)
			.addFunction("setCamera", &Entity::SetCamera)
			.addFunction("setPipeline", &Entity::SetPipeline)
		.endClass();
}

Entity::Entity()
{
}

Entity::~Entity()
{
	OnCleanup();
}

void Entity::SetScript(std::string const & tablename)
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
	m_onRenderCallCallback = std::make_unique<LuaRef>(defaultScript["onRenderCall"]);
	if (m_onRenderCallCallback->isNil())
		m_onRenderCallCallback.reset();
	m_onPickCallback = std::make_unique<LuaRef>(defaultScript["onPick"]);
	if (m_onPickCallback->isNil())
		m_onPickCallback.reset();
	m_onCleanupCallback = std::make_unique<LuaRef>(defaultScript["onCleanup"]);
	if (m_onCleanupCallback->isNil())
		m_onCleanupCallback.reset();

	OnLoad();
}

void Entity::OnLoad()
{
	if (m_onLoadCallback)
		(*m_onLoadCallback)(this);
}

void Entity::OnUpdate(float frametime)
{
	if (m_onUpdateCallback)
		(*m_onUpdateCallback)(this, frametime);
}

bool Entity::OnRender()
{
	if (m_onRenderCallback)
		return (*m_onRenderCallback)(this);
	return true;
}

void Entity::OnRenderCall()
{
	if (m_onRenderCallCallback)
		(*m_onRenderCallCallback)(this);
}

void Entity::OnPick()
{
	if (m_onPickCallback)
		(*m_onPickCallback)(this);
}

void Entity::OnCleanup()
{
	if (m_onCleanupCallback)
		(*m_onCleanupCallback)(this);
}

void Entity::Identity(float instanceID)
{
	m_object->Identity(ClampInstance((uint32_t)instanceID));
}

void Entity::Translate(float instanceID, float x, float y, float z)
{
	m_object->Translate(x, y, z, ClampInstance((uint32_t)instanceID));
}

void Entity::RotateX(float instanceID, float theta)
{
	m_object->RotateX(theta, ClampInstance((uint32_t)instanceID));
}

void Entity::RotateY(float instanceID, float theta)
{
	m_object->RotateY(theta, ClampInstance((uint32_t)instanceID));
}

void Entity::RotateZ(float instanceID, float theta)
{
	m_object->RotateZ(theta, ClampInstance((uint32_t)instanceID));
}

void Entity::Scale1(float instanceID, float S)
{
	m_object->Scale(S, ClampInstance((uint32_t)instanceID));
}

void Entity::Scale3(float instanceID, float Sx, float Sy, float Sz)
{
	m_object->Scale(Sx, Sy, Sz, ClampInstance((uint32_t)instanceID));
}

void Entity::SetCamera(float cam)
{
	int iCam = (int)cam;
	CameraType camType = static_cast<CameraType>(iCam);
	switch (camType)
	{
	case World:
		m_cameraToUse = g_camera.get();
		break;
	case Screen:
		m_cameraToUse = g_screen.get();
		break;
	default:
		break;
	}
}

void Entity::SetPipeline(float pipeline)
{
	int pipe = (int)pipeline;
	m_pipelineToUse = (Pipeline)pipe;
}

void Entity::SetGameObject(IGameObject * object, const CommonTypes::Range& range)
{
	m_object = object;
	m_instanceRange = range;
}

uint32_t Entity::ClampInstance(uint32_t instanceID)
{
	return instanceID + m_instanceRange.begin;
}

