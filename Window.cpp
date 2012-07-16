#include "Window.h"

namespace ee
{
	Window::Window(HINSTANCE hInstance, LPCWSTR windowTitle, BOOL fullScreen,
						int screenWidth, int screenHeight,
						Logger *logger)
		: m_windowTitle(windowTitle),
		  m_screenWidth(screenWidth),
		  m_screenHeight(screenHeight)
	{
		m_hInstance = hInstance;
		m_logger = logger;
		m_fullScreen = fullScreen;
	}

	Window::Window(HINSTANCE hInstance, LPCWSTR windowTitle, Config *config, Logger *logger)
		: m_windowTitle(windowTitle),
		m_screenWidth(config->GetScreenWidth()),
		m_screenHeight(config->GetScreenHeight())
	{
		this->m_hInstance = hInstance;
		this->m_logger = logger;
		this->m_fullScreen = config->GetFullScreen();
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
		wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);

		//Register the window class
		RegisterClassEx(&wndClassEx);

		//RECT wr = {0, 0, m_screenWidth, m_screenHeight};
		//AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		m_hWnd = CreateWindowEx(NULL, m_windowTitle, m_windowTitle, 
							  WS_OVERLAPPEDWINDOW, 300, 300, 
							  m_screenWidth, m_screenHeight, 
							  NULL, NULL, m_hInstance, NULL);

		ShowWindow(m_hWnd, nCmdShow);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
		//ShowCursor(FALSE);

		m_graphics = new Graphics(m_hWnd, m_fullScreen, m_screenWidth, m_screenHeight, m_logger);

		if (!m_graphics->Init())
			return FALSE;

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

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				if (m_inputHandler->isKeyDown(VK_ESCAPE))
					m_isPaused = !m_isPaused;

				m_timer.Tick();

				if (!m_isPaused)
				{
					//TODO Calculate frame stats()
					m_graphics->UpdateScene(m_timer.GetDeltaTime());
					m_graphics->DrawScene(0.0f, 0.0f, 0.0f);

				}
				else
				{
					//Paused
					Sleep(100);
				}
			}
		}
	}

	LRESULT CALLBACK Window::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_KEYDOWN:
			{
				m_inputHandler->keyDown((unsigned int) wParam);
				return 0;
			}
			case WM_KEYUP:
			{
				m_inputHandler->keyDown((unsigned int) wParam);
				return 0;
			}
			default:
			{
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
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