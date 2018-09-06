#pragma once

#include "OblivionInclude.h"

struct SLight
{
	float range;
	float quality;
	DirectX::XMFLOAT2 position;
	Projection proj;
	DirectX::XMFLOAT4 color;
	std::unique_ptr<RenderTexture> occlusionMap;
	std::shared_ptr<Texture> shadowMap;
};

class ShadowLight sealed : public IObject
{
public:
	ShadowLight();
	~ShadowLight();

public:
	void Update(float frameTime);
	void Render();
	void OnMouseMove(float x, float y);
	void OnSize(float width, float height);

private:
	std::vector<SLight>								m_lights;

	std::unique_ptr<Square>							m_debugSquare;
	std::unique_ptr<Square>							m_shadowMapDebugSquare;

	std::vector<Square*>							m_occluders;

	std::unique_ptr<Square>							m_heartSprites;
	std::unique_ptr<Square>							m_gridSprites;
	std::unique_ptr<Square>							m_grid2Sprites;

	MicrosoftPointer<ID3D11ComputeShader>			m_buildShadowMapCS;
	MicrosoftPointer<ID3DBlob>						m_buildShadowMapCSBlob;

	std::unique_ptr<TextureBatchRenderer>			m_batchRenderer;
};
