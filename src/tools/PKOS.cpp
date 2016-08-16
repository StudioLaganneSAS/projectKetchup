//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKOS.h"

#ifdef WIN32

#include <windows.h>

WindowsVersion PKOS::getWindowsVersion()
{
    OSVERSIONINFO info;

    memset(&info, 0, sizeof(OSVERSIONINFO));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if(GetVersionEx(&info))
    {
        switch(info.dwMajorVersion)
        {
        case 4: // 95, 98 or ME
            {
                if(info.dwMinorVersion == 0)
                {
                    return WIN_95;
                }
                else if(info.dwMinorVersion == 10)
                {
                    return WIN_98;
                }
                else if(info.dwMinorVersion == 90)
                {
                    return WIN_ME;
                }
            }
            break;

        case 5: // 2000 or XP
            {
                if(info.dwMinorVersion == 0)
                {
                    return WIN_2K;
                }
                else if(info.dwMinorVersion == 1)
                {
                    return WIN_XP;
                }
            }
            break;

        case 6: // VISTA
            {
                return WIN_VISTA;
            }
            break;

        case 7: // WINDOWS SEVEN
            {
                return WIN_7;
            }
            break;
        }
    }

    return WIN_UNKNOWN;
}

bool PKOS::isVistaOrAbove()
{
    WindowsVersion winver = PKOS::getWindowsVersion();
    return ((winver == WIN_VISTA) || (winver == WIN_7));
}

#endif
