//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKMutex.h"
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <pthread.h>
#endif
#ifdef LINUX
#include <pthread.h>
#endif

// PKMutex

PKMutex::PKMutex()
{
#ifdef WIN32
    this->mutex = CreateMutex(NULL, FALSE, NULL);
#endif

#if(defined(LINUX) || defined (MACOSX))
    pthread_mutexattr_t   mta;
    this->mutex = (void *) new pthread_mutex_t;
    pthread_mutexattr_init(&mta);
    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init((pthread_mutex_t *)this->mutex, &mta);
#endif

}

PKMutex::~PKMutex()
{
#ifdef WIN32
    CloseHandle(this->mutex);
#endif

#if(defined(LINUX) || defined (MACOSX))
    pthread_mutex_destroy((pthread_mutex_t *) this->mutex);
    delete (pthread_mutex_t *) this->mutex;
#endif
}

void PKMutex::wait()
{
#ifdef WIN32
    WaitForSingleObject(this->mutex, INFINITE);
#endif

#if(defined(LINUX) || defined (MACOSX))
    pthread_mutex_t *linuxMutex = (pthread_mutex_t *)this->mutex;
    pthread_mutex_lock(linuxMutex);
#endif
}

bool PKMutex::tryWait()
{
#ifdef WIN32
    DWORD result = WaitForSingleObject(this->mutex, 1);

	if(result == WAIT_ABANDONED || result == WAIT_TIMEOUT)
	{
		return false;
	}

	return true;
#endif
	
#if(defined(LINUX) || defined (MACOSX))
    pthread_mutex_t *linuxMutex = (pthread_mutex_t *)this->mutex;
	return (pthread_mutex_trylock(linuxMutex) == 0);
#endif
}


void PKMutex::release()
{
#ifdef WIN32
    ReleaseMutex(this->mutex);
#endif

#if(defined(LINUX) || defined (MACOSX))
    pthread_mutex_unlock((pthread_mutex_t *)this->mutex);
#endif
}

// PKMutex auto-lock

PKMutexLock::PKMutexLock(PKMutex *pMutex)
: mutex(pMutex)
{
    this->mutex->wait();
}

PKMutexLock::~PKMutexLock()
{
    release();
}

void PKMutexLock::release()
{
    if (this->mutex != NULL)
    {
        this->mutex->release();
        this->mutex = NULL;
    }
}

