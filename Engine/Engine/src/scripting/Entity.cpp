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

void Entity::Identity()
{
	m_object->m_objectWorld[m_instanceID] = DirectX::XMMatrixIdentity();
}

void Entity::Translate(float x, float y, float z)
{
	m_object->m_objectWorld[m_instanceID] = DirectX::XMMatrixTranslation(x, y, z);
}

void Entity::RotateX(float theta)
{
	m_object->m_objectWorld[m_instanceID] = DirectX::XMMatrixRotationX(theta);
}

void Entity::RotateY(float theta)
{
	m_object->m_objectWorld[m_instanceID] = DirectX::XMMatrixRotationY(theta);
}

void Entity::RotateZ(float theta)
{
	m_object->m_objectWorld[m_instanceID] = DirectX::XMMatrixRotationZ(theta);
}

void Entity::Scale1(float S)
{
	m_object->m_objectWorld[m_instanceID] = DirectX::XMMatrixScaling(S, S, S);
}

void Entity::Scale3(float Sx, float Sy, float Sz)
{
	m_object->m_objectWorld[m_instanceID] = DirectX::XMMatrixScaling(Sx, Sy, Sz);
}

void Entity::SetGameObject(IGameObject * object, uint32_t instanceID)
{
	m_object = object;
	m_instanceID = instanceID;
}

