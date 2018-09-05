#include "ShadowLight.h"
#include "Game.h"


ShadowLight::ShadowLight() :
	IObject()
{
	ID3D11ComputeShader ** CS = &m_buildShadowMapCS;
	ShaderHelper::CreateShaderFromFile(L"Shaders/BuildShadowMapComputeShader.cso", "cs_5_0",
		m_d3d11Device.Get(), &m_buildShadowMapCSBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));

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
		light.occlusionMap = std::make_unique<RenderTexture>((uint32_t)light.range, (uint32_t)light.range);
		light.shadowMap = std::make_shared<Texture>(light.quality, 1.f, true); // light.quality x 1 with UAV
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
	m_debugSquare->SetTexture(m_lights[0].occlusionMap->GetOblivionTexture());

	m_shadowMapDebugSquare = std::make_unique<Square>();
	m_shadowMapDebugSquare->Create();
	m_shadowMapDebugSquare->AddInstance();
	m_shadowMapDebugSquare->SetTexture(m_lights[0].shadowMap);
}

ShadowLight::~ShadowLight()
{
}

void ShadowLight::Update(float frameTime)
{
	static auto renderer = Direct3D11::Get();
	for (auto & light : m_lights)
	{
		light.occlusionMap->SetRenderTarget();
		light.occlusionMap->ClearTexture(0.0f, 0.0f, 0.0f, 0.0f);

		for (const auto & occluder : m_occluders)
		{
			occluder->Render<TextureShader>(&light.proj);
		}
	}
	ID3D11RenderTargetView * nullrtv[] = 
	{
		nullptr
	};
	m_d3d11Context->OMSetRenderTargets(1, nullrtv, nullptr);
	EmptyShader::Get()->bind();
	m_d3d11Context->CSSetShader(m_buildShadowMapCS.Get(), nullptr, 0);
	for (auto & light : m_lights)
	{
		ID3D11ShaderResourceView * res[] =
		{
			light.occlusionMap->GetOblivionTexture()->GetTextureSRV()
		};
		m_d3d11Context->CSSetShaderResources(0, 1, res);
		ID3D11UnorderedAccessView * uav[] = 
		{
			light.shadowMap->GetTextureUAV()
		};
		m_d3d11Context->CSSetSamplers(0, 1, renderer->m_linearClampSampler.GetAddressOf());
		m_d3d11Context->CSSetUnorderedAccessViews(0, 1, uav, nullptr);
		m_d3d11Context->Dispatch((UINT)ceil((float)light.quality / 512.0f), 1, 1);
	}
	ID3D11ShaderResourceView * nullres[] =
	{
		nullptr
	};
	m_d3d11Context->CSSetShaderResources(0, 1, nullres);
	ID3D11UnorderedAccessView * nulluav[] =
	{
		nullptr
	};
	m_d3d11Context->CSSetUnorderedAccessViews(0, 1, nulluav, nullptr);
	EmptyShader::Get()->bind();
}

void ShadowLight::Render()
{
	Direct3D11::Get()->DepthDisable();
	m_debugSquare->Render<TextureShader>(g_screen.get());
	m_shadowMapDebugSquare->Render<TextureShader>(g_screen.get());

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

	m_lights[0].proj.TranslateTo(x, y);
	m_lights[0].proj.ConstructTransform();
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

	m_shadowMapDebugSquare->SetWindowInfo(width, height);
	m_shadowMapDebugSquare->Identity();
	m_shadowMapDebugSquare->Scale(3 * 256.f, 10.f);
	m_shadowMapDebugSquare->TranslateTo(0, height - 30.f);
}
