//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SCREEN_TOOL_H
#define PK_SCREEN_TOOL_H

#include "PKTypes.h"
#include <vector>

#ifdef WIN32
#include <windows.h>
typedef HWND PKWnd;
#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
typedef CGrafPtr PKWnd;
#endif

#ifdef LINUX
#include <gtk/gtk.h>
typedef GtkWidget* PKWnd;
#endif

// WIN32 stuff

#ifdef WIN32

#include <windows.h>

typedef HMONITOR (WINAPI *pfMonitorFromWindow)(HWND hwnd, DWORD dwFlags);
typedef BOOL (WINAPI *pfGetMonitorInfo)(HMONITOR hMonitor, LPMONITORINFO lpmi);

#endif

// PKScreenTool

class PKScreenTool
{
public:

	PKScreenTool();
	~PKScreenTool();

	// Get the screen dimensions
	// for the screen the window
	// passed in is on

	PKRect getScreenForWindow(PKWnd window);

	// Get informations on all the
	// screens and their placements
    // Screen 0 is the main screen
    // and should always be available

	std::vector <PKRect> getScreens();

private:

#ifdef WIN32

	HMODULE hUser32;

	pfMonitorFromWindow MonitorFromWindowFunc;
    pfGetMonitorInfo    GetMonitorInfoFunc;

#endif

};

#endif // PK_SCREEN_TOOL_H
