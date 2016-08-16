//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKThread.h"

#ifdef WIN32
#include <windows.h>
#endif

//
// Thread functions
//

#ifdef WIN32

unsigned long __stdcall PMWIN32ThreadFunc(void *params)
{
    PKThread     *thread    = (PKThread *) params;
    IPKRunnable  *runnable  = thread->getRunnableObject();
    IPKRunnable2 *runnable2 = thread->getRunnableObject2();

    if(runnable == NULL)
    {
        if(runnable2 != NULL)
        {
            runnable2->runThread2(thread);
            return 0;
        }

        return 1;
    }

    runnable->runThread(thread);

    return 0;
}

#endif

#ifdef LINUX

void *LINUXThreadFunc(void *params)
{
    PKThread     *thread    = (PKThread *) params;
    IPKRunnable  *runnable  = thread->getRunnableObject();
    IPKRunnable2 *runnable2 = thread->getRunnableObject2();

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if(runnable == NULL)
    {
        if(runnable2 != NULL)
        {
            runnable2->runThread2(thread);
            return NULL;
        }

        return NULL;
    }

    runnable->runThread(thread);
    thread->running = false;

    return NULL;
}

#endif

#ifdef MACOSX

void *MACOSXThreadFunc(void *params)
{
    PKThread     *thread   = (PKThread *) params;
    IPKRunnable  *runnable = thread->getRunnableObject();
    IPKRunnable2 *runnable2 = thread->getRunnableObject2();

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if(runnable == NULL)
    {
        if(runnable2 != NULL)
        {
            runnable2->runThread2(thread);
            return NULL;
        }

        return NULL;
    }

    runnable->runThread(thread);
    thread->running = false;

    return NULL;
}

#endif

//
// Thread class
//

PKThread::PKThread()
{
    this->handle = NULL;

#ifdef LINUX
    this->linHandle = 0;
#endif
#ifdef MACOSX
    this->macHandle = 0;
#endif

    this->running   = false;
    this->runnable  = NULL;
    this->runnable2 = NULL;
    this->forceExit = false;
}

PKThread::PKThread(IPKRunnable *runnable)
{
    this->handle = NULL;

#ifdef LINUX
    this->linHandle = 0;
#endif
#ifdef MACOSX
    this->macHandle = 0;
#endif

    this->running   = false;
    this->runnable  = runnable;
    this->runnable2 = NULL;
    this->forceExit = false;
}

PKThread::PKThread(IPKRunnable2 *runnable2)
{
    this->handle = NULL;

#ifdef LINUX
    this->linHandle = 0;
#endif
#ifdef MACOSX
    this->macHandle = 0;
#endif

    this->running   = false;
    this->runnable  = NULL;
    this->runnable2 = runnable2;
    this->forceExit = false;
}

PKThread::~PKThread()
{
    // safety net
    this->forceImmediateStop();
}

void PKThread::setRunnableObject(IPKRunnable *run)
{
    this->runnable = run;
}

IPKRunnable *PKThread::getRunnableObject()
{
    return this->runnable;
}

void PKThread::setRunnableObject2(IPKRunnable2 *run)
{
    this->runnable2 = run;
}

IPKRunnable2 *PKThread::getRunnableObject2()
{
    return this->runnable2;
}

void PKThread::setParam(std::string name, void *p)
{
    if(name == "")
    {
        return;
    }

    this->params[name] = p;
}

void *PKThread::getParam(std::string name)
{
    return this->params[name];
}

PKThreadPriority PKThread::getCurrentThreadPriority()
{
#ifdef WIN32

    switch(GetThreadPriority(GetCurrentThread()))
    {
        case THREAD_PRIORITY_NORMAL:
            return PKTHREAD_NORMAL;

        case THREAD_PRIORITY_ABOVE_NORMAL:
            return PKTHREAD_ABOVE;

        case THREAD_PRIORITY_TIME_CRITICAL:
            return PKTHREAD_TIMECRITICAL;

        case THREAD_PRIORITY_BELOW_NORMAL:
            return PKTHREAD_BELOW;

        default:
            return PKTHREAD_NORMAL;
    }

#endif

#if(defined(LINUX) || defined(MACOSX))

	pthread_t current = pthread_self();

    int    pr;
    struct sched_param param;
	
    pr = SCHED_OTHER;
	
    pthread_getschedparam(current, &pr, &param);
    
	if(param.sched_priority >= 95)
	{
		return PKTHREAD_TIMECRITICAL;
	}
	
	if(param.sched_priority >= 75)
	{
		return PKTHREAD_ABOVE;
	}
	
	if(param.sched_priority < 50)
	{
		return PKTHREAD_BELOW;
	}
	
	return PKTHREAD_NORMAL;

#endif
}

