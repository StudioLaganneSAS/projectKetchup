//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_PERSIST_H
#define PK_PERSIST_H

#include "PKTypes.h"
#include "PKCritSec.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#endif

#include <string>
#include <vector>

//
//
//

class PKPersistTool
{
public:

    PKPersistTool(std::string appName);
	~PKPersistTool();

	//
	// getters return true if a value was
	// found in the database, or false if
	// no value is currently set...
	//
	// If no value was found, then the pointer
	// passed to receive it is untouched...
	//

	void setInt32(std::string name, int32_t value);
	bool getInt32(std::string name, int32_t *value);	

	void setBool(std::string name, bool value);
	bool getBool(std::string name, bool *value);	

	void setString(std::string name, std::string value);
	bool getString(std::string name, std::string &value);

    void setWString(std::string name, std::wstring value);
	bool getWString(std::string name, std::wstring &value);

	void removeValue(std::string name);

private:

	static PKCritSec mutex;

#ifdef WIN32
	HKEY key;
#endif

#ifdef MACOSX
    CFStringRef appID;
#endif

#ifdef LINUX
    std::string filename;
#endif

};

#endif // PK_PERSIST_H
