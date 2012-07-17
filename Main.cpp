//The entry point of the engine, featuring the
//WinMain function

////////////////////////
//INCLUDES
///////////////////////
#include "Window.h"
#include "Logger.h"
#include "Config.h"
#include "EagleEngineUtil.h"
#include <new>
#include <fstream>
#include <string>
#include <xnamath.h>

#define ERROR_BOX(X) MessageBox(NULL, X, L"Error", MB_ICONERROR | MB_OK)

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow)
{
	//Logger object, currently logging everything
	ee::Logger *logger = new ee::Logger("EagleEngine.log", ee::LEVEL_SEVERE);
	if(!logger->Init())
	{
		ERROR_BOX(L"Could not load logger!");
		return -1;
	}

	//Config for creating window
	ee::Config *config = new ee::Config("config.txt");

	if (!config->Init())
	{
		logger->Log("Could not load config file for reading!", ee::LEVEL_WARNING);
	}

	//Write all info to log file
	if (config->GetFullScreen())
		logger->Log("Full Screen = TRUE", ee::LEVEL_INFO);
	else
		logger->Log("Full Screen = FALSE", ee::LEVEL_INFO);

	if (config->GetVSync())
		logger->Log("V Sync = TRUE", ee::LEVEL_INFO);
	else
		logger->Log("V Sync = FALSE", ee::LEVEL_INFO);

	if (config->GetShowFPS())
		logger->Log("Show FPS = TRUE", ee::LEVEL_INFO);
	else
		logger->Log("Show FPS = FALSE", ee::LEVEL_INFO);

	char buffer[10];
	_itoa_s(config->GetScreenWidth(), buffer, 10);
	logger->Log(buffer, ee::LEVEL_INFO);
	_itoa_s(config->GetScreenHeight(), buffer, 10);
	logger->Log(buffer, ee::LEVEL_INFO);

	//Verify XNAMath CPU support
	if (!XMVerifyCPUSupport())
	{
		ERROR_BOX(L"XNA Maths is not supported on this CPU!");
		logger->Log("XNA Maths is not supported on this CPU!", ee::LEVEL_SEVERE);

		return -1;
	}

	//The window the game will run in
	ee::Window *window = new ee::Window(hInstance, L"Eagle Engine", config, logger);;

	if (!window->Init(nCmdShow))
		return -1;

	OutputDebugString(L"Window creation success!\n");
	logger->Log("Window creation success", ee::LEVEL_INFO);

	window->Run();

	//Free window
	window->Release();
	delete window;
	window = NULL;

	//Free config
	config->Release();
	delete config;
	config = NULL;

	logger->CloseLogger();
	delete logger;
	logger = NULL;

	return 0;
}