void PKThread::setCurrentThreadPriority(PKThreadPriority priority)
{
#ifdef WIN32

    switch(priority)
    {

    case PKTHREAD_NORMAL:
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        break;

    case PKTHREAD_ABOVE:
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
        break;

    case PKTHREAD_TIMECRITICAL:
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
        break;

    case PKTHREAD_BELOW:
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
        break;

    }

#endif

#if(defined(MACOSX) || defined(LINUX))

	pthread_t current = pthread_self();

    int    pr;
    struct sched_param param;
	
    switch(priority)
    {
        case PKTHREAD_NORMAL:
            pr = SCHED_OTHER;
            param.sched_priority = 50;
            break;
			
        case PKTHREAD_ABOVE:
            pr = SCHED_OTHER;
            param.sched_priority = 75;
            break;
			
        case PKTHREAD_TIMECRITICAL:
            pr = SCHED_OTHER;
            param.sched_priority = 97;
            break;
    }
	
    pthread_setschedparam(current, pr, &param);
	
#endif
}

void PKThread::start()
{
#ifdef WIN32

    DWORD threadId;
    this->handle = CreateThread(NULL, 0, PMWIN32ThreadFunc,
                               (LPVOID) this, 0, &threadId);

#endif

#ifdef LINUX

    pthread_create((pthread_t *) &this->linHandle,
                   NULL, LINUXThreadFunc, (void *) this);

    this->running = true;

#endif

#ifdef MACOSX

    pthread_create((pthread_t *) &this->macHandle,
                   NULL, MACOSXThreadFunc, (void *) this);

    this->running = true;

#endif
}

void PKThread::stop()
{
#ifdef WIN32

    if(this->handle != NULL)
    {
        this->forceExit = true;

        while(this->isRunning())
        {
            Sleep(50);
        };

        this->forceExit = false;

        CloseHandle(this->handle);
        this->handle = NULL;
    }

#endif

#ifdef LINUX

    if(this->linHandle != 0)
    {
        this->forceExit = true;
        pthread_join((pthread_t) this->linHandle, NULL);
    }

    this->running   = false;
    this->forceExit = false;

    this->linHandle = 0;

#endif

#ifdef MACOSX

    if(this->macHandle != 0)
    {
        this->forceExit = true;
        pthread_join((pthread_t) this->macHandle, NULL);
    }

    this->running   = false;
    this->forceExit = false;

    this->macHandle = 0;

#endif
}

void PKThread::forceImmediateStop()
{
#ifdef WIN32

    if(this->handle != NULL)
    {
        TerminateThread(this->handle, 0);

        CloseHandle(this->handle);
        this->handle = NULL;
    }

#endif

#ifdef MACOSX

    if(this->macHandle != 0)
    {
        pthread_cancel((pthread_t) this->macHandle);
        this->macHandle = 0;
    }

#endif

#ifdef LINUX

    if(this->linHandle != 0)
    {
        pthread_cancel((pthread_t) this->linHandle);
        this->linHandle = 0;
    }

#endif

    this->running   = false;
}



bool PKThread::isRunning()
{
#ifdef WIN32

    DWORD status = 0;

    if(this->handle == NULL)
    {
        return false;
    }

    GetExitCodeThread(this->handle, &status);

    return (status == STILL_ACTIVE);

#endif

#ifdef LINUX
    return this->running;
#endif

#ifdef MACOSX
    return this->running;
#endif
}

bool PKThread::getExitFlag()
{
    return this->forceExit;
}

