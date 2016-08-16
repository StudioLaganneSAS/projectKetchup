//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_LOGGER_H
#define PK_LOGGER_H

#include "PKTypes.h"
#include "PKFile.h"

//
// Log LEVEL
//

typedef enum {

	PKLOG_LEVEL_1,
	PKLOG_LEVEL_2,
	PKLOG_LEVEL_3,

} PKLogLevel;

//
// PKLogger
//

class PKLogger {

public:

	PKLogger();
	~PKLogger();

	bool open(std::wstring filename);
	bool close();

	void     setCurrentLevel(PKLogLevel level);
	PKLogLevel getCurrentLevel();

	bool log(std::string  message, 
		     PKLogLevel level = PKLOG_LEVEL_1);

private:

	PKFile    *logFile;
	PKLogLevel currentLevel;
};


#endif // PK_LOGGER_H
