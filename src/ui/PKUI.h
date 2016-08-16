//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_UI_H
#define PK_UI_H

//
// UI/UE Specific Types
//

#include <string>

#ifdef WIN32

#include <windows.h>

typedef HWND PKWindowHandle;
typedef HDC  PKDrawingDevice;

#endif

#ifdef MACOSX

#include <Carbon/Carbon.h>

typedef CGrafPtr PKWindowHandle;
typedef CGrafPtr PKDrawingDevice;

#endif

#ifdef LINUX

#include <gtk/gtk.h>
#include <stdlib.h>

typedef GtkWidget * PKWindowHandle;
typedef GtkWidget * PKDrawingDevice;

#endif

// Orientation

#define PK_ORIENTATION_VERTICAL   0
#define PK_ORIENTATION_HORIZONTAL 1

// Sizing enum

typedef enum {

    PK_DIR_NONE,

	PK_DIR_LEFT,
	PK_DIR_RIGHT,
	PK_DIR_TOP,
	PK_DIR_BOTTOM,

	PK_DIR_TOPLEFT,
	PK_DIR_TOPRIGHT,
	PK_DIR_BOTTOMLEFT,
	PK_DIR_BOTTOMRIGHT,
	
	PK_DIR_FORWARD,
	PK_DIR_BACKWARD,

} PKDirection;

//
// Define cross-platform mouse
// buttons types and modifiers
//

typedef enum {

	PK_BUTTON_LEFT,
	PK_BUTTON_RIGHT,
	PK_BUTTON_MIDDLE,
	PK_BUTTON_WHEEL_UP,
	PK_BUTTON_WHEEL_DOWN,

} PKButtonType;

#define PK_MOD_NONE    0x00
#define PK_MOD_CONTROL 0x01
#define PK_MOD_ALT     0x02
#define PK_MOD_SHIFT   0x04
#define PK_MOD_COMMAND 0x08
#define PK_MOD_OPTION  0x10

// Keyboard Keys

typedef enum {

	PK_KEY_UNKNOWN,

	PK_KEY_ESCAPE,
	PK_KEY_ENTER,
	PK_KEY_TAB,
	PK_KEY_BACKSPACE,
	PK_KEY_DELETE,
	PK_KEY_SPACE,

	PK_KEY_LEFT,
	PK_KEY_RIGHT,
	PK_KEY_UP,
	PK_KEY_DOWN,

	PK_KEY_HOME,
	PK_KEY_INSERT,
	PK_KEY_PAGEUP,
	PK_KEY_PAGEDOWN,
	PK_KEY_END,

    PK_KEY_A,
	PK_KEY_B,
	PK_KEY_C,
	PK_KEY_D,
	PK_KEY_E,
	PK_KEY_F,
	PK_KEY_G,
	PK_KEY_H,
	PK_KEY_I,
	PK_KEY_J,
	PK_KEY_K,
	PK_KEY_L,
	PK_KEY_M,
	PK_KEY_N,
	PK_KEY_O,
	PK_KEY_P,
	PK_KEY_Q,
	PK_KEY_R,
	PK_KEY_S,
	PK_KEY_T,
	PK_KEY_U,
	PK_KEY_V,
	PK_KEY_W,
	PK_KEY_X,
	PK_KEY_Y,
	PK_KEY_Z,

	PK_KEY_0,
	PK_KEY_1,
	PK_KEY_2,
	PK_KEY_3,
	PK_KEY_4,
	PK_KEY_5,
	PK_KEY_6,
	PK_KEY_7,
	PK_KEY_8,
	PK_KEY_9,

	PK_KEY_F1,
	PK_KEY_F2,
	PK_KEY_F3,
	PK_KEY_F4,
	PK_KEY_F5,
	PK_KEY_F6,
	PK_KEY_F7,
	PK_KEY_F8,
	PK_KEY_F9,
	PK_KEY_F10,
	PK_KEY_F11,
	PK_KEY_F12,

} PKKey;

//
// Utility functions for
// keyboard handling...
//

#ifdef WIN32

	void PKUIComputeKeyState(WPARAM		   osCode, 
						     unsigned int *modifier,
						     PKKey		  *keyCode,
						     unsigned int *virtualKeyCode);

    void PKUIParseAccelerator(std::string accel,
                              BYTE       *modifier,
                              WORD       *key);

#endif

#ifdef MACOSX

	void PKUIComputeKeyState(EventRecord  *eventRecord, 
							 unsigned int *modifier,
						     PKKey		  *keyCode,
						     unsigned int *virtualKeyCode);

	void PKUIParseAccelerator(std::string accel,
							  UInt8      *modifier,
							  UInt16     *key);

#endif

#ifdef LINUX

	void PKUIParseAccelerator(std::string      accel,
							  GdkModifierType *modifier,
							  guint           *key);

	void PKUIComputeKeyState(GdkEventKey *event, 
							 unsigned int *modifier,
						     PKKey		  *keyCode,
						     unsigned int *virtualKeyCode);

#endif

#endif // PK_UI_H
