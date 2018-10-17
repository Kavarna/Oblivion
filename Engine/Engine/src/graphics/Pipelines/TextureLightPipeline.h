#pragma once


#include "Pipeline.h"
#include "../Helpers/Lights.h"

class TextureLightPipeline : public IPipeline, public Singletone<TextureLightPipeline>
{
public:

	void setSunLight(const Sun&);

	// Inherited via IPipeline
	virtual void __vectorcall bind(ICamera * cam) const override final;
	virtual void __vectorcall bind(DirectX::FXMMATRIX & world, ICamera * cam) const override final;

private:
	Sun m_light;

};