void PKThread::setPriority(PKThreadPriority priority)
{
#ifdef WIN32

    if(this->handle == NULL)
    {
        return;
    }

    switch(priority)
    {
    case PKTHREAD_NORMAL:
        SetThreadPriority(this->handle, THREAD_PRIORITY_NORMAL);
        break;

    case PKTHREAD_ABOVE:
        SetThreadPriority(this->handle, THREAD_PRIORITY_ABOVE_NORMAL);
        break;

    case PKTHREAD_TIMECRITICAL:
        SetThreadPriority(this->handle, THREAD_PRIORITY_TIME_CRITICAL);
        break;

    case PKTHREAD_BELOW:
        SetThreadPriority(this->handle, THREAD_PRIORITY_BELOW_NORMAL);
        break;

    }

#endif

#ifdef MACOSX

    if(this->macHandle == 0)
    {
        return;
    }

    int    pr;
    struct sched_param param;

    switch(priority)
    {
        case PKTHREAD_NORMAL:
            pr = SCHED_OTHER;
            param.sched_priority = 50;
            break;

        case PKTHREAD_ABOVE:
            pr = SCHED_OTHER;
            param.sched_priority = 75;
            break;

        case PKTHREAD_TIMECRITICAL:
            pr = SCHED_OTHER;
            param.sched_priority = 97;
            break;
    }

    pthread_setschedparam((pthread_t) this->macHandle, pr, &param);

#endif

#ifdef LINUX

    if(this->linHandle == 0)
    {
        return;
    }

    int    pr;
    struct sched_param param;

    switch(priority)
    {
        case PKTHREAD_NORMAL:
            pr = SCHED_OTHER;
            param.sched_priority = 50;
            break;

        case PKTHREAD_ABOVE:
            pr = SCHED_OTHER;
            param.sched_priority = 75;
            break;

        case PKTHREAD_TIMECRITICAL:
            pr = SCHED_OTHER;
            param.sched_priority = 97;
            break;
    }

    pthread_setschedparam((pthread_t) this->linHandle, pr, &param);

#endif
}

PKThreadPriority PKThread::getPriority()
{
#ifdef WIN32

    if(this->handle == NULL)
    {
        return PKTHREAD_NORMAL;
    }

    switch(GetThreadPriority(this->handle))
    {
        case THREAD_PRIORITY_NORMAL:
            return PKTHREAD_NORMAL;

        case THREAD_PRIORITY_ABOVE_NORMAL:
            return PKTHREAD_ABOVE;

        case THREAD_PRIORITY_TIME_CRITICAL:
            return PKTHREAD_TIMECRITICAL;

        case THREAD_PRIORITY_BELOW_NORMAL:
            return PKTHREAD_BELOW;

        default:
            return PKTHREAD_NORMAL;
    }

#endif

#ifdef LINUX

    if(this->linHandle == 0)
    {
        return PKTHREAD_NORMAL;
    }
	
    int    pr;
    struct sched_param param;
	
    pr = SCHED_OTHER;
	
    pthread_getschedparam((pthread_t) this->linHandle, &pr, &param);
    
	if(param.sched_priority >= 95)
	{
		return PKTHREAD_TIMECRITICAL;
	}
	
	if(param.sched_priority >= 75)
	{
		return PKTHREAD_ABOVE;
	}
	
	if(param.sched_priority < 50)
	{
		return PKTHREAD_BELOW;
	}
	
	return PKTHREAD_NORMAL;
	
	
#endif

#ifdef MACOSX

    if(this->macHandle == 0)
    {
        return PKTHREAD_NORMAL;
    }
	
    int    pr;
    struct sched_param param;
	
    pr = SCHED_OTHER;
	
    pthread_getschedparam((pthread_t) this->macHandle, &pr, &param);
    
	if(param.sched_priority >= 95)
	{
		return PKTHREAD_TIMECRITICAL;
	}

	if(param.sched_priority >= 75)
	{
		return PKTHREAD_ABOVE;
	}
	
	if(param.sched_priority < 50)
	{
		return PKTHREAD_BELOW;
	}
	
	return PKTHREAD_NORMAL;
	
#endif
}
