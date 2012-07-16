#include "Logger.h"

ee::Logger::Logger()
	: m_fileName("log.log")
{
	m_currentLevel = LEVEL_INFO;
}

ee::Logger::Logger(const std::string &fileName, LoggerLevel warningLevel)
	: m_fileName(fileName)
{
	m_currentLevel = warningLevel;
}

ee::Logger::Logger(const Logger &other)
{
}

ee::Logger::~Logger()
{
}

bool ee::Logger::Init()
{
	m_logFile.open(m_fileName);

	return m_logFile.is_open();
}

ee::LoggerLevel ee::Logger::GetCurrentLevel()
{
	return m_currentLevel;
}

std::string& ee::Logger::GetFileName()
{
	return m_fileName;
}

void ee::Logger::SetCurrentLevel(LoggerLevel level)
{
	m_currentLevel = level;
}

void ee::Logger::Log(const std::string &message, LoggerLevel level)
{
	//We only want to write to file if the level is greater
	if (m_logFile.is_open() && (level != LEVEL_NONE) 
		&& (m_currentLevel != LEVEL_NONE) && (m_currentLevel >= level))
	{
		WriteLevel(level);
		m_logFile << " " << message << std::endl;
	}
}

void ee::Logger::Log(char *message, LoggerLevel level)
{
	//We only want to write to file if the level is greater
	if (m_logFile.is_open() && (level != LEVEL_NONE) 
		&& (m_currentLevel != LEVEL_NONE) && (m_currentLevel >= level))
	{
		WriteLevel(level);
		m_logFile << " " << message << std::endl;
	}
}

void ee::Logger::CloseLogger()
{
	if (m_logFile.is_open())
	{
		m_logFile.close();
	}
}

void ee::Logger::WriteLevel(LoggerLevel level)
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