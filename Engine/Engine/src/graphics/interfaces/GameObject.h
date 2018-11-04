#pragma once


#include "../../common/common.h"
#include "../Texture.h"
#include "ICamera.h"
#include "../../common/interfaces/AlignedObject.h"
#include "Shader.h"
#include "GraphicsObject.h"

#include "../Pipelines/Pipeline.h"
#include "../Pipelines/BasicPipeline.h"
#include "../Pipelines/TexturePipeline.h"
#include "../Pipelines/TextureLightPipeline.h"
#include "../Pipelines/DisplacementLightPipeline.h"
#include "../Pipelines/DepthmapPipeline.h"

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
		std::shared_ptr<Texture> diffuseTexture;
		std::shared_ptr<Texture> bumpMap;
		std::shared_ptr<Texture> specularMap;

		material_t() : hasTexture(false),
			hasBumpMap(false),
			hasSpecularMap(false) {};
	} SMaterial, Material;
}

class IGameObject : public IAlignedObject, public IGraphicsObject
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
					void							SetName(std::string const& name) { m_objectName = name; };
					std::string						GetName() const { return m_objectName; };

protected:
	virtual			uint32_t						GetIndexCount(int subObject = 0) const = 0;
	virtual			uint32_t						GetVertexCount(int subObject = 0) const = 0;

public:
	virtual			uint32_t						AddInstance(DirectX::FXMMATRIX const& mat = DirectX::XMMatrixIdentity());
	virtual			uint32_t						AddInstance(uint32_t number);
	virtual			uint32_t						GetNumInstances() const { return (uint32_t)m_objectWorld.size(); };
	virtual			void							ClearInstances() { m_objectWorld.clear(); };
	virtual			void							RemoveInstance(int ID);
	virtual			void							RemoveInstance(CommonTypes::Range const& range);
	virtual			int								PrepareInstances(const std::function<bool(uint32_t)> & func,
														const std::function<DirectX::XMMATRIX(DirectX::FXMMATRIX)>& modifyWorld = std::function<DirectX::XMMATRIX(DirectX::FXMMATRIX)>()) const;
	virtual			void							Render(ICamera * cam, const PipelineEnum& p) const;

	template <class Pipeline>
					void							Render(ICamera * cam) const;

protected:
	virtual			void							RenderBasic(ICamera * cam) const;
	virtual			void							RenderTexture(ICamera * cam) const;
	virtual			void							RenderTextureLight(ICamera * cam) const;
	virtual			void							RenderDisplacementTextureLight(ICamera * cam) const;
	virtual			void							DrawIndexedInstanced(ICamera * cam,
														const std::function<void(UINT,UINT,UINT)> & render = std::function<void(UINT,UINT,UINT)>()) const = 0;
	virtual			bool							PrepareIA(const PipelineEnum&) const = 0;

protected:
	/*
	* Range between start index and end index
	*/
	static			CommonTypes::Range				AddVertices(std::vector<Oblivion::SVertex> & vertices);
	static			CommonTypes::Range				AddVertices(std::vector<Oblivion::SVertex> & vertices, int start, int end);
	static			void							RemoveVertices(CommonTypes::Range const&);
	static			CommonTypes::Range				AddIndices(std::vector<uint32_t> & indices);
	static			CommonTypes::Range				AddIndices(std::vector<uint32_t> & indices, int start, int end);
	static			void							RemoveIndices(CommonTypes::Range const&);

protected:
					void							BindMaterial(Rendering::material_t const& mat, int shader) const;

public:
	static			void							BindStaticVertexBuffer();

protected:
	static	std::vector<Oblivion::SVertex>			m_staticVertices;
	static	MicrosoftPointer<ID3D11Buffer>			m_staticVerticesBuffer;
	static	std::vector<uint32_t>					m_staticIndices;
	static	MicrosoftPointer<ID3D11Buffer>			m_staticIndicesBuffer;

protected:
			MicrosoftPointer<ID3D11Buffer>			m_instanceBuffer;
			std::vector<DirectX::XMMATRIX>			m_objectWorld;
			ConstantBufferHandle					m_materialBuffer;
	mutable int										m_bindMaterialToShader;
	mutable std::vector<uint32_t>					m_drawnInstances;
			std::string								m_objectName;
};


template <class Pipeline>
inline void IGameObject::Render(ICamera * cam) const
{
	static_assert(std::is_base_of<IPipeline, Pipeline>::value,
		"Generic argument for Render(ICamera * cam) must be a IPipeline based class");

	if (m_objectWorld.size() < 1)
		return;

	if constexpr (std::is_same<Pipeline, BasicPipeline>::value)
	{
		if (!PrepareIA(PipelineEnum::PipelineBasic))
			return;
		RenderBasic(cam);
	}
	else if constexpr (std::is_same<Pipeline, TexturePipeline>::value)
	{
		if (!PrepareIA(PipelineEnum::PipelineTexture))
			return;
		RenderTexture(cam);
	}
	else if constexpr (std::is_same<Pipeline, TextureLightPipeline>::value)
	{
		if (!PrepareIA(PipelineEnum::PipelineTextureLight))
			return;
		RenderTextureLight(cam);
	}
	else if constexpr (std::is_same<Pipeline, DisplacementLightPipeline>::value)
	{
		if (!PrepareIA(PipelineEnum::PipelineDisplacementTextureLight))
			return;
		RenderDisplacementTextureLight(cam);
	}
	else if constexpr (std::is_same<Pipeline, DepthmapPipeline>::value)
	{
		auto pipeline = Pipeline::Get();
		// Just use the closes IA and rendering
		if (pipeline->useDisplacement())
		{
			if (!PrepareIA(PipelineEnum::PipelineDisplacementTextureLight))
				return;
			RenderDisplacementTextureLight(cam);
		}
		else
		{
			if (!PrepareIA(PipelineEnum::PipelineTextureLight))
				return;
			RenderTextureLight(cam);
		}
	}

	if (m_objectWorld.size() == 1)
	{
		auto drawFunc = [&](UINT indexCount, UINT indexStart, UINT vertexStart)
		{
			m_d3d11Context->DrawIndexed(indexCount, indexStart, vertexStart);
		};
		Pipeline::Get()->bind(m_objectWorld[0], cam);
		DrawIndexedInstanced(cam, drawFunc);
	}
	else
	{
		auto drawFunc = [&](UINT indexCount, UINT indexStart, UINT vertexStart)
		{
			m_d3d11Context->DrawIndexedInstanced(indexCount, m_drawnInstances.size(), indexStart, vertexStart, 0);
		};
		Pipeline::Get()->bind(cam);
		DrawIndexedInstanced(cam, drawFunc);
	}
}