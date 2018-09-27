#pragma once


#include "../graphics/Model.h"
#include "../common/commonmath.h"
#include "World.h"
#include <boost/property_tree/ptree.hpp>

enum class ECollisionObjectType
{
	eStatic, eDynamic, eKinematic, eDontCare
};

enum class EShapeType
{
	eStaticMesh, // Aka eTriangleMesh
	eGImpactMesh,
	eHullMesh, eDefaultMesh, eDontCare
};

class CollisionObject : public Model
{
public:
	CollisionObject();
	~CollisionObject();

public:
			void							Create(std::string const& filename) override;
			void							Create(EDefaultObject object) override;
			void							Destroy() override;

			uint32_t						AddInstance(DirectX::FXMMATRIX const& mat = DirectX::XMMatrixIdentity()) override;
			uint32_t						AddInstance(uint32_t num) override;
			uint32_t						AddInstance(float mass, DirectX::FXMMATRIX const& mat = DirectX::XMMatrixIdentity());
			
			void							Update(float frameTime) override;

			void							ReadPhysicsFile(const std::string& filename);
			void							SavePhysics();
	inline	void							Activate(int instanceID);
	inline	void							Identity(int instanceID) override;
	inline	void							Translate(float x, float y, float z, int instanceID) override;
	inline	void							RotateX(float theta, int instanceID = 0) override;
	inline	void							RotateY(float theta, int instanceID = 0) override;
	inline	void							RotateZ(float theta, int instanceID = 0) override;
	inline	void							Scale(float Sx, float Sy, float Sz, int instanceID = 0) override;
	inline	void							GlobalScale(float Sx, float Sy, float Sz)
	{
		m_collisionShape->setLocalScaling(btVector3(Sx, Sy, Sz));
	}
	inline	void							Impulse(float x, float y, float z, uint32_t instanceID = 0)
	{
		if (!isIDinstance(instanceID) || m_collisionType == ECollisionObjectType::eStatic)
			return;
		auto body = m_rigidBodies[instanceID]->m_body;
		body->applyImpulse(btVector3(x, y, z), btVector3(0, 0, 0));//body->getCenterOfMassPosition());
	}
	inline	void							Deactivate(int instanceID)
	{
		if (!isIDinstance(instanceID))
			return;
		m_rigidBodies[instanceID]->m_body->activate(false);
	}


private:
	inline	bool							isIDinstance(uint32_t ID);

private:
	struct RigidBodyInfo
	{
		float				m_mass;
		btMotionState*		m_motionState		= nullptr;
		btRigidBody*		m_body				= nullptr;
		bool				m_hasDifferentShape	= false;

		RigidBodyInfo() = default;
		RigidBodyInfo(float mass, btMotionState * state, btRigidBody * bd) :
			m_mass(mass), m_motionState(state), m_body(bd) { };
	};

private:
	inline	btVector3							CalculateLocalIntertia(float mass);
			btCollisionShape*					CreateStaticCollisionShape();
			btCollisionShape*					CreateHullCollisionShape();
			btCollisionShape*					CreateGImpactCollisionShape();
	inline	void								InitDefaultProperties(ECollisionObjectType = ECollisionObjectType::eDynamic);
	inline	void								InitBodyWithProperties(btRigidBody*);

private:
	float										m_mass;
	ECollisionObjectType						m_collisionType;
	btCollisionShape*							m_collisionShape				= nullptr;
	EShapeType									m_shapeType						= EShapeType::eDontCare;
	EDefaultObject								m_defaultShape;
	std::map<uint32_t, RigidBodyInfo*>			m_rigidBodies;
	boost::property_tree::ptree					m_properties;
	std::string									m_physicsFile;
	bool										m_compound						= false;

	std::vector<btCollisionShape*>				m_usedShapes;
	std::vector<btTriangleMesh*>				m_usedMeshes;
};
