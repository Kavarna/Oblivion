#pragma once

#include "../common/common.h"
#include "../common/interfaces/Singletone.h"

#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class BulletWorld sealed : public Singletone<BulletWorld>
{
	friend class btDebugDraw;
public:
	BulletWorld();
	~BulletWorld();

	void Create() override;
	void CreateDefaultWorld();

	void Update(float frameTime);

	void AddRigidBody(btRigidBody*);
	void RemoveRigidBody(btRigidBody*);

private:
	btCollisionConfiguration*	m_collisionConfiguration;
	btConstraintSolver*			m_constraintSolver;
	btBroadphaseInterface*		m_broadphase;
	btDispatcher*				m_collisionDispatcher;
	btDynamicsWorld*			m_world;
};