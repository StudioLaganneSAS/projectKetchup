//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKTimer.h"

//
// Windows Callback for the timer
//

#ifdef WIN32

#include <windows.h>

#ifndef TIME_KILL_SYNCHRONOUS
#define TIME_KILL_SYNCHRONOUS 0x0100
#endif

void CALLBACK TimerTimeProc(UINT uID, UINT uMsg,
                            DWORD dwUser,
                            DWORD dw1, DWORD dw2)
{
    PKTimer *timer = (PKTimer *) dwUser;

    timer->accessMutexWait();

    //
    // Multimedia callbacks inherit quite a hefty thread priority.
    // we are better off lowering this at each callback in order
    // to allow the user input thread to do it's work. the alternative
    // would be to increase user input thread priority
    //

    PKThreadPriority oldThreadPriority = PKThread::getCurrentThreadPriority();
    PKThread::setCurrentThreadPriority(PKTHREAD_NORMAL);

    timer->realTimerCallbackFunction(uID);

    // Restore thread priority

    PKThread::setCurrentThreadPriority(oldThreadPriority);

    timer->accessMutexRelease();
}

#endif

//
// MACOSX One-shot Callback Function
//

#ifdef MACOSX

pascal void MacTimerAction(EventLoopTimerRef  theTimer,
                           void* userData)
{
    PKTimer *timer = static_cast<PKTimer *>(userData);

    if(timer != NULL)
    {
        timer->timerCallbackFunction();
    }
}

#endif

//
// Fallback periodic thread
//

void PKTimer::runThread2(PKThread *thread)
{
    int64_t baseTime = PKTime::getTime();

    while(!thread->getExitFlag())
    {
        int64_t time = PKTime::getTime();

        double delay = (double) (time - baseTime);
        delay /= PKTime::getTimeScale();
        delay *= 1000;

        if(delay < this->getTimerDelay())
        {
            PKTime::sleep(1);
        }
        else
        {
            baseTime = time;
            this->timerCallbackFunction();
        }
    }
}

/*
 *
 */

PKTimer::PKTimer()
{
    this->running = false;
    this->delay   = 1000;

    this->forceExit = false;
    this->exitOK    = false;

    this->type = PKTIMER_TYPE_PERIODIC;

#ifdef WIN32

    TIMECAPS caps;
    timeGetDevCaps(&caps, sizeof(caps));

    MMRESULT error = timeBeginPeriod(caps.wPeriodMin);
    this->minRes = caps.wPeriodMin;

    this->timerId = 0;

#endif

    this->timeThread = new PKThread(this);

#ifdef MACOSX
    this->theTimer = NULL;
#endif
}

PKTimer::~PKTimer()
{
    PKCritLock lock(&this->accessMutex);

#ifdef WIN32

    if(this->running)
    {
        if(this->type == PKTIMER_TYPE_ONESHOT)
        {
            timeKillEvent(this->timerId);
            this->running = false;
            this->timerId = 0;
        }
        else
        {
            this->stopTimer();
        }
    }

    timeEndPeriod(this->minRes);

#endif

#ifdef MACOSX

	if(this->theTimer != NULL)
	{
        RemoveEventLoopTimer(this->theTimer);
        this->theTimer = NULL;
	}

#endif

    if(this->timeThread->isRunning())
    {
        this->timeThread->stop();
    }

    delete this->timeThread;
}

int PKTimer::getTimerDelay()
{
    return this->delay;
}

void PKTimer::setTimerDelay(int delay)
{
    this->delay = delay;
}

void PKTimer::setTimerType(PKTimerType type)
{
    this->type = type;
}

PKTimerType PKTimer::getTimerType()
{
    return this->type;
}

