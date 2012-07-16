//Filename: Logger.h
//A class which allows logging of information.

#pragma once

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <fstream>

namespace ee
{
	enum LoggerLevel {LEVEL_NONE, LEVEL_INFO, LEVEL_WARNING, LEVEL_SEVERE};
	class Logger
	{
	public:
		//Creates a default Logger with logfile log.log and logging level of info
		Logger();
		//Creates a Logger with the specified log file and warning level
		Logger(const std::string &fileName, LoggerLevel warningLevel);
		Logger(const Logger &other);
		~Logger();
		//Use this to open the logger object
		bool Init();

		LoggerLevel GetCurrentLevel() const;
		const std::string& GetFileName() const;

		void SetCurrentLevel(LoggerLevel);

		//Log a message. The message is only logged if the current logging level
		//is higher or equal to the specified logging level
		void Log(const std::string &message, LoggerLevel level);
		void Log(char *message, LoggerLevel level);
		//Close the logger object. The logger should be closed before program exit.
		void CloseLogger();

	private:
		void WriteLevel(LoggerLevel level);

		LoggerLevel m_currentLevel;
		std::ofstream m_logFile;
		std::string m_fileName;
	};
}

#endif