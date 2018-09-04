#include "ShadowLight.h"
#include "Game.h"


ShadowLight::ShadowLight()
{
	m_lights.emplace_back();
	m_lights.back().position = DirectX::XMFLOAT2(0.0f, 0.0f);
	m_lights.back().quality = 256.0f;
	m_lights.back().range = 256.0f;
	m_lights.back().proj.m_farZ = Game::GetInstance()->GetFarZ();
	m_lights.back().proj.m_nearZ = Game::GetInstance()->GetNearZ();
	m_lights.back().proj.m_width = m_lights.back().quality;
	m_lights.back().proj.m_height = m_lights.back().quality;
	m_lights.back().proj.Construct();

	m_lights.back().proj.SetOrtho(false);
	m_lights.back().proj.Translate(-400, 0.0f);
	m_lights.back().proj.ConstructTransform();

	for (auto & light : m_lights)
	{
		m_occlusionMaps.emplace_back(new RenderTexture((uint32_t)light.range, (uint32_t)light.range));
	}

	m_heartSprites = std::make_unique<Square>();
	m_heartSprites->Create("Resources/heart.png");

	m_gridSprites = std::make_unique<Square>();
	m_gridSprites->Create("Resources/grid.png");

	m_grid2Sprites = std::make_unique<Square>();
	m_grid2Sprites->Create("Resources/grid2.png");

	m_occluders.push_back(m_heartSprites.get());
	m_occluders.push_back(m_gridSprites.get());
	m_occluders.push_back(m_grid2Sprites.get());

	m_debugSquare = std::make_unique<Square>();
	m_debugSquare->Create();
	m_debugSquare->AddInstance();

	m_debugSquare->SetTexture(m_occlusionMaps[0]->GetOblivionTexture());
}

ShadowLight::~ShadowLight()
{
}

void ShadowLight::Update(float frameTime)
{
	for (uint32_t i = 0; i < m_lights.size(); ++i)
	{
		m_occlusionMaps[i]->SetRenderTarget();
		m_occlusionMaps[i]->ClearTexture(0.0f, 1.0f, 0.0f, 1.0f);

		for (const auto & occluder : m_occluders)
		{
			occluder->Render<TextureShader>(&m_lights[i].proj);
		}

	}
}

void ShadowLight::Render()
{
	Direct3D11::Get()->DepthDisable();
	m_debugSquare->Render<TextureShader>(g_screen.get());

	m_heartSprites->Render<TextureShader>(g_screen.get());
	m_gridSprites->Render<TextureShader>(g_screen.get());
	m_grid2Sprites->Render<TextureShader>(g_screen.get());

	Direct3D11::Get()->DepthEnableLess();

	m_lights.back().proj.RenderDebug();
}

void ShadowLight::OnMouseMove(float x, float y)
{
	m_lights[0].position.x = x;
	m_lights[0].position.y = y;

	m_lights.back().proj.TranslateTo(x, y);
	m_lights.back().proj.ConstructTransform();
}

void ShadowLight::OnSize(float width, float height)
{
	m_heartSprites->ClearInstances();
	m_gridSprites->ClearInstances();
	m_grid2Sprites->ClearInstances();
	float scaleX = width / 1000.0f;
	float scaleY = height / 1000.0f;

	for (uint32_t i = 0; i < m_lights.size(); ++i)
		m_lights[i].proj.SetWindowInfo(width, height);

	m_heartSprites->AddInstance();
	m_heartSprites->SetWindowInfo(width, height);
	m_heartSprites->Scale(50.0f, 50.0f);
	m_heartSprites->TranslateTo(500.f * scaleX, 500.f * scaleY);

	m_gridSprites->AddInstance();
	m_gridSprites->SetWindowInfo(width, height);
	m_gridSprites->Scale(50.f, 50.f);
	m_gridSprites->TranslateTo(300.f * scaleX, 500.f * scaleY);

	m_grid2Sprites->AddInstance();
	m_grid2Sprites->SetWindowInfo(width, height);
	m_grid2Sprites->Scale(50.f, 50.f);
	m_grid2Sprites->TranslateTo(700.f * scaleX, 500.f * scaleY);
	m_grid2Sprites->AddInstance();
	m_grid2Sprites->Identity(1);
	m_grid2Sprites->Scale(50.f, 50.f, 1.f, 1);
	m_grid2Sprites->TranslateTo(0.0f, 0.0f, 1);

	m_debugSquare->SetWindowInfo(width, height);
	m_debugSquare->Identity();
	m_debugSquare->Scale(256.f, 256.f);
	m_debugSquare->TranslateTo(width - 300.0f, height - 300.0f);
}
