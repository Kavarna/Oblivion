#pragma once


#include "interfaces/GraphicsObject.h"
#include "../common/interfaces/Singletone.h"
#include "RenderTexture.h"


class ShadowmapBuild : public IGraphicsObject, public Singletone<ShadowmapBuild>
{
public:
	

private:
	std::unique_ptr<RenderTexture>		m_renderTexture;

};