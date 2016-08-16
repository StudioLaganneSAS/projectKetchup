//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_TIMER_H
#define PK_TIMER_H

#include "PKCritSec.h"
#include "PKTime.h"
#include "PKThread.h"

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

//
// Timer Type enum for:
// periodic or one-shot
//

typedef enum {

    PKTIMER_TYPE_PERIODIC,
    PKTIMER_TYPE_ONESHOT,

} PKTimerType;

//
// PKTimer Class
//

class PKTimer : public IPKRunnable2
{
public:

    PKTimer();
    virtual ~PKTimer();

    // Setup

    void        setTimerType(PKTimerType type);
    PKTimerType getTimerType();

    // Timer fire period (in milliseconds)

    void setTimerDelay(int delay);
    int  getTimerDelay();

    // Start / Stop

    virtual void startTimer();
    virtual void stopTimer();

    virtual bool isTimerRunning();

    // Lock this mutex to run
    // code and be sure that
    // the timer is not running
    // while you do so. Do not
    // forget to unlock!

    void accessMutexWait();
    void accessMutexRelease();

    //
    // Override this to be called
    // when the timer fires
    //

    virtual void timerCallbackFunction() = 0;

    //
    // Please ignore (has to be public but
    // is really internal for Windows/Mac)...
    //

    void realTimerCallbackFunction(unsigned int id);
    virtual void runThread2(PKThread *thread);

protected:

    int   delay;
    bool  running;
    int   timerId;
    int   minRes;

    PKTimerType type;
    PKCritSec   accessMutex;
    bool        forceExit;
    bool        exitOK;

    PKThread *timeThread;

#ifdef MACOSX
    EventLoopTimerRef theTimer;
#endif
};

#endif // PK_TIMER_H
