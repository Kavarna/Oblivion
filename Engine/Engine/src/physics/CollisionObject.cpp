#include "CollisionObject.h"

CollisionObject::CollisionObject() :
	Model(),
	m_collisionType(ECollisionObjectType::eStatic)
{};

CollisionObject::CollisionObject(ECollisionObjectType col) :
	Model(),
	m_collisionType(col)
{};

CollisionObject::CollisionObject(Model * model, ECollisionObjectType col) :
	Model(),
	m_collisionType(col)
{
	THROW_ERROR("Collision object from a model not available yet");
}

CollisionObject::~CollisionObject()
{
	Destroy();
};


void CollisionObject::Create(std::string const & filename)
{
	Model::Create(filename);
}

void CollisionObject::Create(EDefaultObject object)
{
	Model::Create(object);

	switch (object)
	{
	case EDefaultObject::Box:
		m_collisionShape = new btBoxShape(btVector3(1, 1, 1));
		break;
	case EDefaultObject::Sphere:
	case EDefaultObject::Geosphere:
		m_collisionShape = new btSphereShape(1.0f);
		break;
	case EDefaultObject::Cylinder:
		m_collisionShape = new btCylinderShape(btVector3(0.5f, 1.5f, 0.5f));
		break;
	case EDefaultObject::Grid:
		m_collisionShape = new btBoxShape(btVector3(50.0f, 0.0f, 50.0f));
		break;
	default:
		break;
	}
}

void CollisionObject::Destroy()
{
	for (auto & body : m_rigidBodies)
	{
		delete body->m_motionState;
		delete body->m_body;
		delete body;
	}

	m_rigidBodies.clear();

	if (m_collisionShape)
		delete m_collisionShape;
}

uint32_t CollisionObject::AddInstance(DirectX::FXMMATRIX const & mat)
{
	btMotionState* motionState = new btDefaultMotionState();
	motionState->setWorldTransform(btTransform(Math::Matrix3x3FromXMMatrix(mat), Math::GetTranslationFromMatrix(mat)));
	btRigidBody::btRigidBodyConstructionInfo bodyCI(m_mass, motionState, m_collisionShape, CalculateLocalIntertia(m_mass));
	btRigidBody* body = new btRigidBody(bodyCI);

	uint32_t instanceID = IGameObject::AddInstance(mat);

	m_rigidBodies.emplace_back(new RigidBodyInfo(m_mass, motionState, body, instanceID));

	BulletWorld::Get()->AddRigidBody(body);

	return instanceID;
}

uint32_t CollisionObject::AddInstance(uint32_t num)
{
	uint32_t start = m_objectWorld.size();

	for (uint32_t i = 0; i < num; ++i)
	{
		btMotionState* motionState = new btDefaultMotionState();
		motionState->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1)));
		btRigidBody::btRigidBodyConstructionInfo bodyCI(m_mass, motionState, m_collisionShape, CalculateLocalIntertia(m_mass));
		btRigidBody * body = new btRigidBody(bodyCI);
		uint32_t instanceID = start + i;

		m_rigidBodies.emplace_back(new RigidBodyInfo(m_mass, motionState, body, instanceID));

		BulletWorld::Get()->AddRigidBody(body);
	}

	return IGameObject::AddInstance(num);
}

void CollisionObject::Update(float frameTime)
{
	for (auto & body : m_rigidBodies)
	{
		btTransform trans;
		body->m_motionState->getWorldTransform(trans);
		float m[16];
		trans.getOpenGLMatrix(m);
		m_objectWorld[body->m_instanceID] = DirectX::XMMATRIX(m);
	}
	Model::Update(frameTime);
}

void CollisionObject::SetMass(float mass)
{
	m_mass = mass;
}

btVector3 CollisionObject::CalculateLocalIntertia(float mass)
{
	if (mass == 0 || m_collisionType == ECollisionObjectType::eStatic)
		return btVector3(0, 0, 0);

	btVector3 ret;
	m_collisionShape->calculateLocalInertia(mass, ret);

	return ret;
}
