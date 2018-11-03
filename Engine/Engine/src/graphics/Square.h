#pragma once


#include "interfaces/GameObject.h"
#include "interfaces/I2DTransforms.h"


class Square :
	public IGameObject, public I2DTransforms
{
public:
	Square();
	~Square();

	// Inherited via IGameObject
	virtual	void						Create(std::string const &) override;
	virtual	void						Create(LPWSTR);
	virtual	void						Create() override;
			void						Update(float frameTime) override;
			void						Destroy() override;
	inline	uint32_t					GetIndexCount(int subObject = 0) const override;
	inline	uint32_t					GetVertexCount(int subObject = 0) const override;
			void						SetTexture(std::shared_ptr<Texture> tex);


	inline	void						Scale(float Sx, float Sy, float Sz = 1.0f, int instanceID = 0) override;

			void						TranslateTo(float X, float Y, int InstanceID = 0);
private:
			void						DrawIndexedInstanced(ICamera * cam, const std::function<void(UINT, UINT, UINT)>&) const override;
			bool						PrepareIA(const PipelineEnum& p) const override;
protected:
	float								m_width;
	float								m_height;


	CommonTypes::Range					m_vertexRange;
	CommonTypes::Range					m_indexRange;
	Rendering::SMaterial				m_material;
};