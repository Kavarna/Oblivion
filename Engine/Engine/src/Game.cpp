#include "Game.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "boost/algorithm/string.hpp"

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
	Sun sunLight{
		{ 0.0f,-1.0f,0.0f,1.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.1f,0.1f,0.1f,1.0f }
	};

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
	m_32SegoeScriptExtented = std::make_shared<CFont>("Resources/Fonts/32SegoeScriptExtented.fnt");
}

void Game::Init3D()
{
	m_ground = std::make_unique<CollisionObject>();
	m_ground->Create(EDefaultObject::Grid);
	m_ground->AddInstance();
	m_ground->Scale(5.0f, 1.0f, 5.0f);

	m_sphere = std::make_unique<CollisionObject>();
	m_sphere->Create(EDefaultObject::Sphere);

	m_tree = std::make_unique<CollisionObject>();
	m_tree->Create("Resources/LowPolyTree");
	m_tree->AddInstance();
	
	uint32_t instance = m_tree->AddInstance();
	m_tree->Translate(60.0f, 0.0f, 3.0f, instance);
	m_tree->Scale(1.5f, 1.5f, 1.5f, instance);
	m_tree->Deactivate(instance);

	m_cup = std::make_unique<CollisionObject>();
	m_cup->Create("Resources/Cup");
	m_cup->AddInstance(DirectX::XMMatrixTranslation(0.0f, 30.f, 15.0f));
	m_cup->Translate(0.0f, 30.0f, 0.0f, 0);
	m_cup->GlobalScale(3.0f, 3.0f, 3.0f);

	m_billboardTest = std::make_unique<BillboardObject>();
	m_billboardTest->Create("Resources/tree0.dds");
	m_billboardTest->AddInstance();
	m_billboardTest->Scale(100.0f, 100.0f);
	m_billboardTest->Translate(0.0f, 50.0f, 123.f);
	instance = m_billboardTest->AddInstance();
	m_billboardTest->Scale(100.f, 100.f, 1.0f, instance);
	m_billboardTest->Translate(45.0f, 50.0f, 123.f, instance);
	
	//m_sponza = std::make_unique<CollisionObject>();
	//m_sponza->Create("Resources/Sponza");
	//m_sponza->AddInstance();

	m_gameObjects.push_back(m_ground.get());
	m_gameObjects.push_back(m_sphere.get());
	m_gameObjects.push_back(m_tree.get());
	m_gameObjects.push_back(m_cup.get());
	////m_models.push_back(m_sponza.get());

	m_models.push_back(m_ground.get());
	m_models.push_back(m_sphere.get());
	m_models.push_back(m_tree.get());
	m_models.push_back(m_cup.get());

	m_directionalLight = std::make_unique<DirectionalLightView>();
	m_directionalLight->setDiffuseColor(1.0f, 1.0f, 1.0f);
	m_directionalLight->setAmbientColor(0.2f, 0.2f, 0.2f);
	m_directionalLight->setDimensions(2048.f, 2048.f);
	m_directionalLight->setFarZ(300.0f);
	m_directionalLight->setNearZ(10.0f);
	m_directionalLight->setFov(DirectX::XM_PI / 4.f);
	m_directionalLight->setPosition(150.0f, 150.0f, 0.0f);
	m_directionalLight->setDirection(0.0f, -1.0f, 0.0f);
	m_directionalLight->build<ProjectionTypes::Perspective>();

	m_shadowMap = std::make_unique<ShadowmapBuild>(2048, 50, 2.f);
	m_shadowMap->AddGameObject(m_tree.get());
	m_shadowMap->AddGameObject(m_cup.get());
	m_shadowMap->AddGameObject(m_sphere.get());
	m_shadowMap->AddGameObject(m_ground.get());
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

	m_camPosText.reset();
	m_camPosText = std::make_unique<Text>(m_32SegoeScriptExtented, 0.0f, 0.0f);
	m_camPosText->SetWindowInfo((float)m_windowWidth, (float)m_windowHeight);
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

	if (mouse.leftButton && !leftClickPressed)
	{
		leftClickPressed = true;
		if (!m_showDeveloperConsole)
		{
			float scale = DX::randomNumber(0.3f, 3.0f);
			uint32_t instanceID = m_sphere->AddInstance(scale);
			auto camPos = g_camera->GetPosition();
			m_sphere->Translate(camPos.x, camPos.y, camPos.z, instanceID);
			auto camDir = g_camera->GetDirection();
			const float speed = 25.0f;
			m_sphere->Impulse(camDir.x * speed, camDir.y * speed, camDir.z * speed, instanceID);
			m_sphere->Scale(scale, scale, scale, instanceID);
		}
	}
	else if (!mouse.leftButton)
		leftClickPressed = false;

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
	m_shadowMap->Build();

	renderer->SetRenderTargetAndDepth();

	/*for (const auto & model : m_models)
	{
		model->Render<DisplacementShader>(g_camera.get());
	}*/
	ShadowMappingPipeline::Get()->EnableDisplacement();
	m_sphere->Render<ShadowMappingPipeline>(g_camera.get());
	////m_sponza->Render<TextureLightShader>(g_camera.get());
	m_ground->Render<ShadowMappingPipeline>(g_camera.get());
	ShadowMappingPipeline::Get()->DisableDisplacement();
	m_tree->Render<ShadowMappingPipeline>(g_camera.get());
	m_cup->Render<ShadowMappingPipeline>(g_camera.get());

	m_billboardTest->Render<TexturePipeline>(g_camera.get());

	auto camPos = g_camera->GetPosition();
	wchar_t buffer[128];
	swprintf_s(buffer, L"Cam pos: (%.2f, %.2f, %.2f)",
		camPos.x, camPos.y, camPos.z);

	m_camPosText->Render(g_screen.get(), buffer, 0.0f, 64.f,
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	if (g_isDeveloper)
	{
		graphicsDebugDrawer->End(g_camera.get());
	}


	End();
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