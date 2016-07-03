#include "Window.h"

bool Window::SetFullScreenSettings(int width, int height)
{
	if (fullscreen)
	{
		DEVMODE dmScreenSettings = {};
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(nullptr, L"The requested fullscreen node is not supported by\nyour video card. Use Windowed mode instead?",
				L"Warning", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = false;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool Window::SetWindowContexts()
{
	GLuint   PixelFormat;
	static PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cDepthBits = 32;

	if (!(hDC = GetDC(hWnd)))
	{
		ReleaseResources();
		ShowErrorMessage(L"Can't create GL device context.", L"ERROR");
		return false;
	}
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
	{
		ReleaseResources();
		ShowErrorMessage(L"Can't find suitable pixel format.", L"ERROR");
		return false;
	}
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		ReleaseResources();
		ShowErrorMessage(L"Can't set pixel format.", L"ERROR");
		return false;
	}
	if (!(hRC = wglCreateContext(hDC)))
	{
		ReleaseResources();
		ShowErrorMessage(L"Can't create GL rendering context.", L"ERROR");
		return false;
	}
	if (!wglMakeCurrent(hDC, hRC))
	{
		ReleaseResources();
		ShowErrorMessage(L"Can't activate GL rendering context.", L"ERROR");
		return false;
	}
	return true;
}

void Window::ReleaseResources()
{
	if (fullscreen)
	{
		ChangeDisplaySettings(nullptr, 0);
		ShowCursor(true);
	}
	if (hRC)
	{
		if (!wglMakeCurrent(nullptr, nullptr))
		{
			ShowErrorMessage(L"Release Of DC And RC failed.", L"SHUTDOWN ERROR");
		}

		if (!wglDeleteContext(hRC))
		{
			ShowErrorMessage(L"Release rendering context failed.", L"SHUTDOWN ERROR");
		}
		hRC = nullptr;
	}
	if (hDC && !ReleaseDC(hWnd, hDC))
	{
		ShowErrorMessage(L"Release device context failed.", L"SHUTDOWN ERROR");
		hDC = nullptr;
	}
	if (hWnd && !DestroyWindow(hWnd))
	{
		ShowErrorMessage(L"Could not release hWnd.", L"SHUTDOWN ERROR");
		hWnd = nullptr;
	}
	if (!UnregisterClass(CLASSNAME, hInstance))
	{
		ShowErrorMessage(L"Could not unregister class.", L"SHUTDOWN ERROR");
		hInstance = nullptr;
	}
}

Window::Window(HINSTANCE instance)
{
	hInstance = instance;
	game = new Game();
	hRC = nullptr;
	hDC = nullptr;
	hWnd = nullptr;
}

LRESULT Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{		
	case WM_ACTIVATE:
		if (!HIWORD(wParam))
			active = true;
		else
			active = false;
		return 0;
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		keys[wParam] = true;
		game->ProcessInput(wParam);
		return 0;
	case WM_KEYUP:
		keys[wParam] = false;
		return 0;
	case WM_SIZE:
		game->ResizeScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::GlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* window;
	if (uMsg == WM_NCCREATE) {
		auto createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)window);
		window->hWnd = hWnd;
	}
	else {
		window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (window != nullptr) {
		return window->WndProc(uMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool Window::CreateGameWindow(int width, int height)
{
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpfnWndProc = &GlobalWndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASSNAME;
	if (!RegisterClass(&wc))
	{
		ShowErrorMessage(L"Failed to register window class.", L"ERROR");
		return false;
	}

	DWORD    dwExStyle;
	DWORD    dwStyle;
	RECT     WindowRect = {};
	WindowRect.right = width;
	WindowRect.bottom = height;

	if (fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);
	if (!CreateWindowEx(
		dwExStyle, CLASSNAME, TITLE,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
		0, 0,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		nullptr, nullptr, hInstance, this))
	{
		ReleaseResources();
		ShowErrorMessage(L"Window creation error.", L"ERROR");
		return false;
	}
	return true;
}

void Window::Draw()
{
	if (keys['R'])	
		game->ResetRotation();	
	else	
		game->Rotate(
			keys['D'] ? 1 : keys['A'] ? -1 : 0,
			keys['W'] ? 1 : keys['S'] ? -1 : 0, 
			keys['Q'] ? 1 : keys['E'] ? -1 : 0);	
	game->Draw();
	SwapBuffers(hDC);
}

void Window::UpdateGame()
{
	game->Update();
}

void Window::ShowErrorMessage(LPCWSTR text, LPCWSTR title)
{
	MessageBox(nullptr, text, title, MB_OK | MB_ICONEXCLAMATION);
}

bool Window::Show(bool fullscreenflag)
{
	long width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	long height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	fullscreen = fullscreenflag;
	if (!fullscreen)
	{
		width /= 2;
		height /= 2;
	}
	if (!SetFullScreenSettings(width, height))
		return false;
	if (!CreateGameWindow(width, height))
		return false;
	if (!SetWindowContexts())
		return false;

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	game->ResizeScene(width, height);
	game->InitGL();
	return true;
}

void Window::Update()
{
	timer.Update();
}

bool Window::ExitRequested()
{
	return keys[VK_ESCAPE];
}

bool Window::SwitchModeIfRequested()
{
	if (keys[VK_F1])
	{
		keys[VK_F1] = false;
		ReleaseResources();
		fullscreen = !fullscreen;
		return Show(fullscreen);
	}
	return true;
}

bool Window::IsActive() const
{
	return active;
}

void Window::RunGame()
{
	game->Initialize();
	timer.SetTimer(0.01, &Window::Draw, this);
	timer.SetTimer(0.666667, &Window::UpdateGame, this);
	timer.Initialize();
}

Window::~Window()
{
	delete game;
}
