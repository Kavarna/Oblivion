#pragma once


#include "interfaces/GameObject.h"
#include "Direct3D11.h"

enum class EDefaultObject
{
	Box, Sphere, Geosphere, Cylinder, Grid
};

class Model : public IGameObject
{
	typedef CommonTypes::RayHitInfo RayHitInfo;
public:
	Model();
	~Model();

public:
	virtual void Update(float frameTime) override
	{
	};

public:
	virtual void							Create(std::string const& filename) override;
			void							Create(EDefaultObject object);
	virtual void							Destroy() override;

public:
	inline	uint32_t						GetIndexCount(int subObject = 0) const;
	inline	uint32_t						GetVertexCount(int subObject = 0) const;

public:
			RayHitInfo	__vectorcall		PickObject(DirectX::FXMVECTOR& rayPos, DirectX::FXMVECTOR& rayDir) const;
			void							ImGuiChangeMaterial();

public:
			void							SetMaterial(Rendering::Material && mat, int materialIndex = 0);

protected:
			void							DrawIndexedInstanced(ICamera * cam, const Pipeline& p) const override;
			bool							PrepareIA(const Pipeline& p) const override;


private:
	virtual	bool							ShouldRenderInstance(ICamera * cam, uint32_t id) const;

private:
	struct Mesh
	{
		CommonTypes::Range					m_vertexRange;
		CommonTypes::Range					m_indexRange;
		int									m_materialIndex;
		// DirectX::BoundingBox				m_boundingBox; /// kinda useless
	};
private:
	MicrosoftPointer<ID3D11Buffer>			m_indexBuffer;
	
	DirectX::BoundingBox					m_boundingBox;

	mutable std::vector<uint32_t>			m_drawnInstances;

	std::vector<Mesh>						m_meshes;
	std::vector<Rendering::Material>		m_materials;
	std::vector<uint32_t>					m_indices;

	Rendering::Material*					m_selectedMaterial;
};

