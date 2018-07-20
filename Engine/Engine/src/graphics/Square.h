#pragma once


#include "interfaces/GameObject.h"


class Square :
	public IGameObject
{
public:
	Square();
	~Square();

	// Inherited via IGameObject
			void						Create(std::string const &) override;
			void						Create() override;
			void						Update(float frameTime) override;
			void						Destroy() override;
	inline	uint32_t					GetIndexCount(int subObject = 0) const override;
	inline	uint32_t					GetVertexCount(int subObject = 0) const override;

	template <class Shader, bool bindShader = false>
			void						Render(ICamera * cam) const;


private:
			void						RenderTexture(ICamera * cam) const;

private:
			void						DrawIndexed(ICamera * cam) const;
private:
	MicrosoftPointer<ID3D11Buffer>		m_indexBuffer;
	CommonTypes::Range					m_vertexRange;
	std::vector<uint32_t>				m_indices;
	Rendering::SMaterial				m_material;
};

template<class Shader, bool bindShader>
inline void Square::Render(ICamera * cam) const
{
	static_assert(std::is_base_of<IShader, Shader>::value,
		"Can't render a game object with a non-shader generic argument");

	m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11Context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	if (m_objectWorld.size() < 1)
		return;

	if constexpr (std::is_same<Shader, TextureShader>::value)
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
	DrawIndexed(cam);
}
