//Filename: Window.h

#pragma once

#ifndef _WINDOW_H_
#define _WINDOW_H_

////////////////////
//INCLUDES
////////////////////
#include <Windows.h>
#include "Logger.h"
#include "Graphics.h"
#include "InputHandler.h"
#include "Config.h"
#include "Timer.h"

namespace ee
{
	class Window
	{
	public:
		Window(HINSTANCE hInstance, LPCWSTR windowTitle, BOOL fullScreen,
			   int screenWidth, int screenHeight, Logger *logger);
		//Use a config file to create the window instead
		Window(HINSTANCE hInstance, LPCWSTR windowTitle, Config *config, Logger *logger);
		Window(const Window&);
		~Window();

		BOOL Init(int nCmdShow);
		void Release();
		void Run();

		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	private:
		//The title of the window
		LPCWSTR m_windowTitle;
		//The application handle
		HINSTANCE m_hInstance;
		//Screen width and height
		int m_screenWidth, m_screenHeight;
		//Handle to window
		HWND m_hWnd;
		//Whether full screen or not
		BOOL m_fullScreen;
		//The logger object
		Logger *m_logger;
		//The graphics object
		Graphics *m_graphics;
		//Input handler object
		InputHandler *m_inputHandler;
		//Whether the game is paused
		BOOL m_isPaused;
		//Timer object
		Timer m_timer;
	};
}

//External handle to the window
static ee::Window *windowHandle = NULL;
//External WndProc
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif