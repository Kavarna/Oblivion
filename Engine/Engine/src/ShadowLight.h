#pragma once

#include "OblivionInclude.h"

struct SLight
{
	float range;
	float quality;
	DirectX::XMFLOAT2 position;
	Projection proj;
};

class ShadowLight sealed
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

	std::vector<std::unique_ptr<RenderTexture>>		m_occlusionMaps;

	std::unique_ptr<Square>							m_debugSquare;

	std::vector<Square*>							m_occluders;

	std::unique_ptr<Square>							m_heartSprites;
	std::unique_ptr<Square>							m_gridSprites;
	std::unique_ptr<Square>							m_grid2Sprites;
};
