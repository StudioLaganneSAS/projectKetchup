//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SAFE_EVENTS_H
#define PK_SAFE_EVENTS_H

#include "PKTypes.h"
#include "PKCritSec.h"
#include "PKThread.h"

#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

#ifdef LINUX
#include <gtk/gtk.h>
#endif


//
// IPKSafeEventHandler
//

class IPKSafeEventHandler
{
public:
    
    virtual ~IPKSafeEventHandler() {}
	
	//
	// This is the main callback
	//
	
	virtual void processSafeEvent(std::string  eventName,
								  void        *payload) = 0;
};

//
// PKSafeEventRecord
//

class PKSafeEventRecord : public IPKRunnable
{
public:

    // From PKThread
    
    void runThread(PKThread *thread);


    // Members

    PKThread            *thread;
    double               delay;
    IPKSafeEventHandler *handler;
    std::string          eventName;
    void                *payload;
};

//
// PKSafeEvents
//

class PKSafeEvents : public IPKRunnable
{
public:

    static void initialize();

    static void deinitialize();

	static void postSafeEvent(IPKSafeEventHandler *handler,
	                          std::string          eventName,
	                          void                *payload,
                              void                *record = NULL);

    static void postSafeEventWithDelay(IPKSafeEventHandler *handler,
                                       double               delayInSeconds,
                                       std::string          eventName,
            	                       void                *payload);

    static void cancelDelayedEventsWithName(std::string name);

private:

#ifdef WIN32
    
    static HWND hwnd;

	static LRESULT CALLBACK PKSafeEventsWindowProc(HWND hwnd,
								 		           UINT uMsg,
										           WPARAM wParam,
										           LPARAM lParam);

#endif

#ifdef MACOSX
public:
	static EventHandlerRef safeEventHandler;
	static PKCritSec       safeEventMutex;
#endif

#ifdef LINUX
    static gboolean gtk_safe_event_function(gpointer data);
#endif

    static PKCritSec delayedEventMutex;
    static std::vector<PKSafeEventRecord *> delayedEvents;
};

#endif // PK_SAFE_EVENTS_H
