//The entry point of the engine, featuring the
//WinMain function

////////////////////////
//INCLUDES
///////////////////////
#include "Window.h"
#include "Logger.h"
#include "Config.h"
#include <new>
#include <fstream>
#include <string>

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

	char buffer[10];
	itoa(config->GetScreenWidth(), buffer, 10);
	logger->Log(buffer, ee::LEVEL_INFO);
	itoa(config->GetScreenHeight(), buffer, 10);
	logger->Log(buffer, ee::LEVEL_INFO);

	//The window the game will run in
	ee::Window *window;

	try
	{
		window = new ee::Window(hInstance, L"Title", config, logger);
	}
	catch (std::bad_alloc&)
	{
		OutputDebugString(L"Bad alloc failure!");
		logger->Log("Bad alloc for window", ee::LEVEL_SEVERE);
		return -1;
	}

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