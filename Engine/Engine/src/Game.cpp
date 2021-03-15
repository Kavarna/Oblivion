#include "Game.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "boost/algorithm/string.hpp"

#include "./graphics/Pipelines/ColorPipeline.h"

std::once_flag			Game::m_gameFlag;
std::unique_ptr<Game>	Game::m_gameInstance = nullptr;

Game* Game::GetInstance()
{
	std::call_once(m_gameFlag, [&] { m_gameInstance = std::make_unique<Game>(); });

	return m_gameInstance.get();
}


Game::Game()
{
}


Game::~Game()
{
	Destroy();
}

void Game::Destroy()
{
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	UnregisterClass(ENGINE_NAME, m_windowInstance);
	DestroyWindow(m_windowHandle);
}

void Game::Create(HINSTANCE hInstance, uint32_t width, uint32_t height)
{
	m_windowInstance = hInstance;
	m_windowWidth = width;
	m_windowHeight = height;

	InitWindow();
}

void Game::Create(HWND hWnd)
{
	m_windowHandle = hWnd;
	InitDirect3D();
	InitInput();
	InitImGui();
	InitSettings();
}

void Game::InitSettings()
{
	using boost::property_tree::ptree;
	ptree pt;

	try
	{
		boost::property_tree::json_parser::read_json("Settings.json", pt);
	}
	catch (const boost::property_tree::json_parser_error& e)
	{
		const char error[] = "cannot open file";
		if (strncmp(e.what(), error, strlen(error)))
		{
			DX::OutputVDebugString(L"[LOG]: Couldn't find file Settings.json, creating it.\n");
			WriteSettings();
			boost::property_tree::json_parser::read_json("Settings.json", pt);
		}
	}

	auto renderer = Direct3D11::Get();

	renderer->m_hasMSAA = pt.get_child("Graphics.MSAA").get_value<bool>();
	renderer->m_hasVerticalSync = pt.get_child("Graphics.VSync").get_value<bool>();
	renderer->resetSwapChain();
	renderer->OnResize(m_windowHandle, m_windowWidth, m_windowHeight);

	m_mouseSensivity = pt.get_child("Gameplay.Mouse sensivity").get_value<float>();
	g_isDeveloper = pt.get_child("Gameplay.Developer").get_value<bool>();
	Model::m_advancedCheckMinimum = pt.get_child("Graphics.Model.Advanced check").get_value<int>();

	if (g_isDeveloper)
	{
		auto graphicsDebugDrawer = GraphicsDebugDraw::Get();
		graphicsDebugDrawer->BuildBatchRenderers();
		bool flags;
		flags = pt.get_child("Developer.Draw AABB").get_value<bool>();
		if (flags) graphicsDebugDrawer->SetFlag(DBG_DRAW_BOUNDING_BOX);
		flags = pt.get_child("Developer.Draw Frustums").get_value<bool>();
		if (flags) graphicsDebugDrawer->SetFlag(DBG_DRAW_BOUNDING_FRUSTUM);
		auto physicsDebugDrawer = btDebugDraw::Get();
		int physicsFlags;
		physicsFlags = pt.get_child("Physics.Debug flags").get_value<int>();
		physicsDebugDrawer->setDebugMode(physicsFlags);
		flags = pt.get_child("Physics.Debug enabled").get_value<bool>();
		if (flags) physicsDebugDrawer->EnableDebug();
	}
}

void Game::InitWindow()
{
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = NULL;
	wndClass.hIcon = NULL; // TODO: To be added
	wndClass.hIconSm = NULL;
	wndClass.hInstance = m_windowInstance;
	wndClass.lpszClassName = ENGINE_NAME;
	wndClass.lpfnWndProc = WndProc;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wndClass);

	CreateWindowEx(WS_EX_CLIENTEDGE, ENGINE_NAME, ENGINE_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_windowWidth, m_windowHeight, nullptr, nullptr,
		m_windowInstance, nullptr);

	UpdateWindow(m_windowHandle);
	ShowWindow(m_windowHandle, SW_SHOWNORMAL);
	SetFocus(m_windowHandle);
}

void Game::InitInput()
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(m_windowHandle);
	m_mouse->SetVisible(true);
	m_mouse->SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
}

