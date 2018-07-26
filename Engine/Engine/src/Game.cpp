#include "Game.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/ini_parser.hpp"

std::once_flag			Game::m_gameFlag;
std::unique_ptr<Game>	Game::m_gameInstance = nullptr;

#if DEBUG || _DEBUG
#define INCREASE_DEBUG_VAR(var, value) var += value;
#else
#define INCREASE_DEBU_VAR(var, value)
#endif

Game* Game::GetInstance()
{
	std::call_once(m_gameFlag, [&] { m_gameInstance = std::make_unique<Game>(); });

	return m_gameInstance.get();
}


Game::Game()
{}


Game::~Game()
{
	Destroy();
}

void Game::Destroy()
{
	WriteSettings();

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
	InitInput();
	InitDirect3D();
	InitImGui();
	Init2D();
	Init3D();
	InitSizeDependent();
	InitSettings();
	auto shader = DisplacementShader::Get();
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

	auto debugDrawer = DebugDraw::Get();
	debugDrawer->BuildBatchRenderers();
	bool flags;
	flags = pt.get_child("Developer.Draw AABB").get_value<bool>();
	if (flags) debugDrawer->SetFlag(DBG_DRAW_BOUNDING_BOX);
	flags = pt.get_child("Developer.Draw Frustums").get_value<bool>();
	if (flags) debugDrawer->SetFlag(DBG_DRAW_BOUNDING_FRUSTUM);

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

	m_windowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, ENGINE_NAME, ENGINE_NAME,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
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
		{0.5f,-0.5f,0.0f,1.0f},
		{1.0f,1.0f,1.0f,1.0f},
		{0.2f,0.2f,0.2f,1.0f}
	};
	TextureLightShader::Get()->SetLightInformations(sunLight);
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

void Game::Init2D()
{
}

void Game::Init3D()
{
	// auto renderer = Direct3D11::Get();

	m_groundModel = std::make_unique<Model>();
	m_groundModel->Create(EDefaultObject::Grid);
	m_groundModel->AddInstance();
	m_groundModel->Translate(0.0f, -0.01f, 0.0f);
	m_groundModel->SetName("Ground");

	m_treeModel = std::make_unique<Model>();
	m_treeModel->Create("Resources\\tree.obl");
	m_treeModel->AddInstance();
	m_treeModel->RotateY(DirectX::XM_PIDIV4);
	m_treeModel->Translate(-25.0f, 0.0f, 30.0f);
	m_treeModel->AddInstance();
	m_treeModel->RotateY(DirectX::XM_PIDIV2, 1);
	m_treeModel->Translate(25.0f, 0.0f, 30.0f, 1);
	m_treeModel->SetName("Low poly tree");

	m_offRoadCar = std::make_unique<Model>();
	m_offRoadCar->Create("Resources\\OffRoadCar.obl");
	m_offRoadCar->AddInstance();
	m_offRoadCar->Scale(0.3f);
	m_offRoadCar->Translate(0.0f, -0.1f, 0.0f);
	m_offRoadCar->SetName("Offroad car");

	m_models.push_back(m_groundModel.get());
	m_models.push_back(m_treeModel.get());
	m_models.push_back(m_offRoadCar.get());

}

void Game::InitSizeDependent()
{
	float FOV = DirectX::XM_PI / 4;
	float HByW = (float)m_windowWidth / (float)m_windowHeight;
	float nearZ = 1.0f;
	float farZ = 1000.0f;
	if (m_camera)
	{
		Camera * cam = new Camera(FOV, HByW, nearZ, farZ);
		auto camDir = m_camera->GetDirection();
		cam->SetDirection({ camDir.x,camDir.y,camDir.z,1.0f });
		auto camPos = m_camera->GetPosition();
		cam->SetPosition({ camPos.x,camPos.y,camPos.z,1.0f });
		m_camera.reset(cam);
	}
	else
	{
		m_camera = std::make_unique<Camera>(FOV, HByW, nearZ, farZ);
		m_camera->SetPosition(DirectX::XMVectorSet(0.0f, 2.0f, -3.0f, 1.0f));
		m_screen = std::make_unique<Projection>();
	}
	m_screen->m_width = (float) m_windowWidth;
	m_screen->m_height = (float) m_windowHeight;
	m_screen->m_nearZ = nearZ;
	m_screen->m_farZ = farZ;
	m_screen->Construct();
	auto renderer = Direct3D11::Get();
	if (renderer->Available())
	{
#if DEBUG || _DEBUG
		m_debugSquare = std::make_unique<Square>();
		m_debugSquare->Create();
		m_debugSquare->AddInstance();
		m_debugSquare->SetWindowInfo((float)m_windowWidth, (float)m_windowHeight);
		m_debugSquare->Scale(100.0f, 100.0f);
		m_debugSquare->TranslateTo(m_windowWidth - 76.0f, m_windowHeight - 76.0f);
		m_debugSquare->SetName("Debug square");
#endif
	}
}

