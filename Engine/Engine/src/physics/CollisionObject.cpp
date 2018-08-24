#include "CollisionObject.h"

CollisionObject::CollisionObject() :
	Model(),
	m_collisionType(ECollisionObjectType::eStatic)
{};

CollisionObject::CollisionObject(float mass, ECollisionObjectType col) :
	Model(),
	m_mass(mass),
	m_collisionType(ECollisionObjectType::eDontCare)
{
	if (col == ECollisionObjectType::eKinematic)
		THROW_ERROR("Can't create a kinematic collision object yet");
};

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

	if (m_shapeType != EShapeType::eDefaultMesh)
	{
		switch (m_shapeType)
		{
		case EShapeType::eGImpactMesh:
			break;
		case EShapeType::eStaticMesh:
			break;
		case EShapeType::eDontCare:
		case EShapeType::eHullMesh:
			m_collisionShape = CreateHullCollisionShape();
			break;
		default:
			break;
		}
	}
}

void CollisionObject::Create(EDefaultObject object)
{
	Model::Create(object);
	m_shapeType = EShapeType::eDefaultMesh;
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
	for (auto it = m_rigidBodies.begin(); it != m_rigidBodies.end(); ++it)
	{
		delete it->second->m_motionState;
		delete it->second->m_body;
		delete it->second;
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

	m_rigidBodies[instanceID] = new RigidBodyInfo(m_mass, motionState, body);

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

		m_rigidBodies[instanceID] = new RigidBodyInfo(m_mass, motionState, body);

		BulletWorld::Get()->AddRigidBody(body);
	}

	return IGameObject::AddInstance(num);
}

void CollisionObject::Update(float frameTime)
{
	for (auto it = m_rigidBodies.begin(); it != m_rigidBodies.end(); ++it)
	{
		btTransform trans = it->second->m_body->getWorldTransform();
		float m[16];
		trans.getOpenGLMatrix(m);
		m_objectWorld[it->first] = DirectX::XMMATRIX(m);
	}
	Model::Update(frameTime);
}

inline void CollisionObject::RotateX(float theta, int instanceID)
{
	if (m_collisionType == ECollisionObjectType::eDynamic)
	{
		Activate(instanceID);
		m_rigidBodies[instanceID]->m_body->setAngularVelocity(btVector3(theta, 0, 0));
	}
	else
	{
		btMatrix3x3 rotMat;
		rotMat.setEulerYPR(0, theta, 0); // Pitch = theta
		btTransform trans;
		m_rigidBodies[instanceID]->m_motionState->getWorldTransform(trans);
		btMatrix3x3 curRot = trans.getBasis();
		rotMat = curRot * rotMat;
		trans.setBasis(rotMat);
		m_rigidBodies[instanceID]->m_motionState->setWorldTransform(trans);
	}
}

inline void CollisionObject::RotateY(float theta, int instanceID)
{
	if (m_collisionType == ECollisionObjectType::eDynamic)
	{
		Activate(instanceID);
		m_rigidBodies[instanceID]->m_body->setAngularVelocity(btVector3(0, theta, 0));
	}
	else
	{
		btMatrix3x3 rotMat;
		rotMat.setEulerYPR(theta, 0, 0); // Pitch = theta
		btTransform trans;
		m_rigidBodies[instanceID]->m_motionState->getWorldTransform(trans);
		btMatrix3x3 curRot = trans.getBasis();
		rotMat = curRot * rotMat;
		trans.setBasis(rotMat);
		m_rigidBodies[instanceID]->m_motionState->setWorldTransform(trans);
	}
}

inline void CollisionObject::RotateZ(float theta, int instanceID)
{
	if (m_collisionType == ECollisionObjectType::eDynamic)
	{
		Activate(instanceID);
		m_rigidBodies[instanceID]->m_body->setAngularVelocity(btVector3(0, 0, theta));
	}
	else
	{
		btMatrix3x3 rotMat;
		rotMat.setEulerYPR(0, 0, theta); // Pitch = theta
		btTransform trans;
		m_rigidBodies[instanceID]->m_motionState->getWorldTransform(trans);
		btMatrix3x3 curRot = trans.getBasis();
		rotMat = curRot * rotMat;
		trans.setBasis(rotMat);
		m_rigidBodies[instanceID]->m_motionState->setWorldTransform(trans);
	}
}

inline void CollisionObject::Scale(float Sx, float Sy, float Sz, int instanceID)
{
	THROW_ERROR("Scaling not available for physics-basec objects");
	/*if (m_rigidBodies[instanceID]->m_collisionShape)
	{
		m_collisionShape->setLocalScaling(btVector3(Sx, Sy, Sz));
	}
	else
	{
		for (auto it = m_rigidBodies.begin(); it != m_rigidBodies.end(); ++it)
		{
			RigidBodyInfo* object = it->second;
			
		}
	}*/
}

btVector3 CollisionObject::CalculateLocalIntertia(float mass)
{
	if (mass == 0 || m_collisionType == ECollisionObjectType::eStatic)
		return btVector3(0, 0, 0);

	btVector3 ret;
	m_collisionShape->calculateLocalInertia(mass, ret);
	
	return ret;
}

btCollisionShape * CollisionObject::CreateHullCollisionShape()
{
	uint32_t startIndex = INT_MAX, endIndex = 0;
	for (auto & mesh : m_meshes)
	{
		if (mesh.m_vertexRange.begin < startIndex)
			startIndex = mesh.m_vertexRange.begin;
		if (mesh.m_vertexRange.end > endIndex)
			endIndex = mesh.m_vertexRange.end;
	}
	btConvexHullShape * hullShape = new btConvexHullShape(
		(btScalar*)&m_staticVertices[startIndex],
		endIndex - startIndex,
		sizeof(Oblivion::Vertex)
	);
	return hullShape;
}
