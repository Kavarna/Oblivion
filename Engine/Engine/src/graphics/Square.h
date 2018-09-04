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
			void						SetTexture(std::unique_ptr<Texture>&& tex);


			void						SetWindowInfo(float windowWidth, float windowHeight);
	inline	void						Scale(float Sx, float Sy, float Sz = 1.0f, int instanceID = 0) override;

			void						TranslateTo(float X, float Y, int InstanceID = 0);
private:
			void						DrawIndexedInstanced(ICamera * cam) const override;
			bool						PrepareIA(const Pipeline& p) const override;
private:
	float								m_width;
	float								m_height;
	float								m_windowWidth;
	float								m_windowHeight;

	MicrosoftPointer<ID3D11Buffer>		m_indexBuffer;
	CommonTypes::Range					m_vertexRange;
	std::vector<uint32_t>				m_indices;
	Rendering::SMaterial				m_material;
};