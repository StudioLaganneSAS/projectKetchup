//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_WINDOW_H
#define PK_WINDOW_H

//
// This is the base class to create
// cross-platform UI windows 
//

#include "PKUI.h"
#include "PKView.h"
#include "PKMenu.h"
#include "PKMenuItem.h"

#include <string>
#include <vector>

#include "PKCritSec.h"
#include "PKThread.h"

// Private Stuff

#ifdef WIN32

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, 
													  COLORREF crKey, 
													  BYTE bAlpha, 
													  DWORD dwFlags);

typedef BOOL (WINAPI *lpfnUpdateLayeredWindow)(HWND hwnd,
											   HDC hdcDst,
											   POINT *pptDst,
											   SIZE *psize,
											   HDC hdcSrc,
											   POINT *pptSrc,
											   COLORREF crKey,
											   BLENDFUNCTION *pblend,
											   DWORD dwFlags);

typedef BOOL (WINAPI *lpfnIsAppThemed)();

#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

typedef struct {

    int   x;
    int   y;
    int   w;
    int   h;
    void *data;
    
} PKDrawRequest;

// PKWindowStyle

typedef int PKWindowStyle;

#define PK_WINDOW_STYLE_DEFAULT	     (1 <<   0)
#define PK_WINDOW_STYLE_BORDERLESS   (1 <<   1)
#define PK_WINDOW_STYLE_TOPMOST	     (1 <<   2)
#define PK_WINDOW_STYLE_OVER_TOPMOST (1 <<   3)
#define PK_WINDOW_STYLE_NO_TASKBAR   (1 <<   4)
#define PK_WINDOW_STYLE_NO_BKGBRUSH  (1 <<   5)
#define PK_WINDOW_STYLE_LAYERED	     (1 <<   6)
#define PK_WINDOW_STYLE_NO_SHADOW	 (1 <<   7)
#define PK_WINDOW_STYLE_NO_MAX		 (1 <<   8)
#define PK_WINDOW_STYLE_NO_RESIZE    (1 <<   9)
#define PK_WINDOW_STYLE_OVERLAY      (1 <<  10)
#define PK_WINDOW_STYLE_NO_ACTIVATE  (1 <<  11)
#define PK_WINDOW_STYLE_NO_MIN		 (1 <<  12)
#define PK_WINDOW_STYLE_DB_BUFFER    (1 <<  13)
#define PK_WINDOW_STYLE_ACCEPT_DROP  (1 <<  14)
#define PK_WINDOW_STYLE_SHEET		 (1 <<  15)

// PKWindow