void PKTimer::startTimer()
{
    PKCritLock lock(&this->accessMutex);

    this->forceExit = false;
    this->exitOK    = false;

    if(this->running)
    {
        return;
    }

#ifdef WIN32

    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&info);

    UINT flags = (this->type == PKTIMER_TYPE_PERIODIC ? TIME_PERIODIC : TIME_ONESHOT);
    flags |= TIME_CALLBACK_FUNCTION;

    if((info.dwMajorVersion >= 5 && info.dwMinorVersion >= 1) || // XP
       (info.dwMajorVersion >= 6)) // Vista
    {
        flags |= TIME_KILL_SYNCHRONOUS;
    }

    this->timerId = (int) timeSetEvent((int)(this->delay),  // uDelay
                                       (int)(this->minRes),// uResolution
                                       (LPTIMECALLBACK) TimerTimeProc,       // lpTimeProc
                                       (DWORD)(LPVOID)this, // dwUser
                                       flags                // fuEvent
                                       );

   if(this->timerId == 0)
   {
       // timeSetEventFailed, probably because we have
       // reached the limit for the number of multimedia
       // timers that the OS can handle. We need to fallback
       // to a thread...

        this->timeThread->start();

        if(this->getTimerDelay() < 50)
        {
            this->timeThread->setPriority(PKTHREAD_TIMECRITICAL);
        }
   }

#endif

#ifdef MACOSX

    if(this->type == PKTIMER_TYPE_PERIODIC)
    {
        this->timeThread->start();

        if(this->getTimerDelay() < 50)
        {
            this->timeThread->setPriority(PKTHREAD_TIMECRITICAL);
        }
    }
    else
    {
        EventLoopRef       mainLoop;
        EventLoopTimerUPP  timerUPP;

        mainLoop = GetMainEventLoop();
        timerUPP = NewEventLoopTimerUPP(MacTimerAction);

        EventTimerInterval start;
        EventTimerInterval interval;

        // ONESHOT

        {
            start    = this->delay*kEventDurationMillisecond;
            interval = 0;
        }

        InstallEventLoopTimer (mainLoop,
                               start, interval,
                               timerUPP,
                               this,
                               &this->theTimer);
    }


#endif

#ifdef LINUX

    if(this->type == PKTIMER_TYPE_PERIODIC)
    {
        this->timeThread->start();

        if(this->getTimerDelay() < 50)
        {
            this->timeThread->setPriority(PKTHREAD_TIMECRITICAL);
        }
    }
    else
    {
        // TODO: LINUX one shot timer
    }


#endif

    this->running = true;
}

void PKTimer::stopTimer()
{
#ifdef WIN32

    if(this->timerId != 0)
    {
        OSVERSIONINFO info;
        info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&info);

        if((info.dwMajorVersion >= 5 && info.dwMinorVersion >= 1) || // XP
           (info.dwMajorVersion >= 6)) // Vista
        {
            timeKillEvent(this->timerId);
        }
        else
        {
            // We can't use TIME_KILL_SYNCHRONOUS because it
            // was introduced with Windows XP only so the only
            // way to properly stop the timer and be sure that
            // the timer function will not get called after we
            // stop it, is to stop it FROM the timer function....

            this->accessMutex.wait();
            this->exitOK    = false;
            this->forceExit = true;
            this->accessMutex.release();

            bool ok = false;

            while(!ok)
            {
                PKTime::sleep(5);

                this->accessMutex.wait();

                if(this->exitOK == true)
                {
                    ok = true;
                }

                this->accessMutex.release();
            }
        }

        // All done!

        this->timerId = 0;
    }
    else
    {
        // TimerId is 0 which means
        // we are using the fallback thread
        // instead of multimedia timers

        this->timeThread->stop();
    }

#endif

#ifdef MACOSX

    if(this->type == PKTIMER_TYPE_PERIODIC)
    {
        this->timeThread->stop();
    }
    else
    {
        RemoveEventLoopTimer(this->theTimer);
        this->theTimer = NULL;
    }

#endif

#ifdef LINUX

    if(this->type == PKTIMER_TYPE_PERIODIC)
    {
        this->timeThread->stop();
    }
    else
    {
        // TODO: linux
    }

#endif

    this->running = false;
}

bool PKTimer::isTimerRunning()
{
    PKCritLock lock(&this->accessMutex);

    return this->running;
}

void PKTimer::accessMutexWait()
{
    this->accessMutex.wait();
}

void PKTimer::accessMutexRelease()
{
    this->accessMutex.release();
}

void PKTimer::realTimerCallbackFunction(unsigned int id)
{
    if(this->forceExit)
    {
#ifdef WIN32
        timeKillEvent(id);
#endif
        this->exitOK = true;
    }
    else
    {
        this->timerCallbackFunction();

        if(this->type == PKTIMER_TYPE_ONESHOT)
        {
            this->running = false;
        }
    }
}
