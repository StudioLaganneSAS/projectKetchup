//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCursorTools.h"

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

#ifdef WIN32

#include <windows.h>

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

#endif

void PKCursorTools::setCursor(PKCursorID id)
{
#ifdef WIN32

    switch(id)
    {
    case PK_CURSOR_NONE:
        {
            SetCursor(NULL);
        }
        break;

    case PK_CURSOR_ARROW:
        {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
        break;

    case PK_CURSOR_HAND:
        {
            SetCursor(LoadCursor(NULL, IDC_HAND));
        }
        break;
    }

#endif

#ifdef MACOSX

    switch(id)
    {
        case PK_CURSOR_NONE:
        {
            SetThemeCursor(NULL);
        }
        break;

        case PK_CURSOR_ARROW:
        {
            SetThemeCursor(kThemeArrowCursor);
        }
        break;

        case PK_CURSOR_HAND:
        {
            SetThemeCursor(kThemePointingHandCursor);
        }
        break;
    }

#endif
}


void PKCursorTools::showCursor()
{
#ifdef WIN32
    ShowCursor(TRUE);
#endif

#ifdef MACOSX
    ShowCursor();
#endif
}

void PKCursorTools::hideCursor()
{
#ifdef WIN32
    ShowCursor(FALSE);
#endif

#ifdef MACOSX
    HideCursor();
#endif
}
