#include "CollisionObject.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>

CollisionObject::CollisionObject() :
	Model(),
	m_collisionType(ECollisionObjectType::eStatic)
{};

CollisionObject::~CollisionObject()
{
	Destroy();
};


void CollisionObject::Create(std::string const & filename)
{
	Model::Create(filename);

	if (m_shapeType != EShapeType::eDefaultMesh)
	{
		ReadPhysicsFile(filename + ".physics");
		switch (m_shapeType)
		{
		case EShapeType::eGImpactMesh:
			break;
		case EShapeType::eStaticMesh:
			m_collisionShape = CreateStaticCollisionShape();
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
		InitDefaultProperties();
		m_collisionShape = new btBoxShape(btVector3(1, 1, 1));
		break;
	case EDefaultObject::Sphere:
	case EDefaultObject::Geosphere:
		InitDefaultProperties();
		m_collisionShape = new btSphereShape(1.0f);
		break;
	case EDefaultObject::Cylinder:
		InitDefaultProperties();
		m_collisionShape = new btCylinderShape(btVector3(0.5f, 1.5f, 0.5f));
		break;
	case EDefaultObject::Grid:
		InitDefaultProperties(ECollisionObjectType::eStatic);
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

	for (auto it = m_usedShapes.begin(); it != m_usedShapes.end(); ++it)
		delete (*it);
	m_usedShapes.clear();

	for (auto it = m_usedMeshes.begin(); it != m_usedMeshes.end(); ++it)
		delete (*it);
	m_usedMeshes.clear();

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
	InitBodyWithProperties(body);

	BulletWorld::Get()->AddRigidBody(body);

	return instanceID;
}

uint32_t CollisionObject::AddInstance(uint32_t num)
{
	uint32_t start = (uint32_t)m_objectWorld.size();

	for (uint32_t i = 0; i < num; ++i)
	{
		btMotionState* motionState = new btDefaultMotionState();
		motionState->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1)));
		btRigidBody::btRigidBodyConstructionInfo bodyCI(m_mass, motionState, m_collisionShape, CalculateLocalIntertia(m_mass));
		btRigidBody * body = new btRigidBody(bodyCI);
		uint32_t instanceID = start + i;

		m_rigidBodies[instanceID] = new RigidBodyInfo(m_mass, motionState, body);
		InitBodyWithProperties(body);

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

inline void CollisionObject::ReadPhysicsFile(const std::string & filename)
{
	using boost::algorithm::to_lower_copy;
	m_physicsFile = filename;
	try
	{
		boost::property_tree::read_json(filename, m_properties);
	}
	catch (const boost::property_tree::json_parser_error& e)
	{
		const char error[] = "cannot open file";
		if (strncmp(e.what(), error, strlen(error)))
		{
			DX::OutputVDebugString(L"[LOG]: Couldn't find file %s, creating it.\n", filename);
			InitDefaultProperties();
			SavePhysics();
		}
	}
	m_mass = DX::GetAttributeOrValue(m_properties, "Physics.Mass", 0.0f);
	std::string collisionType = DX::GetAttributeOrValue<std::string, decltype(m_properties)>
		(m_properties, "Physics.Collision Shape.Dynamics", "eDontCare");
	if (to_lower_copy(collisionType) == "edontcare")
		m_collisionType = ECollisionObjectType::eStatic;
	else if (to_lower_copy(collisionType) == "estatic")
		m_collisionType = ECollisionObjectType::eStatic;
	else if (to_lower_copy(collisionType) == "edynamic")
		m_collisionType = ECollisionObjectType::eDynamic;
	else if (to_lower_copy(collisionType) == "ekinematic")
		m_collisionType = ECollisionObjectType::eKinematic;

	std::string collisionShape = DX::GetAttributeOrValue<std::string, decltype(m_properties)>
		(m_properties, "Physics.Collision Shape.Type", "eDontCare");
	if (to_lower_copy(collisionShape) == "edontcare")
		m_shapeType = EShapeType::eHullMesh;
	else if (to_lower_copy(collisionShape) == "ehullmesh")
		m_shapeType = EShapeType::eHullMesh;
	else if (to_lower_copy(collisionShape) == "estaticmesh")
		m_shapeType = EShapeType::eStaticMesh;
	else if (to_lower_copy(collisionShape) == "egimpactmesh")
		m_shapeType = EShapeType::eGImpactMesh;
	else if (to_lower_copy(collisionShape) == "edefaultmesh")
		m_shapeType = EShapeType::eHullMesh;

	m_compound = DX::GetAttributeOrValue<bool, decltype(m_properties)>
		(m_properties, "Physics.Collision Shape.Compound", false);
}

void CollisionObject::SavePhysics()
{
	boost::property_tree::write_json(m_physicsFile, m_properties);
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
	THROW_ERROR("Scaling not available for physics-based objects");
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

btCollisionShape * CollisionObject::CreateStaticCollisionShape()
{
	auto getVertexID = [&](uint32_t meshID, uint32_t vertexID)
	{
		uint32_t id = m_meshes[meshID].m_vertexRange.begin + vertexID;
		DirectX::XMFLOAT3 pos = m_staticVertices[id].Position;
		return btVector3(pos.x, pos.y, pos.z);
	};
	btCollisionShape * shape = nullptr;
	if (m_compound)
	{
		//THROW_ERROR("Compound static mesh unavailable");
		btCompoundShape * compoundShape = new btCompoundShape(true, (int)m_meshes.size());
		for (uint32_t i = 0; i < m_meshes.size(); ++i)
		{
			btTriangleMesh * mesh = new btTriangleMesh(true, false);
			for (uint32_t j = m_meshes[i].m_indexRange.begin; j < m_meshes[i].m_indexRange.end; j += 3)
			{
				btVector3 v0, v1, v2;
				v0 = getVertexID(i, m_indices[j]);
				v1 = getVertexID(i, m_indices[j + 1]);
				v2 = getVertexID(i, m_indices[j + 2]);
				mesh->addTriangle(v0, v1, v2);
			}
			m_usedMeshes.push_back(mesh);
			compoundShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1)), new btBvhTriangleMeshShape(mesh, false));
		}
		shape = compoundShape;
	}
	else
	{
		btTriangleMesh * mesh = new btTriangleMesh(true, false);
		for (uint32_t i = 0; i < m_meshes.size(); ++i)
		{
			for (uint32_t j = m_meshes[i].m_indexRange.begin; j < m_meshes[i].m_indexRange.end; j += 3)
			{
				btVector3 v0, v1, v2;
				v0 = getVertexID(i, m_indices[j]);
				v1 = getVertexID(i, m_indices[j + 1]);
				v2 = getVertexID(i, m_indices[j + 2]);
				mesh->addTriangle(v0, v1, v2);
			}
		}
		m_usedMeshes.push_back(mesh);
		shape = new btBvhTriangleMeshShape(mesh, true);
	}
	return shape;
}

