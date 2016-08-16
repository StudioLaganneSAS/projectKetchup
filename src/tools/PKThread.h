//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_THREAD_H
#define PK_THREAD_H

#include "PKTypes.h"

#include <string>
#include <map>

class PKThread;

typedef enum
{

    PKTHREAD_NORMAL,
    PKTHREAD_ABOVE,
    PKTHREAD_TIMECRITICAL,
    PKTHREAD_BELOW,

} PKThreadPriority;

class IPKRunnable
{
public:

    virtual ~IPKRunnable() {}

    // Override this to be your
    // thread function called to
    // be the thread body...

    virtual void runThread(PKThread *thread) = 0;
};

//
// The IRunnable2 interface is used
// internally when implementing threads, 
// so that classes can inherit both IPKRunnable and
// our own IPKRunnable2 derived classes.
// See for Example PKTimer.h
//

class IPKRunnable2
{
public:

    virtual ~IPKRunnable2() {}
    virtual void runThread2(PKThread *thread) = 0;
};

class PKThread
{
public:

    PKThread();
    PKThread(IPKRunnable  *runnable);
    PKThread(IPKRunnable2 *runnable2);

    virtual ~PKThread();

    // Setup

    void         setRunnableObject(IPKRunnable *runnable);
    IPKRunnable *getRunnableObject();

    void          setRunnableObject2(IPKRunnable2 *runnable2);
    IPKRunnable2 *getRunnableObject2();

    void  setParam(std::string name, void *p);
    void *getParam(std::string name);

    // Start / Stop

    void start();
    void stop();

    // Poll thread state

    bool isRunning();

    // Set priority (only while running)

    void             setPriority(PKThreadPriority priority);
    PKThreadPriority getPriority();

    // Call this from the IRunnable
    // to know when you are asked to
    // exit...

    bool getExitFlag();

    // Static Helpers

    static PKThreadPriority getCurrentThreadPriority();
    static void             setCurrentThreadPriority(PKThreadPriority priority);

    // Be careful with that one, only
    // use as the last resort as the
    // state of the thread is then
    // mostly undefined.

    void forceImmediateStop();

protected:

    IPKRunnable         *runnable;
    IPKRunnable2        *runnable2;
    bool                 forceExit;

    void                          *handle;
    std::map <std::string, void *> params;

#ifdef LINUX
    unsigned long        linHandle;
#endif

#ifdef MACOSX
    unsigned long        macHandle;
#endif

public:
    bool                 running;
};

#endif // PK_THREAD_H
