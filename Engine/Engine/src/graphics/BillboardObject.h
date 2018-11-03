#pragma once

#include "interfaces/GameObject.h"
#include "Square.h"

class BillboardObject : public Square
{
	static constexpr const float DefaultWidthHeight = 10.f;
public:
	BillboardObject();
	~BillboardObject();

					void		CreateFromTexture(std::shared_ptr<Texture> texture);

	virtual			void		DrawIndexedInstanced(ICamera * cam, const std::function<void(UINT, UINT, UINT)>&) const override;
	virtual			void		Destroy() override;

	virtual inline	void		RotateX(float, int) override {};
	virtual inline	void		RotateY(float, int) override {};
	virtual inline	void		RotateZ(float, int) override {};


private:
	

};