void Game::InitDirect3D()
{
	Direct3D11 * d3d = Direct3D11::Get();
	d3d->Create(m_windowHandle);
	d3d->OnResize(m_windowHandle, m_windowWidth, m_windowHeight);
	Sun sunLight;
	sunLight.setDirection(0.0f, -1.0f, 0.0f);
	sunLight.setDiffuseColor(1.0f, 1.0f, 1.0f);
	sunLight.setAmbientColor(0.1f, 0.1f, 0.1f);

	BulletWorld::Get()->CreateDefaultWorld();
}

void Game::InitImGui()
{
	Direct3D11 * d3d = Direct3D11::Get();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplDX11_Init(m_windowHandle, d3d->getDevice().Get(), d3d->getContext().Get());

	ImGui::StyleColorsDark();
}

void Game::InitPipelines()
{
	Sun s;
	s.m_direction = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	s.m_diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	s.m_ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	TextureLightPipeline::Get()->setSunLight(s);
	DepthmapPipeline::Get()->EnableDisplacement();
	ShadowMappingPipeline::Get()->EnableDisplacement();
}

void Game::Init2D()
{
	m_batchRenderer = std::make_unique<BatchRenderer>();
	m_batchRenderer->Create();

	g_screenNDC = std::make_unique<ProjectionNDC>();
}

void Game::Init3D()
{
	m_directionalLight = std::make_unique<DirectionalLightView>();
	m_directionalLight->setDiffuseColor(1.0f, 1.0f, 1.0f);
	m_directionalLight->setAmbientColor(0.2f, 0.2f, 0.2f);
	m_directionalLight->setDimensions(1024.f);
	m_directionalLight->setFarZ(300.0f);
	m_directionalLight->setNearZ(10.0f);
	m_directionalLight->setFov(DirectX::XM_PI / 4.f);
	m_directionalLight->setPosition(0.0f, 250.f, 0.0f);
	m_directionalLight->setDirection(0.0f, -1.0f, 0.0f);
	m_directionalLight->build<ProjectionTypes::Perspective>();

	m_shadowMap = std::make_unique<ShadowmapBuild>(m_directionalLight.get(), 50, 4.f);
	m_shadowMap->SetLightView(m_directionalLight.get());
	ShadowMappingPipeline::Get()->setShadowMap(m_shadowMap->GetShadowmapTexture(), m_shadowMap->GetLightView());
}

void Game::InitSizeDependent()
{
	float FOV = DirectX::XM_PI / 4;
	float HByW = (float)m_windowWidth / (float)m_windowHeight;
	float nearZ = GetNearZ();
	float farZ = GetFarZ();
	if (g_camera)
	{
		Camera * cam = new Camera(FOV, HByW, nearZ, farZ);
		//auto camDir = g_camera->GetDirection();
		//cam->SetDirection({ camDir.x,camDir.y,camDir.z,1.0f });
		auto camRotation = g_camera->GetCamRotation();
		cam->SetCamRotation(camRotation);
		auto camPos = g_camera->GetPosition();
		cam->SetPosition({ camPos.x,camPos.y,camPos.z,1.0f });
		g_camera.reset(cam);
		g_camera->Construct();
	}
	else
	{
		g_camera = std::make_unique<Camera>(FOV, HByW, nearZ, farZ);
		g_camera->SetPosition(DirectX::XMVectorSet(0.0f, 2.0f, -3.0f, 1.0f));
		g_screen = std::make_unique<Projection>();
	}
	g_screen->m_width = (float) m_windowWidth;
	g_screen->m_height = (float) m_windowHeight;
	g_screen->m_nearZ = nearZ;
	g_screen->m_farZ = farZ;
	g_screen->Construct();

}

void Game::Run()
{
	InitPipelines();
	Init2D();
	InitSizeDependent();
	Init3D();
	MSG message;
	Update();
	while (true)
	{
		if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				break;
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Update();
			Render();
		}
	}
	WriteSettings();
}

bool leftClickPressed = false;

