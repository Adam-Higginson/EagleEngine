#include "Config.h"

namespace ee
{
	Config::Config()
	{
		this->m_fullScreen = false;
		this->m_vSync = false;
		this->m_4xMsaa = true;
		this->m_showFPS = false;
		this->m_screenWidth = 800;
		this->m_screenHeight = 600;

		//Don't open file
	}

	Config::Config(const std::string &fileName)
		: m_fileName(fileName)
	{
	}

	Config::Config(const Config &other)
	{
	}

	Config::~Config()
	{
	}

	bool Config::Init()
	{

		//Don't do anything if file name is empty!
		if (this->m_fileName.empty())
		{
			return true;
		}

		this->m_configFile.open(this->m_fileName);

		const std::string comment("//");
		if (this->m_configFile.is_open())
		{
			while (!this->m_configFile.eof())
			{
				std::string currentLine;
				std::getline(this->m_configFile, currentLine);

				if (currentLine.find("full_screen") != std::string::npos)
				{
					if (currentLine.find("TRUE") != std::string::npos)
						this->m_fullScreen = true;
					else
						this->m_fullScreen = false;
				}
				else if (currentLine.find("v_sync") != std::string::npos)
				{
					if (currentLine.find("TRUE") != std::string::npos)
						this->m_vSync = true;
					else
						this->m_vSync = false;

				}
				else if (currentLine.find("screen_width") != std::string::npos)
				{
					std::string::size_type foundPos = currentLine.find("screen_width");
					std::string stringToErase("screen_width = ");
					currentLine.erase(currentLine.begin() + foundPos, currentLine.begin() + foundPos + stringToErase.length());

					this->m_screenWidth = atoi(currentLine.c_str());
				}
				else if (currentLine.find("screen_height") != std::string::npos)
				{
					std::string::size_type foundPos = currentLine.find("screen_height");
					std::string stringToErase("screen_height = ");
					currentLine.erase(currentLine.begin() + foundPos, currentLine.begin() + foundPos + stringToErase.length());

					this->m_screenHeight = atoi(currentLine.c_str());
				}
				else if (currentLine.find("4xMSAA") != std::string::npos)
				{
					if (currentLine.find("TRUE") != std::string::npos)
						this->m_4xMsaa = true;
					else
						this->m_4xMsaa = false;
				}
				else if (currentLine.find("show_fps") != std::string::npos)
				{
					if (currentLine.find("TRUE") != std::string::npos)
						this->m_showFPS = true;
					else
						this->m_showFPS = false;
				}
				//Comment found, ignore line
				else if (currentLine.compare(0, comment.size(), comment))
				{
					continue;
				}

				//Otherwise just continue
			}
		}

		else
		{
			//Coudn't open config file, create default!
			std::ofstream defaultConfig("config.txt");

			if (!defaultConfig.is_open())
				return false;

			defaultConfig << "//This is the default config, each line beginning with '//' is ignored" << std::endl;
			defaultConfig << "full_screen = FALSE" << std::endl;
			defaultConfig << "screen_width = 800" << std::endl;
			defaultConfig << "screen_height = 600" << std::endl;
			defaultConfig << "v_sync = FALSE" << std::endl;
			defaultConfig << "4xMSAA = TRUE" << std::endl;

			defaultConfig.close();

			//Set all member variables to default values
			this->m_fullScreen = false;
			this->m_vSync = false;
			this->m_screenWidth = 800;
			this->m_screenHeight = 600;
		}


		//Config file read OK!
		return true;
	}

	void Config::Release()
	{
		if (this->m_configFile.is_open())
		{
			this->m_configFile.close();
		}
	}

	bool Config::GetFullScreen() const
	{
		return this->m_fullScreen;
	}

	bool Config::GetVSync() const
	{
		return this->m_vSync;
	}

	bool Config::Get4xMsaa() const
	{
		return this->m_4xMsaa;
	}

	bool Config::GetShowFPS() const
	{
		return this->m_showFPS;
	}

	int Config::GetScreenWidth() const
	{
		return this->m_screenWidth;
	}

	int Config::GetScreenHeight() const
	{
		return this->m_screenHeight;
	}




}