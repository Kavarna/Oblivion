#include "World.h"

BulletWorld::BulletWorld()
{
}

BulletWorld::~BulletWorld()
{
	delete m_world;
	delete m_collisionConfiguration;
	delete m_constraintSolver;
	delete m_broadphase;
	delete m_collisionDispatcher;
}

void BulletWorld::Create()
{
	DX::OutputVDebugString(L"Creating default bullet world\n");
	CreateDefaultWorld();
}

void BulletWorld::CreateDefaultWorld()
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_constraintSolver = new btSequentialImpulseConstraintSolver();
	m_broadphase = new btDbvtBroadphase();
	m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher,
			m_broadphase, m_constraintSolver, m_collisionConfiguration);
	
	m_world->setGravity(btVector3(0, -10, 0));
}

void BulletWorld::Update(float frameTime)
{
	m_world->stepSimulation(frameTime);
}

void BulletWorld::AddRigidBody(btRigidBody * body)
{
	m_world->addRigidBody(body);
}

void BulletWorld::RemoveRigidBody(btRigidBody * body)
{
	m_world->removeRigidBody(body);
}
