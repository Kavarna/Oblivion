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

	inline	void							Identity(int instanceID) override
	{
		m_rigidBodies[instanceID]->m_body->
			getMotionState()->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1)));
	}
	inline	void							Translate(float x, float y, float z, int instanceID) override 
	{
		m_rigidBodies[instanceID]->m_body->translate(btVector3(x, y, z));
	}
	inline	void							RotateX(float theta, int instanceID = 0) override;
	inline	void							RotateY(float theta, int instanceID = 0) override;
	inline	void							RotateZ(float theta, int instanceID = 0) override;
	inline	void							Scale(float Sx, float Sy, float Sz, int instanceID = 0) override;

private:
	struct RigidBodyInfo
	{
		float				m_mass;
		btMotionState*		m_motionState		= nullptr;
		btRigidBody*		m_body				= nullptr;

		RigidBodyInfo() = default;
		RigidBodyInfo(float mass, btMotionState * state, btRigidBody * bd) :
			m_mass(mass), m_motionState(state), m_body(bd) {};
	};

private:
	inline	btVector3							CalculateLocalIntertia(float mass);

private:
	float										m_mass;
	ECollisionObjectType						m_collisionType;
	btCollisionShape*							m_collisionShape				= nullptr;
	std::map<uint32_t, RigidBodyInfo*>			m_rigidBodies;
};
