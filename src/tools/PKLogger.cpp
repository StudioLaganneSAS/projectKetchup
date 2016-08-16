//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKLogger.h"

//
// PKLogger
//

PKLogger::PKLogger()
{
	this->logFile      = NULL;
	this->currentLevel = PKLOG_LEVEL_2;
}

PKLogger::~PKLogger()
{
	if(this->logFile != NULL)
	{
		this->close();
	}
}

void PKLogger::setCurrentLevel(PKLogLevel level)
{
	this->currentLevel = level;
}

PKLogLevel PKLogger::getCurrentLevel()
{
	return this->currentLevel;
}

bool PKLogger::open(std::wstring filename)
{
	bool result = true;

	if(this->logFile != NULL)
	{
		result = this->close();

		if(result != true)
		{
			return result;
		}
	}

	this->logFile = new PKFile();

	PKFileError openResult = this->logFile->open(filename, PKFILE_MODE_WRITE);

	if(openResult != PKFILE_OK)
	{
		delete this->logFile;
		this->logFile = NULL;

		return false;
	}

	return result;
}

bool PKLogger::close()
{
	if(this->logFile != NULL)
	{
		if(this->logFile->close() != PKFILE_OK)
		{
			return false;
		}

		delete this->logFile;
		this->logFile = NULL;
	}

	return true;
}

bool PKLogger::log(std::string message, PKLogLevel level)
{
	if(level <= this->currentLevel && message != "")
	{
		int8_t *buffer = new int8_t[message.length() + 2];
		memcpy(buffer, message.c_str(), message.length());

		int index = 0;

#ifdef WIN32
		buffer[message.length()]     = '\r';
		index++;
#endif
		
		buffer[message.length() + index] = '\n';

		this->logFile->write(buffer, message.length() + 1 + index);
		delete [] buffer;
	}

	return true;
}

