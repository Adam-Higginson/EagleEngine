#include "Window.h"

namespace ee
{
	Window::Window(HINSTANCE hInstance, LPCWSTR windowTitle, BOOL fullScreen,
						int screenWidth, int screenHeight,
						Logger *logger)
		: m_windowTitle(windowTitle),
		  m_screenWidth(screenWidth),
		  m_screenHeight(screenHeight),
		  m_showFPS(false)
	{
		m_hInstance = hInstance;
		m_logger = logger;
		m_fullScreen = fullScreen;
		m_config = new Config();
		m_isResizing = FALSE;
		m_fullScreen = FALSE;
		m_minimised = FALSE;
	}

	Window::Window(HINSTANCE hInstance, LPCWSTR windowTitle, Config *config, Logger *logger)
		: m_windowTitle(windowTitle),
		m_screenWidth(config->GetScreenWidth()),
		m_screenHeight(config->GetScreenHeight())
	{
		this->m_hInstance = hInstance;
		this->m_logger = logger;
		this->m_fullScreen = config->GetFullScreen();
		this->m_showFPS = config->GetShowFPS();
		this->m_config = config;
		m_isResizing = FALSE;
		m_fullScreen = FALSE;
		m_minimised = FALSE;
		m_isPaused = FALSE;
	}


	Window::Window(const Window &other)
	{
	}

	Window::~Window()
	{
	}

	BOOL Window::Init(int nCmdShow)
	{
		//Create input handler
		m_inputHandler = new InputHandler;
		if (!m_inputHandler->Init())
		{
			m_logger->Log("Could not start input handler!", ee::LEVEL_SEVERE);
			return FALSE;
		}
		//The window class
		WNDCLASSEX wndClassEx;
		//External handle to this window
		windowHandle = this;

		ZeroMemory(&wndClassEx, sizeof(WNDCLASSEX));

		wndClassEx.cbSize = sizeof(WNDCLASSEX);
		wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
		wndClassEx.lpfnWndProc = WndProc;
		wndClassEx.hInstance = m_hInstance;
		wndClassEx.lpszClassName = m_windowTitle;
		wndClassEx.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClassEx.hIcon = LoadIcon(0, IDI_APPLICATION);

		//Register the window class
		if (!RegisterClassEx(&wndClassEx))
		{
			m_logger->Log("Could not register window class!", ee::LEVEL_SEVERE);
			return FALSE;
		}

		RECT wr = {0, 0, m_screenWidth, m_screenHeight};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
		int width = wr.right - wr.left;
		int height = wr.bottom - wr.top;

		m_hWnd = CreateWindowEx(NULL, m_windowTitle, m_windowTitle, 
							  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
							  width, height, 
							  NULL, NULL, m_hInstance, NULL);

		if (!m_hWnd)
		{
			m_logger->Log("Could not create main window!", ee::LEVEL_SEVERE);
			return FALSE;
		}

		m_graphics = new Graphics(m_hWnd, m_config, m_logger);

		if (!m_graphics->Init())
			return FALSE;

		ShowWindow(m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);
		//ShowCursor(FALSE);

		/*RECT size;
		GetWindowRect(m_hWnd, &size);

		m_logger->Log("Found window width:", ee::LEVEL_INFO);
		m_logger->Log(IntegerToString(size.right - size.left), ee::LEVEL_INFO);*/


		return TRUE;
	}

	void Window::Release()
	{
		m_graphics->Release();
		delete m_graphics;
		m_graphics = NULL;

		delete m_inputHandler;
		m_inputHandler = NULL;

		DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		UnregisterClass(m_windowTitle, m_hInstance);

		windowHandle = NULL;
	}

