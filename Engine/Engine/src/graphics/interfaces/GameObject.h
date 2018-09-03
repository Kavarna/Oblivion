#pragma once


#include "../../common/common.h"
#include "../../common/commontypes.h"
#include "../Texture.h"
#include "ICamera.h"
#include "AlignedObject.h"
#include "Shader.h"
#include "Object.h"

#include "../Shaders/BasicShader.h"
#include "../Shaders/TextureLightShader.h"
#include "../Shaders/TextureShader.h"
#include "../Shaders/DisplacementShader.h"

#include "../Helpers/GraphicsDebugDraw.h"

namespace Rendering
{
	typedef struct material_t
	{
		std::string name;
		bool hasTexture = false;
		bool hasBumpMap = false;
		bool hasSpecularMap = false;
		DirectX::XMFLOAT4 diffuseColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		float specular = 32.0f;
		float opacity = 1.0f;
		float tessMin = 1.0f;
		float tessMax = 1.0f;
		float tessScale = 1.0f;
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
	friend class Entity;
public:
	IGameObject();
	virtual ~IGameObject() 
	{ };

public:
	virtual			void Create(std::string const&) = 0;
	virtual			void Create() {};
	virtual			void Update(float frameTime);
	virtual			void Destroy() = 0;

public:
	virtual inline	void Identity(int instanceID = 0) { m_objectWorld[instanceID] = DirectX::XMMatrixIdentity(); };
	virtual inline	void Scale(float S, int instanceID = 0) { Scale(S, S, S, instanceID); };
	virtual inline	void Scale(float Sx, float Sy, float Sz, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixScaling(Sx, Sy, Sz); };
	virtual inline	void Translate(float x, float y, float z, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixTranslation(x, y, z); };
	virtual inline	void RotateX(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationX(Theta); };
	virtual inline	void RotateY(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationY(Theta); };
	virtual inline	void RotateZ(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationZ(Theta); };

public:
public:
					void							SetName(std::string const& name) { m_objectName = name; };
					std::string						GetName() const { return m_objectName; };

protected:
	virtual			uint32_t						GetIndexCount(int subObject = 0) const = 0;
	virtual			uint32_t						GetVertexCount(int subObject = 0) const = 0;

public:
	virtual			uint32_t						AddInstance(DirectX::FXMMATRIX const& mat = DirectX::XMMatrixIdentity());
	virtual			uint32_t						AddInstance(uint32_t number);
	virtual			void							RemoveInstance(int ID);
	virtual			void							RemoveInstance(CommonTypes::Range const& range);
	virtual			int								PrepareInstances(std::function<bool(uint32_t)> & func) const;
	virtual			void							Render(ICamera * cam, const Pipeline& p) const;

	template <class Shader>
					void							Render(ICamera * cam) const;

protected:
	virtual			void							RenderBasic(ICamera * cam) const;
	virtual			void							RenderTexture(ICamera * cam) const;
	virtual			void							RenderTextureLight(ICamera * cam) const;
	virtual			void							RenderDisplacementTextureLight(ICamera * cam) const;
	virtual			void							DrawIndexedInstanced(ICamera * cam) const = 0;
	virtual			bool							PrepareIA(const Pipeline&) const = 0;

protected:
	/*
	* Range between start index and end index
	*/
	static			CommonTypes::Range				AddVertices(std::vector<Oblivion::SVertex> & vertices);
	static			CommonTypes::Range				AddVertices(std::vector<Oblivion::SVertex> & vertices, int start, int end);
	static			void							RemoveVertices(CommonTypes::Range const&);

protected:
					void							BindMaterial(Rendering::material_t const& mat, int shader) const;

public:
	static			void							BindStaticVertexBuffer();

protected:
	static	std::vector<Oblivion::SVertex>			m_staticVertices;
	static	MicrosoftPointer<ID3D11Buffer>			m_staticVerticesBuffer;

protected:
			MicrosoftPointer<ID3D11Buffer>			m_instanceBuffer;
			std::vector<DirectX::XMMATRIX>			m_objectWorld;
			MicrosoftPointer<ID3D11Buffer>			m_materialBuffer;
	mutable int										m_bindMaterialToShader;
			std::string								m_objectName;
};

template<class Shader>
inline void IGameObject::Render(ICamera * cam) const
{
	static_assert(std::is_base_of<IShader, Shader>::value,
		"Generic argument for Rende must be a IShader based class");

	if (m_objectWorld.size() < 1)
		return;

	if constexpr (std::is_same<Shader, BasicShader>::value)
	{
		PrepareIA(Pipeline::PipelineBasic);
		RenderBasic(cam);
	}
	else if constexpr (std::is_same<Shader, TextureShader>::value)
	{
		PrepareIA(Pipeline::PipelineTexture);
		RenderTexture(cam);
	}
	else if constexpr (std::is_same<Shader, TextureLightShader>::value)
	{
		PrepareIA(Pipeline::PipelineTextureLight);
		RenderTextureLight(cam);
	}
	else if constexpr (std::is_same<Shader, DisplacementShader>::value)
	{
		PrepareIA(Pipeline::PipelineDisplacementTextureLight);
		RenderDisplacementTextureLight(cam);
	}
	else
	{
		static_assert(false,
			"Can't render a game object using this shader");
	}

	DrawIndexedInstanced(cam);
}
