#pragma once


#include "../../common/common.h"
#include "../../common/commontypes.h"
#include "../Texture.h"
#include "Camera.h"
#include "AlignedObject.h"
#include "Shader.h"
#include "Object.h"

#include <OblivionObjects.h>

namespace Rendering
{
	typedef struct material_t
	{
		std::string name;
		bool hasTexture;
		bool hasBumpMap;
		bool hasSpecularMap;
		DirectX::XMFLOAT4 diffuseColor;
		float specular;
		float opacity;
		std::unique_ptr<Texture> diffuseTexture;
		std::unique_ptr<Texture> bumpMap;
		std::unique_ptr<Texture> specularMap;

		material_t() : hasTexture(false),
			hasBumpMap(false),
			hasSpecularMap(false) {};
	} SMaterial, Material;
}

class IGameObject : public AlignedObject, public IObject
{
	friend class Direct3D11;
public:
	IGameObject() = default;
	virtual ~IGameObject() 
	{ };

public:
	virtual void Create(std::string const&) = 0;
	virtual void Update(float frameTime) = 0;
	virtual void Destroy() = 0;

public:
	inline void Identity(int instanceID = 0) { m_objectWorld[instanceID] = DirectX::XMMatrixIdentity(); };
	inline void Scale(float S, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixScaling(S, S, S); };
	inline void Translate(float x, float y, float z, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixTranslation(x, y, z); };
	inline void RotateX(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationX(Theta); };
	inline void RotateY(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationY(Theta); };
	inline void RotateZ(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationZ(Theta); };

protected:
	virtual uint32_t GetIndexCount(int subObject = 0) const = 0;
	virtual uint32_t GetVertexCount() const = 0;

public:
	/*
	* returns the index of the new added instance
	*/
	virtual DirectX::XMMATRIX&				AddInstance(DirectX::FXMMATRIX const& mat = DirectX::XMMatrixIdentity());
	virtual DirectX::XMMATRIX*				AddInstance(uint32_t number);
	virtual void							RemoveInstance(int ID);
	virtual void							RemoveInstance(CommonTypes::Range const& range);

protected:
	/*
	* Range between start index and end index
	*/
	static CommonTypes::Range				AddVertices(std::vector<Oblivion::SVertex> & vertices);
	static CommonTypes::Range				AddVertices(std::vector<Oblivion::SVertex> & vertices, int start, int end);
	static void								RemoveVertices(CommonTypes::Range const&);
	
public:
	static void								BindStaticVertexBuffer();

protected:
	static std::vector<Oblivion::SVertex>	m_staticVertices;
	static MicrosoftPointer<ID3D11Buffer>	m_staticVerticesBuffer;

protected:
	MicrosoftPointer<ID3D11Buffer>			m_instanceBuffer;
	std::vector<DirectX::XMMATRIX>			m_objectWorld;
};