	void Window::Run()
	{
		MSG msg = {0};

		//Reset the timer
		m_timer.Reset();

		bool spaceDown = false;
		bool fDown = false;
		while (msg.message != WM_QUIT)
		{

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				m_timer.Tick();

				if (!m_isPaused)
				{
					if (m_showFPS)
					{
						DrawFrameStats();
					}

					if (m_inputHandler->IsKeyDown(VK_SPACE))
					{
						spaceDown = true;
					}
					if (m_inputHandler->IsKeyUp(VK_SPACE))
					{
						if (spaceDown)
							m_graphics->ToggleWireframe();

						spaceDown = false;
					}

					if (m_inputHandler->IsKeyDown('F'))
					{
						fDown = true;
					}
					if (m_inputHandler->IsKeyUp('F'))
					{
						if (fDown)
							m_graphics->ToggleFlashlight();

						fDown = false;
					}

					//Pause key, toggle paused
					if (m_inputHandler->IsKeyDown(VK_ESCAPE))
					{
						break;
					}

					m_graphics->UpdateScene(m_timer.GetDeltaTime());
					m_graphics->DrawScene(0.f, 0.f, 0.f);

				}
				else
				{
					//Paused
					//OutputDebugString(L"Paused!\n");
					Sleep(100);
				}
			}

		}
	}

	LRESULT CALLBACK Window::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//OutputDebugString((LPCWSTR)IntegerToString(msg).c_str());
		//OutputDebugString(L"\n");
		switch (msg)
		{
			case WM_KEYDOWN:
				m_inputHandler->KeyDown((unsigned int) wParam);
				return 0;
			case WM_KEYUP:
				m_inputHandler->KeyUp((unsigned int) wParam);
				return 0;
			case WM_ACTIVATE:
				if (LOWORD(wParam) == WA_INACTIVE)
				{
					m_isPaused = TRUE;
					m_timer.Stop();
				}
				else
				{
					m_isPaused = FALSE;
					m_timer.Start();
				}

				return 0;
			//When the user grabs the resize bars
			case WM_ENTERSIZEMOVE:
					m_isPaused = TRUE;
					m_isResizing = TRUE;
					m_timer.Stop();

				return 0;
			//When the user releases resize bars
			case WM_EXITSIZEMOVE:
				m_isPaused = FALSE;
				m_isResizing = FALSE;
				m_timer.Start();
				Resize();
				return 0;

			case WM_SIZE:
				m_screenWidth = LOWORD(lParam);
				m_screenHeight = HIWORD(lParam);
				if (m_graphics->IsDevice())
				{
					if (wParam == SIZE_MAXIMIZED)
					{
						m_isPaused = false;
						m_fullScreen = TRUE;
						m_minimised = FALSE;
						Resize();
					}
					else if (wParam == SIZE_MINIMIZED)
					{
						m_isPaused = true;
						m_fullScreen = FALSE;
						m_minimised = TRUE;
					}
					else if (wParam == SIZE_RESTORED)
					{
						if (m_minimised)
						{
							m_isPaused = false;
							m_minimised = false;
							Resize();
						}

						else if (m_fullScreen)
						{
							m_isPaused = false;
							m_fullScreen = false;
							Resize();
						}
					
						else if (m_isResizing)
						{
							//don't do anything
						}

						else
						{
							//MessageBox(0, L"Full screen", 0, 0);
							Resize();
						}
					}
				}
				return 0;
			//This message is sent when a menu is active and the user presses
			//a key that isn't a mnemonic or accelerator key
			case WM_MENUCHAR:
			{
				//When pressing alt-enter, make sure not to beep!
				return MAKELRESULT(0, MNC_CLOSE);
			}
			//Stop the window getting too small
			case WM_GETMINMAXINFO:
			{
				((MINMAXINFO *)lParam)->ptMinTrackSize.x = 200;
				((MINMAXINFO *)lParam)->ptMinTrackSize.y = 200;

				return 0;
			}
			case WM_LBUTTONDOWN:
				m_inputHandler->MouseDown(ee::MOUSE_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				m_graphics->MouseDown(MOUSE_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;

			case WM_MBUTTONDOWN:
				m_inputHandler->MouseDown(ee::MOUSE_MIDDLE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;

			case WM_RBUTTONDOWN:
				m_inputHandler->MouseDown(ee::MOUSE_RIGHT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;

			case WM_LBUTTONUP:
				m_inputHandler->MouseUp(ee::MOUSE_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				m_graphics->MouseUp(MOUSE_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;

			case WM_MBUTTONUP:
				m_inputHandler->MouseUp(ee::MOUSE_MIDDLE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;

			case WM_RBUTTONUP:
				m_inputHandler->MouseUp(ee::MOUSE_RIGHT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;

			case WM_MOUSEMOVE:
				m_inputHandler->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				m_graphics->MouseMoved(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;

			default:
			{
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
		}
	}

	///////////////////
	//Private functions
	//////////////////
	void Window::Resize()
	{
		OutputDebugString(L"In Window::Resize()\n");
		m_graphics->Resize(m_screenWidth, m_screenHeight);
	}

	void Window::DrawFrameStats()
	{
		//Stats currently appended to window caption bar
		static int frameCount = 0;
		static float timeElapsed = 0.0f;

		frameCount++;

		//Find out averages over a 1 second period
		if ((m_timer.GetTotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCount; //fps = frameCount / 1
			float mspf = 1000.0f / fps; //milliseconds per frame

			std::wostringstream outStream;
			outStream.precision(6);
			outStream << m_windowTitle << L"   FPS:" << fps << L"   Frame Time: " << mspf << L" (ms)";
			SetWindowText(m_hWnd, outStream.str().c_str());

			//Reset for next average
			frameCount = 0;
			timeElapsed += 1.0f;
		}
	}

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return windowHandle->MessageHandler(hWnd, msg, wParam, lParam);
		}
	}
}