class PKWindow 
#ifdef WIN32
: public PKView
#endif
#ifdef MACOSX
: public PKView, IPKRunnable2
#endif
#ifdef LINUX
: public PKView
#endif
{
public:

	static std::string FRAME_RECT_PROPERTY;
	static std::string CAPTION_WSTRING_PROPERTY;
	static std::string SHOWING_BOOL_PROPERTY;
    static std::string ICON_WSTRING_PROPERTY;
    static std::string BACKGROUNDCOLOR_COLOR_PROPERTY;

public:

	PKWindow(std::wstring   id, // must be unique
			 PKWindowStyle  style,
			 PKWindowHandle parent = NULL,
             std::wstring   icon   = L"");

protected:
	virtual ~PKWindow();

public:
    virtual PKWindowHandle getWindowHandle();

#ifdef WIN32
    virtual HBRUSH getBackgroundBrush();
#endif
	
	// Menu

    virtual void setMenu(PKMenu *menu);
	PKMenu *getMenu();

    // Utility

    virtual bool isUsingXPThemes();

	virtual void getGlobalMouse(int32_t *x,
								int32_t *y);

	virtual void getClientMouse(int32_t *x,
								int32_t *y);

    // Move / Resize

	virtual void moveWindowTo(int x, int y);
	virtual void moveClientTo(int x, int y);

    virtual void centerOnScreen();

	virtual int getWindowX();
	virtual int getWindowY();

	virtual int getWindowWidth();
	virtual int getWindowHeight();

	virtual int getClientX();
	virtual int getClientY();

	virtual int getClientWidth();
	virtual int getClientHeight();

	virtual void resizeWindowAreaTo(int w, int h);
	virtual void resizeClientAreaTo(int w, int h);

	virtual void setAlpha(unsigned char alpha);

	// Show / Hide

	virtual void show();
	virtual void hide();
	virtual bool isShowing();

	virtual void raise();

	virtual void hideMouse();
	virtual void showMouse();

    virtual void minimize();
    virtual void maximize();

	// Threading / Messages

	virtual void sendCustomMessage(uint32_t id);
	virtual void receiveCustomMessage(uint32_t id);

    //
    // i18n
    //

    virtual void updateUIStrings();

	//
	// Drawing
	//

    virtual void draw32BppBitmap(unsigned int x,
								 unsigned int y,
								 void        *data,
								 unsigned int w,
								 unsigned int h);

	virtual void redraw(); // triggers a system redraw

	//
	// Reimplement to draw custom
	//

	virtual void draw(PKDrawingDevice device,
					  PKRect rectToPaint);

	// Draw Util

	virtual void fillRect(PKDrawingDevice device,
						  PKRect  rect,
						  PKColor color);

	// Files Drop Callback

	virtual void filesDropped(std::vector<std::wstring> files);

	// Misc Callbacks

    virtual void menuItemAboutToBeShown(PKMenu *fromMenu,
										PKMenuItem *item,
										std::string id);

	virtual void menuItemClicked(PKMenu *fromMenu,
                                 PKMenuItem *item,
                                 std::string id);

	virtual void windowShowing();
	virtual void windowHiding();

	virtual void windowMoved();
	virtual void windowResized();

	virtual void windowMinimized();
	virtual void windowMaximized();

	virtual void windowResizing(PKDirection dir,
				    PKRect     *rect);

	virtual void windowGetMinMax(uint32_t *minWidth,
				     uint32_t *minHeight,
				     uint32_t *maxWidth,
				     uint32_t *maxHeight);

	virtual void windowActivated(bool activated);

	virtual void windowClosed();

	virtual void displayChange();

	//
	// Mouse events
	//
	// Sent by the window when the mouse
	// hovers, or is down/up as well as for
	// double clicks and mouse wheel
	//

	virtual void mouseDown(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod);

	virtual void mouseUp(PKButtonType button,
						 int x,
						 int y,
						 unsigned int mod);

	virtual void mouseDblClick(PKButtonType button,
							   int x,
							   int y,
							   unsigned int mod);

	virtual void mouseMove(int x,
						   int y,
						   unsigned int mod);

	virtual void mouseLeave();

	virtual void mouseWheel(PKButtonType button,
							int x,
							int y,
							unsigned int mod);

	//
	// keyDown() / keyUp()
	//
	// Sent when the window has
	// the focus and a key is 
	// pressed/released.
	//

	virtual void keyDown(unsigned int modifier,
						 PKKey        keyCode,
						 unsigned int virtualCode);

	virtual void keyUp(unsigned int modifier,
					   PKKey        keyCode,
					   unsigned int virtualCode);
					   
	// IRunnable2

#ifdef MACOSX
    void runThread2(PKThread *thread);
#endif

	// PKObject

	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

#ifdef LINUX

	static void gtk_destroy(GtkWidget *widget,
                                gpointer   data);

    static gboolean gtk_expose(GtkWidget      *widget,
                               GdkEventExpose *event,
                               gpointer        user_data);                                

#endif

protected:

    PKMenu *menu;
    bool  hasMenu;

	PKWindowHandle parentWindow;
	
#ifdef WIN32

	HWND  window;
	DWORD wstyle;
	DWORD exStyle;

	HMODULE hUser32;

	lpfnSetLayeredWindowAttributes setLayeredWindowAttributes;
	lpfnUpdateLayeredWindow        updateLayeredWindow;

	HMODULE hUxTheme;
	lpfnIsAppThemed	isAppThemed;

#endif

#ifdef LINUX
	GtkWidget *window;
#endif

#ifdef MACOSX

	WindowRef macwindow;
	CGrafPtr window;

    int lastMouseX;
    int lastMouseY;

    int64_t lastClickTime;
	
	WindowGroupRef wndGroup;
	PKThread mouseThread;

	std::vector <PKDrawRequest *> drawRequests;
	PKCritSec drawMutex;

	EventHandlerRef handler;

	EventHandlerRef dragTrackingHandler;
	EventHandlerRef dragReceiveHandler;

#endif

	std::wstring id;
	bool mouseHidden;
	bool hasChildren;

	PKWindowStyle style;
	
	PKCritSec eventMutex;
	PKCritSec mouseMutex;

private:

#ifdef WIN32

	static LRESULT CALLBACK PKWindowProc(HWND hwnd,
								 		 UINT uMsg,
										 WPARAM wParam,
										 LPARAM lParam);

public:
	void getMouseParams(WPARAM wParam, 
					    LPARAM lParam,
					    int *x,
					    int *y,
					    unsigned int *mod);

#endif

#ifdef MACOSX

	static pascal OSStatus PKWindowEventHandler(EventHandlerCallRef  nextHandler,
												EventRef             inEvent, 
											    void                *userData);
	
	static pascal OSErr PKWindowDragTrackingHandler(DragTrackingMessage trackingMessage,
													WindowRef           windowRef,
													void               *handlerRefCon,
													DragRef             dragRef);

	static pascal OSErr PKWindowDragReceiveHandler(WindowRef windowRef, 
												   void     *handlerRefCon, 
												   DragRef   dragRef);

#endif
};

#endif // PK_WINDOW_H
