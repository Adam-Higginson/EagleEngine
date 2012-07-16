//Filename: config.cpp

#pragma once

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <fstream>
#include <string>

namespace ee
{
	class Config
	{
	public:
		//Creates a config which will read from the specified file name
		Config(const std::string &fileName);
		//Creates a config with the default settings
		Config();
		Config(const Config &other);
		~Config();

		//Inits the config object, by reading input from a file if specified
		bool Init();
		void Release();

		bool	GetFullScreen() const;
		bool	GetVSync() const;
		bool	Get4xMsaa() const;
		bool	GetShowFPS() const;
		int		GetScreenWidth() const;
		int		GetScreenHeight() const;

	private:
		std::ifstream m_configFile;
		const std::string m_fileName;
		//Set config values
		bool m_fullScreen, m_vSync, m_4xMsaa, m_showFPS;
		int m_screenWidth, m_screenHeight;
	};
}

#endif