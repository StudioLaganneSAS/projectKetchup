//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKCritSec.h"
#include "stdio.h"

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#ifdef MACOSX
#include "CoreServices/CoreServices.h"
#endif

#ifdef LINUX
#include "PKMutex.h"
#endif

//
// PKCritSec::PKCritSec
//

PKCritSec::PKCritSec()
{
#ifdef WIN32
    LPCRITICAL_SECTION cs = new CRITICAL_SECTION();

    InitializeCriticalSection(cs);
    this->critSection = cs;
#endif

#ifdef LINUX
    this->critSection = (void *) new PKMutex();
#endif

#ifdef MACOSX
    MPCriticalRegionID id;
    MPCreateCriticalRegion(&id);
    this->critSection = (void *) id;
#endif
}

//
// PKCritSec::~PKCritSec
//

PKCritSec::~PKCritSec()
{
#ifdef WIN32
    LPCRITICAL_SECTION cs = (CRITICAL_SECTION*) this->critSection;
    DeleteCriticalSection(cs);
    delete cs;
#endif

#ifdef LINUX
    delete (PKMutex*) this->critSection;
#endif

#ifdef MACOSX
    MPDeleteCriticalRegion((MPCriticalRegionID) this->critSection);
#endif
}

//
// PKCritSec::wait
//

void PKCritSec::wait()
{
#ifdef WIN32
    EnterCriticalSection((CRITICAL_SECTION*) this->critSection);
#endif

#ifdef LINUX
    ((PKMutex*) this->critSection)->wait();
#endif

#ifdef MACOSX
    MPEnterCriticalRegion((MPCriticalRegionID) this->critSection, kDurationForever);
#endif
}

//
// PKCritSec::tryWait
//

bool PKCritSec::tryWait()
{
    bool result = false;
#ifdef WIN32
    result = (TryEnterCriticalSection((CRITICAL_SECTION*) this->critSection) == TRUE);
#endif

#ifdef LINUX
    result = ((PKMutex*) this->critSection)->tryWait();
#endif

#ifdef MACOSX
    OSStatus status;
    status = MPEnterCriticalRegion((MPCriticalRegionID) this->critSection, kDurationImmediate);
    if (status == noErr)
        result = true;
#endif

    return result;
}

//
// PKCritSec::release
//

void PKCritSec::release()
{
#ifdef WIN32
    LeaveCriticalSection((CRITICAL_SECTION*) this->critSection);
#endif

#ifdef LINUX
    ((PKMutex*) this->critSection)->release();
#endif

#ifdef MACOSX
    MPExitCriticalRegion((MPCriticalRegionID) this->critSection);
#endif
}

//
// CritLock
//

PKCritLock::PKCritLock(PKCritSec *section)
{
    this->section = section;

    if(this->section != NULL)
    {
        this->section->wait();
    }
}

PKCritLock::~PKCritLock()
{
    if(this->section != NULL)
    {
        this->section->release();
    }
}
