//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_OS_H
#define PK_OS_H

#include "PKTypes.h"

#ifdef WIN32

typedef enum {

    WIN_UNKNOWN,
    WIN_95,
    WIN_98,
    WIN_ME,
    WIN_2K,
    WIN_XP,
    WIN_VISTA,
	WIN_7,

} WindowsVersion;


#endif

class PKOS
{
public:

#ifdef WIN32
    static WindowsVersion getWindowsVersion();
    static bool isVistaOrAbove();
#endif

};

#endif // PK_OS_H
