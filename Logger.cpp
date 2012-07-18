#include "Logger.h"

namespace ee
{
	Logger::Logger()
		: m_fileName("log.log"),
		  m_shouldWriteLevel(true)
	{
		m_currentLevel = LEVEL_INFO;
	}

	Logger::Logger(const std::string &fileName, LoggerLevel warningLevel)
		: m_fileName(fileName),
		  m_shouldWriteLevel(true)
		 
	{
		m_currentLevel = warningLevel;
	}

	Logger::Logger(const Logger &other)
	{
	}

	Logger::~Logger()
	{
	}

	bool Logger::Init()
	{
		m_logFile.open(m_fileName);

		return m_logFile.is_open();
	}

	LoggerLevel Logger::GetCurrentLevel() const
	{
		return m_currentLevel;
	}

	const std::string& Logger::GetFileName() const
	{
		return m_fileName;
	}

	void Logger::SetCurrentLevel(LoggerLevel level)
	{
		m_currentLevel = level;
	}

	void Logger::Log(const std::string &message, LoggerLevel level)
	{
		//We only want to write to file if the level is greater
		if (m_logFile.is_open() && (level != LEVEL_NONE) 
			&& (m_currentLevel != LEVEL_NONE) && (m_currentLevel >= level))
		{
			if (m_shouldWriteLevel)
				WriteLevel(level);
			m_logFile << message << std::endl;
		}
	}

	void Logger::Log(char *message, LoggerLevel level)
	{
		//We only want to write to file if the level is greater
		if (m_logFile.is_open() && (level != LEVEL_NONE) 
			&& (m_currentLevel != LEVEL_NONE) && (m_currentLevel >= level))
		{
			if (m_shouldWriteLevel)
				WriteLevel(level);
			m_logFile << message << std::endl;
		}
	}

	void Logger::CloseLogger()
	{
		if (m_logFile.is_open())
		{
			m_logFile.close();
		}
	}

	void Logger::ShouldWriteLevel(bool shouldWrite)
	{
		m_shouldWriteLevel = shouldWrite;
	}

	void Logger::WriteLevel(LoggerLevel level)
	{
		std::string returnString("");
		switch (level)
		{
			case LEVEL_INFO:
			{
				m_logFile << "Info:";
				break;
			}
			case LEVEL_WARNING:
			{
				m_logFile << "Warning:";
				break;
			}
			case LEVEL_SEVERE:
			{
				m_logFile << "Severe:";
				break;
			}
			default:
				m_logFile << "";
				break;
		}

		//return returnString;
	}
}