void Game::Run()
{
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
}

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
		m_camera->WalkForward(cameraFrameTime);
	if (kb.S)
		m_camera->WalkBackward(cameraFrameTime);
	if (kb.D)
		m_camera->StrafeRight(cameraFrameTime);
	if (kb.A)
		m_camera->StrafeLeft(cameraFrameTime);



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
			static bool bLeftClick = false;
			if (mouse.leftButton && !bLeftClick)
			{
				bLeftClick = true;
				if (PickObject())
				{
					m_selectObjects = false;
				}
				else
					m_selectedObject = nullptr;
			}
			else if (!mouse.leftButton)
				bLeftClick = false;
		}

	}

	if (m_menuActive)
		m_camera->Update(frameTime, 0.0f, 0.0f);
	else
		m_camera->Update(frameTime, mouse.x * m_mouseSensivity, mouse.y * m_mouseSensivity);

	static bool bRightClick = false;
	if (mouse.rightButton && !bRightClick)
	{
		bRightClick = true;
		if (m_menuActive)
			m_mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
		else
			m_mouse->SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
		m_menuActive = !m_menuActive;
	}
	else if (!mouse.rightButton)
		bRightClick = false;
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

	if (m_showDeveloperConsole)
	{
		ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		ImGui::Text("Application average %.5f s/frame (%.1f FPS)", 1.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Draw calls: %d", g_drawCalls);
		if (m_selectObjects)
			ImGui::Text("Picking object");

		ImGui::End();

		ImGui::Begin("Developer");

		auto debugDrawer = DebugDraw::Get();
		bool bDrawAABB = debugDrawer->RenderBoundingBox();
		ImGui::Checkbox("Draw Bounding Boxes", &bDrawAABB);
		bool bDrawFrustum = debugDrawer->RenderBoundingFrustum();
		ImGui::Checkbox("Draw Bounding Frustums", &bDrawFrustum);

		if (bDrawAABB != debugDrawer->RenderBoundingBox())
		{
			debugDrawer->ToggleFlag(DBG_DRAW_BOUNDING_BOX);
		}
		if (bDrawFrustum != debugDrawer->RenderBoundingFrustum())
		{
			debugDrawer->ToggleFlag(DBG_DRAW_BOUNDING_FRUSTUM);
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

	pt.put("Gameplay.Mouse sensivity", m_mouseSensivity);
	pt.put("Gameplay.Developer", g_isDeveloper);

	auto debugDrawer = DebugDraw::Get();
	pt.put("Developer.Draw AABB", debugDrawer->RenderBoundingBox());
	pt.put("Developer.Draw Frustums", debugDrawer->RenderBoundingFrustum());

	boost::property_tree::json_parser::write_json("Settings.json", pt);
}

bool Game::PickObject()
{
	using namespace DirectX;
	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(m_windowHandle, &mouse);

	auto projection = m_camera->GetProjection();

	float pickedXinViewSpace, pickedYinViewSpace, pickedZinViewSpace;
	pickedXinViewSpace = ((2.0f * mouse.x) / m_windowWidth - 1.0f) / XMVectorGetX(projection.r[0]);
	pickedYinViewSpace = -((2.0f * mouse.y) / m_windowHeight - 1.0f) / XMVectorGetY(projection.r[1]);
	pickedZinViewSpace = 1.0f;

	XMVECTOR pickedRayInViewSpace = XMVectorSet(pickedXinViewSpace, pickedYinViewSpace, pickedZinViewSpace, 0.0f);
	pickedRayInViewSpace = XMVector3Normalize(pickedRayInViewSpace);

	XMMATRIX invView;
	XMVECTOR det;
	invView = XMMatrixInverse(&det, m_camera->GetView());

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

	auto debugDrawer = DebugDraw::Get();
	if (g_isDeveloper)
	{
		g_drawCalls = 0;
		debugDrawer->Begin();
	}

	IGameObject::BindStaticVertexBuffer();
	
#if DEBUG || _DEBUG
	m_debugSquare->Render(m_screen.get(), Pipeline::Texture);
#endif

	TextureLightShader::Get()->bind();

	m_groundModel->Render(m_camera.get(), Pipeline::DisplacementTextureLight);
	m_treeModel->Render(m_camera.get(), Pipeline::TextureLight);
	m_offRoadCar->Render(m_camera.get(), Pipeline::TextureLight);

	EmptyShader::Get()->bind(); // Clear the pipeline

	if (g_isDeveloper)
	{
		debugDrawer->End(m_camera.get());
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

	case WM_INPUT:
	case WM_MOUSEMOVE:
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

	case WM_QUIT:
	DestroyWindow(hWnd);
	break;
	case WM_DESTROY:
	PostQuitMessage(0);
	break;
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}