void Game::Update()
{
	auto renderer = Direct3D11::Get();
	float frameTime = 1.0f / ImGui::GetIO().Framerate;
	if (frameTime > 0.15)
		return;
	auto kb = m_keyboard->GetState();
	auto mouse = m_mouse->GetState();
	float cameraFrameTime = frameTime;
	if (kb.LeftShift)
		cameraFrameTime *= 10;
	if (kb.W)
		g_camera->WalkForward(cameraFrameTime);
	if (kb.S)
		g_camera->WalkBackward(cameraFrameTime);
	if (kb.D)
		g_camera->StrafeRight(cameraFrameTime);
	if (kb.A)
		g_camera->StrafeLeft(cameraFrameTime);

	if (kb.D1 || kb.D2)
	{
		m_selectedDisplay = 0;
	}
	else if (kb.D3)
	{
		m_selectedDisplay = 3;
	}
	else if (kb.D4)
	{
		m_selectedDisplay = 4;
	}
	else if (kb.D5)
	{
		m_selectedDisplay = 5;
	}
	else if (kb.D6)
	{
		m_selectedDisplay = 6;
	}
	else if (kb.D7)
	{
		m_selectedDisplay = 7;
	}
	else if (kb.D8)
	{
		m_selectedDisplay = 8;
	}
	else if (kb.D9)
	{
		m_selectedDisplay = 9;
	}
	else if (kb.D0)
	{
		m_selectedDisplay = 10;
	}

	static bool updatePhysics = true;
	static bool pausePressed = false;
	if (kb.Pause && !pausePressed)
	{
		updatePhysics = !updatePhysics;
		pausePressed = true;
	}
	else if (!kb.Pause)
		pausePressed = false;
	
	if (updatePhysics)
		BulletWorld::Get()->Update(frameTime);

	if (g_isDeveloper)
	{
		static bool bTilde = false;
		if (kb.OemTilde && !bTilde)
		{
			bTilde = true;
			m_showDeveloperConsole = !m_showDeveloperConsole;
		}
		if (!kb.OemTilde)
		{
			bTilde = false;
		}
		if (kb.U)
		{
			renderer->RSWireframeRender();
		}
		if (kb.O)
		{
			renderer->RSSolidRender();
		}

		if (m_selectObjects)
		{
			//static bool bLeftClick = false;
			if (mouse.leftButton && !leftClickPressed)
			{
				leftClickPressed = true;
				if (PickObject())
				{
					m_selectObjects = false;
				}
				else
					m_selectedObject = nullptr;
			}
			else if (!mouse.leftButton)
				leftClickPressed = false;
		}

	}

	if (m_menuActive)
		g_camera->Update(frameTime, 0.0f, 0.0f);
	else
	{
		mouse.x = DX::clamp(mouse.x, -25, 25);
		mouse.y = DX::clamp(mouse.y, -25, 25);
		g_camera->Update(frameTime, mouse.x * m_mouseSensivity, mouse.y * m_mouseSensivity);
	}

	static bool bRightClick = false;
	if (mouse.rightButton && !bRightClick)
	{
		bRightClick = true;
		if (m_menuActive)
		{
			m_mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
			while (ShowCursor(FALSE) > 0);
		}
		else
		{
			m_mouse->SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
			while (ShowCursor(TRUE) <= 0);
		}
		m_menuActive = !m_menuActive;
	}
	else if (!mouse.rightButton)
		bRightClick = false;

	for (auto & model : m_gameObjects)
	{
		model->Update(frameTime);
	}
}

void Game::Begin()
{
	auto renderer = Direct3D11::Get();
	renderer->Begin();
	ImGui_ImplDX11_NewFrame();
}

