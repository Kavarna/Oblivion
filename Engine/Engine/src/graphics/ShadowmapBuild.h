#pragma once


#include "interfaces/GraphicsObject.h"
#include "../common/interfaces/Singletone.h"
#include "RenderTexture.h"
#include "interfaces/GameObject.h"

struct DirectionalLightView;

class ShadowmapBuild : public IGraphicsObject
{
public:
	ShadowmapBuild(uint32_t shadowMapsize = 1024, INT depthBias = 10, FLOAT slopeScaledDepthBias = 2.f, FLOAT depthBiasClamp = 0.0f);
	~ShadowmapBuild() = default;

public:
	void Create(uint32_t shadowMapsize = 1024, INT depthBias = 10, FLOAT slopeScaledDepthBias = 2.f, FLOAT depthBiasClamp = 0.0f);
	
	void Build(DirectionalLightView*);

	void AddGameObject(const IGameObject* gameObject);

private:
	std::unique_ptr<RenderTexture>				m_renderTexture;
	MicrosoftPointer<ID3D11RasterizerState>		m_rasterizer;

	std::vector<const IGameObject*>				m_occluders;
};