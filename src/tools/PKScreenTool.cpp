//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKScreenTool.h"

// WIN32 stuff

#ifdef WIN32

#ifndef MONITOR_DEFAULTTONEAREST
#define MONITOR_DEFAULTTONEAREST 0x00000002

typedef struct tagMONITORINFO
{
    DWORD cbSize;
    RECT  rcMonitor;
    RECT  rcWork;
    DWORD dwFlags;
}
MONITORINFO, *LPMONITORINFO;

#ifndef HMONITOR_DECLARED
DECLARE_HANDLE(HMONITOR);
#endif

#endif // MONITOR_DEFAULTTONEAREST

#endif // WIN32

// ScreenUtility

PKScreenTool::PKScreenTool()
{
#ifdef WIN32

	this->MonitorFromWindowFunc = NULL;
    this->GetMonitorInfoFunc    = NULL;

	this->hUser32 = LoadLibrary(L"user32.dll");

	if(this->hUser32 == NULL)
	{
		return;
	}

	this->MonitorFromWindowFunc = (pfMonitorFromWindow) GetProcAddress(this->hUser32, "MonitorFromWindow");
    this->GetMonitorInfoFunc    = (pfGetMonitorInfo)    GetProcAddress(this->hUser32, "GetMonitorInfoA");

#endif
}

PKScreenTool::~PKScreenTool()
{
#ifdef WIN32

	if(this->hUser32 != NULL)
	{
		FreeLibrary(hUser32);
	}

#endif
}

// Get the screen dimensions
// for the screen the window
// passed in is on

PKRect PKScreenTool::getScreenForWindow(PKWnd window)
{
	PKRect r;
	memset(&r, 0, sizeof(PKRect));

#ifdef WIN32

	HMONITOR monitor = NULL;
	
	if(this->MonitorFromWindowFunc != NULL)
	{
		monitor = this->MonitorFromWindowFunc(window, MONITOR_DEFAULTTONEAREST);
	}

	if(monitor == NULL)
	{
		return r;
	}

	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);

	if(this->GetMonitorInfoFunc != NULL && 
	   this->GetMonitorInfoFunc(monitor, &info))
	{
		int sw = (info.rcMonitor.right - info.rcMonitor.left);
		int sh = (info.rcMonitor.bottom - info.rcMonitor.top);

		r.x = info.rcMonitor.left;
		r.y = info.rcMonitor.top;
		r.w = sw;
		r.h = sh;

		return r;
	}

#endif

#ifdef MACOSX

    // Get Location of parent window
    
    Rect      windowRect;
    WindowRef parentWindow = GetWindowFromPort(window);
    
    GetWindowPortBounds(parentWindow, &windowRect);
    
    int parentW = windowRect.right - windowRect.left;
    int parentH = windowRect.bottom - windowRect.top;
    
    Point pt;
    
    pt.h = parentW/2;
    pt.v = parentH/2;
    
    CGrafPtr oldPort;
    Rect     clip;
    
    GetPort(&oldPort);    
    SetPort(window);
    
    GetPortBounds(window, &clip);
    SetOrigin(0,0);

    LocalToGlobal(&pt);
    
    SetOrigin(clip.left, clip.top);
    SetPort(oldPort);
    
    // Get screen dimensions 
    
    CGPoint wPt;
    
    wPt.x = pt.h;
    wPt.y = pt.v;
    
    CGDirectDisplayID display;
    CGDisplayCount    maxDisplays  = 1;
    CGDisplayCount    displayCount = 1;
    
    CGGetDisplaysWithPoint(wPt, maxDisplays, &display, &displayCount);

    CGRect dRect = CGDisplayBounds(display);

	r.x = dRect.origin.x;
	r.y = dRect.origin.y;
	r.w = dRect.size.width;
	r.h = dRect.size.height;

#endif

	return r;
}

// Get informations on all the
// screens and their placements

#ifdef WIN32

BOOL CALLBACK PKMonitorEnumProc(HMONITOR hMonitor,
                                HDC hdcMonitor,
                                LPRECT lprcMonitor,
                                LPARAM dwData)
{
    std::vector<PKRect> *screens;
    
    screens = (std::vector <PKRect> *) dwData;
    
    PKRect rect;

    rect.x = lprcMonitor->left;
    rect.y = lprcMonitor->top;
    rect.w = (lprcMonitor->right - lprcMonitor->left);
    rect.h = (lprcMonitor->bottom - lprcMonitor->top);

    (*screens).push_back(rect);

    return TRUE;
}

#endif

std::vector <PKRect> PKScreenTool::getScreens()
{
	std::vector <PKRect> screens;

#ifdef LINUX

    GdkScreen *screen = gdk_screen_get_default();

    if(screen)
    {
        gint numMonitors =  gdk_screen_get_n_monitors(screen);

        for(uint32_t i=0; i<numMonitors; i++)
        {
            PKRect rect;
            GdkRectangle r;

            gdk_screen_get_monitor_geometry(screen, i, &r);

            rect.x = r.x;
            rect.y = r.y;
            rect.w = r.width;
            rect.h = r.height;

            screens.push_back(rect);
        }
    }

#endif

#ifdef WIN32

    EnumDisplayMonitors(NULL, NULL, 
                        PKMonitorEnumProc, 
                       (LPARAM)(LPVOID)&screens);

#endif

#ifdef MACOSX
	
	CGDisplayCount count = 0;
	CGDirectDisplayID ids[10];
	
	CGDisplayErr err = CGGetActiveDisplayList(10, ids, &count);
	
	if(err == kCGErrorSuccess)
	{
		for(uint32_t i=0; i < count; i++)
		{
			CGDirectDisplayID id = ids[i];
			
			PKRect r;
			CGRect dRect = CGDisplayBounds(id);
			
			r.x = dRect.origin.x;
			r.y = dRect.origin.y;
			r.w = dRect.size.width;
			r.h = dRect.size.height;

			screens.push_back(r);
		}
	}
	
	
#endif
	
	return screens;
}

