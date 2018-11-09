#pragma once


#include "interfaces/GraphicsObject.h"
#include "../common/interfaces/Singletone.h"
#include "RenderTexture.h"
#include "Helpers/Lights.h"
#include "interfaces/GameObject.h"
#include "Pipelines/ShadowMappingPipeline.h"

class ShadowmapBuild : public IGraphicsObject
{
public:
	ShadowmapBuild(uint32_t shadowMapsize = 1024, INT depthBias = 10, FLOAT slopeScaledDepthBias = 2.f, FLOAT depthBiasClamp = 0.0f);
	~ShadowmapBuild() = default;

public:
	void Create(uint32_t shadowMapsize = 1024, INT depthBias = 10, FLOAT slopeScaledDepthBias = 2.f, FLOAT depthBiasClamp = 0.0f);
	
	void Build();

	void AddGameObject(const IGameObject* gameObject);

	DirectionalLightView* GetLightView() const { return m_lightView; };
	void SetLightView(DirectionalLightView* light) { m_lightView = light; };

	Texture* GetShadowmapTexture() const { return m_renderTexture->GetOblivionDepth().lock().get(); };

private:
	std::unique_ptr<RenderTexture>				m_renderTexture;
	MicrosoftPointer<ID3D11RasterizerState>		m_rasterizer;

	std::vector<const IGameObject*>				m_occluders;
	
	DirectionalLightView*						m_lightView;
};