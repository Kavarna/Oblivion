#pragma once


#include "../graphics/Model.h"
#include "../common/commonmath.h"
#include "World.h"

enum class ECollisionObjectType
{
	eStatic, eDynamic, eKinematic
};

class CollisionObject : public Model
{
public:
	CollisionObject();
	CollisionObject(ECollisionObjectType);
	CollisionObject(Model*, ECollisionObjectType);
	~CollisionObject();

public:
	void							Create(std::string const& filename) override;
	void							Create(EDefaultObject object) override;
	void							Destroy() override;

	uint32_t						AddInstance(DirectX::FXMMATRIX const& mat = DirectX::XMMatrixIdentity()) override;
	uint32_t						AddInstance(uint32_t num) override;
	
	void							Update(float frameTime) override;

	void							SetMass(float mass);

	void							Translate(float x, float y, float z, int instanceID) override 
	{
		for (uint32_t i = 0; i < m_rigidBodies.size(); ++i)
			if (m_rigidBodies[i]->m_instanceID == instanceID)
			{
				m_rigidBodies[i]->m_body->translate(btVector3(x, y, z));
				return;
			}
	}

private:
	struct RigidBodyInfo
	{
		float			m_mass;
		btMotionState*	m_motionState;
		btRigidBody*	m_body;
		uint32_t		m_instanceID;

		RigidBodyInfo() = default;
		RigidBodyInfo(float mass, btMotionState * state, btRigidBody * bd, uint32_t iid) :
			m_mass(mass), m_motionState(state), m_body(bd), m_instanceID(iid) {};
	};

private:
	inline btVector3				CalculateLocalIntertia(float mass);

private:
	float							m_mass;
	ECollisionObjectType			m_collisionType;
	btCollisionShape*				m_collisionShape				= nullptr;
	std::vector<RigidBodyInfo*>		m_rigidBodies;
};
