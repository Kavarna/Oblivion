#pragma once

#include "../common/common.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class BulletWorld
{
	friend class btDebugDraw;
public:
	BulletWorld();
	~BulletWorld();

	MAKE_SINGLETONE(BulletWorld);

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