void Game::End()
{
	auto mouse = m_mouse->GetState();
	auto renderer = Direct3D11::Get();

	bool hasMSAA = renderer->m_hasMSAA;
	if (m_showDeveloperConsole)
	{
		ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Checkbox("Vertical sync", &renderer->m_hasVerticalSync);
		ImGui::Checkbox("Use MSAA", &renderer->m_hasMSAA);
		ImGui::DragFloat("Mouse sensivity", &m_mouseSensivity, 0.01f, 1.0f, 5.0f);

		if (hasMSAA != renderer->m_hasMSAA)
		{
			renderer->resetSwapChain();
			renderer->OnResize(m_windowHandle, m_windowWidth, m_windowHeight);
		}

		ImGui::End();


		ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		ImGui::Text("Application average %.5f s/frame (%.1f FPS)", 1.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Draw calls: %d", g_drawCalls);
		if (m_selectObjects)
			ImGui::Text("Picking object");

		ImGui::End();

		ImGui::Begin("Developer");

		{
			auto graphicsDebugDrawer = GraphicsDebugDraw::Get();
			bool bDrawAABB = graphicsDebugDrawer->RenderBoundingBox();
			ImGui::Checkbox("Graphics Draw Bounding Boxes", &bDrawAABB);
			bool bDrawFrustum = graphicsDebugDrawer->RenderBoundingFrustum();
			ImGui::Checkbox("Graphics Draw Bounding Frustums", &bDrawFrustum);

			if (bDrawAABB != graphicsDebugDrawer->RenderBoundingBox())
			{
				graphicsDebugDrawer->ToggleFlag(DBG_DRAW_BOUNDING_BOX);
			}
			if (bDrawFrustum != graphicsDebugDrawer->RenderBoundingFrustum())
			{
				graphicsDebugDrawer->ToggleFlag(DBG_DRAW_BOUNDING_FRUSTUM);
			}

			ImGui::SliderInt("Model advanced check minimum", (int*)&Model::m_advancedCheckMinimum, 1, 1000);
		}
		ImGui::Separator();
		{
			auto physicsDebugDrawer = PhysicsDebugDraw::Get();
			bool bDrawAABB = physicsDebugDrawer->GetFlagState(btDebugDraw::DBG_DrawAabb);
			ImGui::Checkbox("Physics Draw Bounding Boxes", &bDrawAABB);
			bool bDrawWireframe = physicsDebugDrawer->GetFlagState(btDebugDraw::DBG_DrawWireframe);
			ImGui::Checkbox("Physics Draw wireframe", &bDrawWireframe);
			bool bMaxDraw = physicsDebugDrawer->GetFlagState(btDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
			ImGui::Checkbox("Physics full debug draw", &bMaxDraw);
			
			if (bDrawAABB != physicsDebugDrawer->GetFlagState(btDebugDraw::DBG_DrawAabb))
				physicsDebugDrawer->ToggleFlag(btDebugDraw::DBG_DrawAabb);
			if (bDrawWireframe != physicsDebugDrawer->GetFlagState(btDebugDraw::DBG_DrawWireframe))
				physicsDebugDrawer->ToggleFlag(btDebugDraw::DBG_DrawWireframe);
			if (bMaxDraw != physicsDebugDrawer->GetFlagState(btDebugDraw::DBG_MAX_DEBUG_DRAW_MODE))
				physicsDebugDrawer->ToggleFlag(btDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);

			if (ImGui::Button("Toggle Physics debug drawing"))
			{
				physicsDebugDrawer->ToggleDebug();
			}


			ImGui::Separator();
		}

		if (ImGui::Button("Change materials"))
		{
			m_selectObjects = !m_selectObjects;
			m_selectedObject = nullptr;
		}

		if (ImGui::Button("Close"))
			m_showDeveloperConsole = false;

		ImGui::End();

		if (m_selectedObject)
		{
			m_selectedObject->ImGuiChangeMaterial();
		}
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	renderer->End();
}

void Game::WriteSettings()
{
	boost::property_tree::ptree pt;

	pt.put("Graphics.VSync", Direct3D11::Get()->m_hasVerticalSync);
	pt.put("Graphics.MSAA", Direct3D11::Get()->m_hasMSAA);
	pt.put("Graphics.Model.Advanced check", Model::m_advancedCheckMinimum);

	pt.put("Gameplay.Mouse sensivity", m_mouseSensivity);
	pt.put("Gameplay.Developer", g_isDeveloper);

	auto graphicsDebugDrawer = GraphicsDebugDraw::Get();
	pt.put("Developer.Draw AABB", graphicsDebugDrawer->RenderBoundingBox());
	pt.put("Developer.Draw Frustums", graphicsDebugDrawer->RenderBoundingFrustum());

	auto physicsDebugDrawer = btDebugDraw::Get();
	int flags = physicsDebugDrawer->getDebugMode();
	bool debugEnabled = physicsDebugDrawer->isEnabled();
	pt.put("Physics.Debug flags", flags);
	pt.put("Physics.Debug enabled", debugEnabled);

	boost::property_tree::json_parser::write_json("Settings.json", pt);
}

bool Game::PickObject()
{
	using namespace DirectX;
	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(m_windowHandle, &mouse);

	auto projection = g_camera->GetProjection();

	float pickedXinViewSpace, pickedYinViewSpace, pickedZinViewSpace;
	pickedXinViewSpace = ((2.0f * mouse.x) / m_windowWidth - 1.0f) / XMVectorGetX(projection.r[0]);
	pickedYinViewSpace = -((2.0f * mouse.y) / m_windowHeight - 1.0f) / XMVectorGetY(projection.r[1]);
	pickedZinViewSpace = 1.0f;

	XMVECTOR pickedRayInViewSpace = XMVectorSet(pickedXinViewSpace, pickedYinViewSpace, pickedZinViewSpace, 0.0f);
	pickedRayInViewSpace = XMVector3Normalize(pickedRayInViewSpace);

	XMMATRIX invView;
	XMVECTOR det;
	invView = XMMatrixInverse(&det, g_camera->GetView());

	XMVECTOR pickRayPos = XMVector3TransformCoord(pickedRayInViewSpace, invView);
	XMVECTOR pickRayDir = XMVector3TransformNormal(pickedRayInViewSpace, invView);

	CommonTypes::RayHitInfo min;
	min.dist = FLT_MAX;
	min.instanceID = -1;

	m_selectedObject = nullptr;

	for (auto & model : m_models)
	{
		CommonTypes::RayHitInfo rayHit = model->PickObject(pickRayPos, pickRayDir);
		if (rayHit < min && rayHit.instanceID != -1 && rayHit.dist > 0.0f)
		{
			min = rayHit;
			m_selectedObject = model;
		}
	}
	
	return m_selectedObject != 0;
}

void Game::Render()
{
	auto renderer = Direct3D11::Get();
	if (!renderer->Available())
		return;

	Begin();

	auto graphicsDebugDrawer = GraphicsDebugDraw::Get();
	if (g_isDeveloper)
	{
		g_drawCalls = 0;
		graphicsDebugDrawer->Begin();
		btDebugDraw::Get()->Render();
	}
	m_directionalLight->RenderDebug();

	IGameObject::BindStaticVertexBuffer();

	renderer->SetRenderTargetAndDepth();
	renderer->RSCullNone();

	switch (m_selectedDisplay)
	{
		case 0:
			break;
		case 3:
			Display3();
			break;
		case 4:
			Display4();
			break;
		case 5:
			Display5();
			break;
		case 6:
			Display6();
			break;
		case 7:
			Display7();
			break;
		case 8:
			Display8();
			break;
		case 9:
			Display9();
			break;
		case 10:
			Display10();
			break;
		default:
			break;
	}

	renderer->RSLastState();

	if (g_isDeveloper)
	{
		graphicsDebugDrawer->End(g_camera.get());
	}


	End();
}

void Game::Display3()
{

	static float step = 0.05f;
	static float maxX = 100.f;

	ImGui::Begin("Display 3");
	ImGui::SliderFloat("Step", &step, 0.002f, 0.2f);
	ImGui::SliderFloat("Maximum x", &maxX, 10.f, 1000.f);
	ImGui::End();

	m_batchRenderer->Begin();

	std::vector<DirectX::XMFLOAT3> points;
	points.reserve(size_t(maxX / step + 0.5f) + 1);
	float minY = FLT_MAX, maxY = -FLT_MAX;
	for (float x = 0.0f; x <= maxX; x += step)
	{
		if (x == 0)
		{
			points.emplace_back(0.0f, 1.0f, 0.0f);
		}
		else
		{
			float y = fabs(round(x) - x);
			y /= x;
			points.emplace_back(x, y, 0.0f);
		}

		auto pt = points.back();
		maxY = max(pt.y, maxY);
		minY = min(pt.y, minY);
	}
	maxY = max(fabs(minY), fabs(maxY));

	for (auto& pt : points)
	{
		pt.y /= maxY;
		pt.x /= (maxX * 0.25f);
		m_batchRenderer->Vertex(pt, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::Display4()
{
	static float a = 0.3f;
	static float b = 0.2f;
	static float step = 0.05f;

	ImGui::Begin("Display 4");
	ImGui::SliderFloat("Step", &step, 0.002f, 0.2f);
	ImGui::SliderFloat("Parameter a", &a, 0.02f, 0.5f);
	ImGui::SliderFloat("Parameter b", &b, 0.02f, 0.5f);
	ImGui::End();

	m_batchRenderer->Begin();

	for (float t = -DirectX::XM_PI; t <= DirectX::XM_PI; t += step)
	{
		float x = 2 * (a * cos(t) + b) * cos(t);
		float y = 2 * (a * cos(t) + b) * sin(t);
		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x, y, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::Display5()
{
	static float a = 0.2f;
	static float step = 0.001f;

	ImGui::Begin("Display 5");
	ImGui::SliderFloat("Step", &step, 0.001f, 0.05f);
	ImGui::SliderFloat("Parameter a", &a, 0.02f, 0.5f);
	ImGui::End();

	auto xFunctionEx = [](float t, float a) -> float
	{
		return float(a / (4 * pow(cos(t), 2) - 3));
	};
	auto yFunctionEx = [](float t, float a) -> float
	{
		return float((a * tan(t)) / (4 * pow(cos(t), 2) - 3));
	};
	auto xFunction = std::bind(xFunctionEx, std::placeholders::_1, a);
	auto yFunction = std::bind(yFunctionEx, std::placeholders::_1, a);

	m_batchRenderer->Begin();

	for (float t = -DirectX::XM_PIDIV2 + step; t < -DirectX::XM_PI / 6.f - step; t += step * 2.f)
	{

		float x1, x2, y1, y2;
		x1 = xFunction(t);
		x2 = xFunction(t + step);
		y1 = yFunction(t);
		y2 = yFunction(t + step);

			
		m_batchRenderer->Vertex(DirectX::XMFLOAT3(-1.f, 1.f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x2, y2, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x1, y1, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_batchRenderer->Begin();

	m_batchRenderer->Vertex(DirectX::XMFLOAT3(-1.f, 1.f, 0.1f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	for (float t = -DirectX::XM_PIDIV2 + step; t < -DirectX::XM_PI / 6.f - step; t += step)
	{
		m_batchRenderer->Vertex(DirectX::XMFLOAT3(xFunction(t), yFunction(t), 0.1f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	}
	m_batchRenderer->Vertex(DirectX::XMFLOAT3(-1.f, 1.f, 0.1f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::Display6()
{
	static float a = 0.1f;
	static float b = 0.2f;
	static float step = 0.05f;

	ImGui::Begin("Display 6");
	ImGui::SliderFloat("Step", &step, 0.002f, 0.2f);
	ImGui::SliderFloat("Parameter a", &a, 0.02f, 0.5f);
	ImGui::SliderFloat("Parameter b", &b, 0.02f, 0.5f);
	ImGui::End();

	// Approximated sin(t) = 1 respectively sin(t) = -1 and solved equation:
	// x = -1 -> -1 = a * t - b - 1
	// x = 1 -> 1 = a * t - b + 1
	// Waste power in order to generate a better image
	float minT = (b - 1 - 1) / a;
	float maxT = (b + 1 + 1) / a;

	m_batchRenderer->Begin();
	
	for (float t = minT; t <= maxT; t += step)
	{
		float x = a * t - b * sin(t);
		float y = a - b * cos(t);

		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x, y, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::Display7()
{
	static float r = 0.3f;
	static float R = 0.1f;
	static float step = 0.05f;

	ImGui::Begin("Display 7");
	ImGui::SliderFloat("Step", &step, 0.002f, 0.2f);
	ImGui::SliderFloat("Parameter r", &r, 0.02f, 0.5f);
	ImGui::SliderFloat("Parameter R", &R, 0.02f, 0.5f);
	ImGui::End();

	m_batchRenderer->Begin();

	for (float t = 0.0f; t <= DirectX::XM_2PI; t += step)
	{
		float x = (R + r) * cos((r / R) * t) - r * cos(t + (r / R) * t);
		float y = (R + r) * sin((r / R) * t) - r * sin(t + (r / R) * t);

		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x, y, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::Display8()
{
	static float r = 0.3f;
	static float R = 0.1f;
	static float step = 0.05f;

	ImGui::Begin("Display 8");
	ImGui::SliderFloat("Step", &step, 0.002f, 0.2f);
	ImGui::SliderFloat("Parameter r", &r, 0.02f, 0.5f);
	ImGui::SliderFloat("Parameter R", &R, 0.02f, 0.5f);
	ImGui::End();

	m_batchRenderer->Begin();

	for (float t = 0.0f; t <= DirectX::XM_2PI; t += step)
	{
		float x = (R - r) * cos((r / R) * t) - r * cos(t - (r / R) * t);
		float y = (R - r) * sin((r / R) * t) - r * sin(t - (r / R) * t);

		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x, y, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::Display9()
{
	static float a = 0.4f;
	static float step = 0.000005f;

	ImGui::Begin("Display 9");
	ImGui::SliderFloat("Step", &step, 0.000001f, 0.2f, "%.6f");
	ImGui::SliderFloat("Parameter a", &a, 0.02f, 0.5f);
	ImGui::End();

	m_batchRenderer->Begin();

	for (float t = DirectX::XM_PIDIV4 - step; t > -DirectX::XM_PIDIV4; t -= step)
	{
		 float r = a * sqrt(2 * cos(2 * t));
		 float x = r * cos(t);
		 float y = r * sin(t);

		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x, y, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	for (float t = -DirectX::XM_PIDIV4 + step; t < DirectX::XM_PIDIV4; t += step)
	{
		float r = -a * sqrt(2 * cos(2 * t));
		float x = r * cos(t);
		float y = r * sin(t);

		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x, y, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::Display10()
{
	static float a = 0.02f;
	static float step = 0.05f;
	static float maxValue = 100;

	ImGui::Begin("Display 10");
	ImGui::SliderFloat("Step", &step, 0.000002f, 0.2f, "%.7f");
	ImGui::SliderFloat("Parameter a", &a, 0.0002f, 0.5f, "%.5f");
	ImGui::SliderFloat("Max t value", &maxValue, 10.f, 1000.f);
	ImGui::End();

	m_batchRenderer->Begin();

	for (float t = 0.0f; t <= maxValue; t += step)
	{
		float r = a * exp(1 + t);
		float x = r * cos(t);
		float y = r * sin(t);

		m_batchRenderer->Vertex(DirectX::XMFLOAT3(x, y, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	m_batchRenderer->End<ColorPipeline>(g_screenNDC.get(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void Game::OnSize(uint32_t width, uint32_t height)
{
	if (width < 10 || height < 10)
		return;
	m_windowWidth = width;
	m_windowHeight = height;

	InitSizeDependent();

	Direct3D11::Get()->OnResize(m_windowHandle, m_windowWidth, m_windowHeight);

	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}

void Game::OnMouseMove()
{
	auto mouse = m_mouse->GetState();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_ACTIVATEAPP:
	DirectX::Keyboard::ProcessMessage(Message, wParam, lParam);
	DirectX::Mouse::ProcessMessage(Message, wParam, lParam);


	break;

	case WM_MOUSEMOVE:
	//DirectX::Mouse::ProcessMessage(Message, wParam, lParam);
	//Game::GetInstance()->OnMouseMove();
	//break;
	case WM_INPUT:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
	DirectX::Mouse::ProcessMessage(Message, wParam, lParam);
	break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	DirectX::Keyboard::ProcessMessage(Message, wParam, lParam);
	break;

	case WM_SIZE:
	Game::GetInstance()->OnSize(LOWORD(lParam), HIWORD(lParam));
	break;

	case WM_CREATE:
	Game::GetInstance()->Create(hWnd);
	break;

	case WM_QUIT:
	DestroyWindow(hWnd);
	break;
	case WM_DESTROY:
	PostQuitMessage(0);
	break;
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}