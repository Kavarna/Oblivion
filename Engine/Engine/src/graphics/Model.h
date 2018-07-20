#pragma once


#include "interfaces/GameObject.h"
#include "Direct3D11.h"

enum class EDefaultObject
{
	Box, Sphere, Geosphere, Cylinder, Grid
};

class Model : public IGameObject
{
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
	template <class Shader, bool bindShader = false>
			void							Render(ICamera * cam) const;
	virtual void							Destroy() override;

public:
	inline	uint32_t						GetIndexCount(int subObject = 0) const;
	inline	uint32_t						GetVertexCount(int subObject = 0) const;

public:
			void							SetMaterial(Rendering::Material && mat, int materialIndex = 0);

private:
			void							RenderBasicShader(ICamera * cam) const;
			void							RenderTextureLightShader(ICamera * cam) const;
			void							RenderTexture(ICamera * cam) const;

private:
			void							DrawIndexedInstanced(ICamera * cam) const;
			void							BindMaterial(int index, int shader) const;

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
	MicrosoftPointer<ID3D11Buffer>			m_materialBuffer;
	
	DirectX::BoundingBox					m_boundingBox;

	std::vector<Mesh>						m_meshes;
	std::vector<Rendering::Material>		m_materials;
	std::vector<uint32_t>					m_indices;
};


template <class Shader, bool bindShader>
void Model::Render(ICamera * cam) const
{
	static_assert(std::is_base_of<IShader, Shader>::value,
		"Can't render a game object with a non-shader generic argument");

	m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11Context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	
	if (m_objectWorld.size() < 1)
		return;

	if constexpr (std::is_same<Shader,BasicShader>::value)
	{
		RenderBasicShader(cam);
	}
	else if constexpr (std::is_same<Shader, TextureLightShader>::value)
	{
		RenderTextureLightShader(cam);
	}
	else if constexpr (std::is_same<Shader, TextureShader>::value)
	{
		RenderTexture(cam);
	}
	else
	{
		static_assert(false,
			"Can't render model using this shader");
	}

	if constexpr (bindShader)
	{
		Shader::Get()->bind();
	}
	DrawIndexedInstanced(cam);
}