btCollisionShape * CollisionObject::CreateHullCollisionShape()
{
	btCollisionShape * shape = nullptr;;
	if (m_compound)
	{
		btCompoundShape * compoundShape = new btCompoundShape(true, (int)m_meshes.size());
		for (auto & mesh : m_meshes)
		{
			btConvexHullShape * hullShape = new btConvexHullShape(
				(btScalar*)&m_staticVertices[mesh.m_vertexRange.begin],
				mesh.m_vertexRange.end - mesh.m_vertexRange.begin,
				sizeof(Oblivion::Vertex)
			);
			m_usedShapes.push_back(hullShape);
			compoundShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1)), hullShape);
		}
		shape = compoundShape;
	}
	else
	{
		uint32_t startIndex = INT_MAX, endIndex = 0;
		for (auto & mesh : m_meshes)
		{
			if (mesh.m_vertexRange.begin < startIndex)
				startIndex = mesh.m_vertexRange.begin;
			if (mesh.m_vertexRange.end > endIndex)
				endIndex = mesh.m_vertexRange.end;
		}
		shape = new btConvexHullShape(
			(btScalar*)&m_staticVertices[startIndex],
			endIndex - startIndex,
			sizeof(Oblivion::Vertex)
		);
	}
	return shape;
}

inline void CollisionObject::InitDefaultProperties(ECollisionObjectType shapeType)
{
	if (shapeType == ECollisionObjectType::eDynamic)
	{
		m_properties.put("Physics.Mass", 1.0f);
		m_properties.put("Physics.Friction", 1.0f);
		m_properties.put("Physics.Restitution", 0.5f);
		m_properties.put("Physics.Spinning friction", 0.2f);
		m_properties.put("Physics.Rolling friction", 0.2f);
		m_properties.put("Physics.Collision Shape.Dynamics", "eDynamic");
		m_properties.put("Physics.Collision Shape.Type", "eDontCare");
		m_properties.put("Physics.Collision Shape.Compound", m_compound);
		m_mass = 1.0f;
		m_collisionType = ECollisionObjectType::eDynamic;
		m_collisionType = ECollisionObjectType::eDontCare;
	}
	else
	{
		m_properties.put("Physics.Mass", 0.0f);
		m_properties.put("Physics.Friction", 1.0f);
		m_properties.put("Physics.Restitution", 0.5f);
		m_properties.put("Physics.Spinning friction", 0.2f);
		m_properties.put("Physics.Rolling friction", 0.2f);
		m_properties.put("Physics.Dynamics", "eStatic");
		m_properties.put("Physics.Collision Shape.Type", "eDontCare");
		m_properties.put("Physics.Collision Shape.Compound", m_compound);
		m_mass = 0.0f;
		m_collisionType = ECollisionObjectType::eStatic;
		m_collisionType = ECollisionObjectType::eDontCare;
	}
}

inline void CollisionObject::InitBodyWithProperties(btRigidBody * body)
{
	body->setFriction(DX::GetAttributeOrValue<float, decltype(m_properties)>(m_properties, "Physics.Friction", 0.5f));
	body->setRestitution(DX::GetAttributeOrValue<float, decltype(m_properties)>(m_properties, "Physics.Restitution", 0.3f));
	body->setSpinningFriction(DX::GetAttributeOrValue<float, decltype(m_properties)>(m_properties, "Physics.Spinning friction", 0.5f));
	body->setRollingFriction(DX::GetAttributeOrValue<float, decltype(m_properties)>(m_properties, "Physics.Rolling friction", 0.5f));
}
