//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKBrowserTool.h"
#include "PKStr.h"

#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#endif

//
// openURL
//

void PKBrowserTool::openURL(std::string URL)
{
#ifdef WIN32
    std::wstring wUrl = PKStr::stringToWString(URL);
    ShellExecute(NULL, L"open", wUrl.c_str(), L"", L"", SW_SHOWNORMAL);
#endif

#ifdef MACOSX
	std::string command = "open " + URL;
	system(command.c_str());
#endif

#ifdef LINUX
	std::string command = "gnome-open " + URL;
	system(command.c_str());
#endif
}
