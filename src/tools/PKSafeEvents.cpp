//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKSafeEvents.h"
#include "PKTime.h"

// Event Struct

typedef struct {

	IPKSafeEventHandler *handler;
	std::string          eventName;
	void                *payload;
    void                *record;

} PKSafeEventContext;

// Delayed events members

PKCritSec                        PKSafeEvents::delayedEventMutex;
std::vector<PKSafeEventRecord *> PKSafeEvents::delayedEvents;

// WIN32 stuff

#ifdef WIN32

// Our window (HWND)

HWND PKSafeEvents::hwnd = NULL;

// Safe Event Message

#define WM_PK_SAFE_EVENT (WM_USER + 100)

// Window Proc

LRESULT CALLBACK PKSafeEvents::PKSafeEventsWindowProc(HWND hwnd,
			 		                                  UINT uMsg,
					                                  WPARAM wParam,
					                                  LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PK_SAFE_EVENT:
		{
            PKSafeEventContext *context = (PKSafeEventContext *) lParam;

            if(context != NULL)
            {
                if(context->handler != NULL)
                {
                    context->handler->processSafeEvent(context->eventName, 
                                                       context->payload);
                }

                if(context->record != NULL)
                {
                    PKSafeEventRecord *rec = (PKSafeEventRecord *) context->record;

                    PKSafeEvents::delayedEventMutex.wait();
    
                    std::vector<PKSafeEventRecord *> newRecords;

                    for(uint32_t i=0; i<PKSafeEvents::delayedEvents.size(); i++)
                    {
                        PKSafeEventRecord *thisRec = PKSafeEvents::delayedEvents[i];

                        if(thisRec != rec)
                        {
                            newRecords.push_back(thisRec);
                        }
                    }

                    PKSafeEvents::delayedEvents = newRecords;

                    delete rec->thread;
                    delete rec;

                    PKSafeEvents::delayedEventMutex.release();
                }

                delete context;
            }
		}
		break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#endif

//
// MACOSX stuff
//

#ifdef MACOSX

#define safeEventClass 'PKSA' 
#define safeEventKind  'PKSK' 
#define safeParamName  'PKSP'
#define safeParamType  'void'

EventHandlerRef	PKSafeEvents::safeEventHandler = NULL;

PKCritSec PKSafeEvents::safeEventMutex;

OSStatus SafeEventHandlerProc(EventHandlerCallRef inHandlerCallRef,
							  EventRef inEvent,
							  void * inUserData)
{
    UInt32 eventClass = GetEventClass(inEvent);
    UInt32 eventKind  = GetEventKind(inEvent);
	
    switch(eventClass)
    {
			// Custom Event
			
		case safeEventClass:
		{
			if(eventKind == safeEventKind)
			{
				OSErr err;
				
				void *data;
				
				err = GetEventParameter(inEvent,
										safeParamName,
										safeParamType,
										NULL,
										sizeof(void *),
										NULL,
										(void*)&data);	
				
				if(err != noErr)
				{
					return eventNotHandledErr;
				}
				
				PKSafeEventContext *pevent = (PKSafeEventContext *) data;
				
				if(pevent != NULL)
				{
					PKSafeEvents::safeEventMutex.wait();
					
					if(pevent->handler != NULL)
					{
						pevent->handler->processSafeEvent(pevent->eventName, pevent->payload);
					}					

                    if(pevent->record != NULL)
                    {
                        PKSafeEventRecord *rec = (PKSafeEventRecord *) pevent->record;

                        PKSafeEvents::delayedEventMutex.wait();
        
                        std::vector<PKSafeEventRecord *> newRecords;

                        for(uint32_t i=0; i<PKSafeEvents::delayedEvents.size(); i++)
                        {
                            PKSafeEventRecord *thisRec = PKSafeEvents::delayedEvents[i];

                            if(thisRec != rec)
                            {
                                newRecords.push_back(thisRec);
                            }
                        }

                        PKSafeEvents::delayedEvents = newRecords;

                        delete rec->thread;
                        delete rec;

                        PKSafeEvents::delayedEventMutex.release();
                    }

                    delete pevent;
					
					PKSafeEvents::safeEventMutex.release();
					
					return noErr;
				}
			}
		}
	}
	
	return eventNotHandledErr;
}

#endif

//
// PKSafeEvents
//

void PKSafeEvents::initialize()
{
#ifdef WIN32

    if(PKSafeEvents::hwnd != NULL)
    {
        // Already inited
        return;
    }

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    // Create our window

	WNDCLASSEX wclass;

	wclass.cbSize	     = sizeof(WNDCLASSEX);
	wclass.style	     = 0;
    wclass.lpfnWndProc   = PKSafeEvents::PKSafeEventsWindowProc;
	wclass.cbClsExtra    = 0;
	wclass.cbWndExtra    = 0;
	wclass.hInstance     = hInst;
	wclass.hIcon         = NULL;
	wclass.hCursor       = NULL;
	wclass.hbrBackground = NULL;
	wclass.lpszMenuName  = NULL;
	wclass.hIconSm       = NULL;
	
	std::wstring className = L"PKSafeEvents_HWND";
	wclass.lpszClassName = className.c_str(); 

	RegisterClassEx(&wclass);

	RECT client;

	client.left   = 0;
	client.top    = 0;
	client.right  = 200;
	client.bottom = 100;

    PKSafeEvents::hwnd = CreateWindowEx(0,
								        className.c_str(),
								        L"",
								        WS_OVERLAPPEDWINDOW, 
								        client.left, client.top,
								        client.right - client.left,
								        client.bottom - client.top,
								        NULL, NULL, hInst, NULL);

#endif

#ifdef MACOSX

	if(PKSafeEvents::safeEventHandler != NULL)
	{
		// Already inited
		return;
	}
	
	EventTypeSpec safeEventSpec = {safeEventClass, safeEventKind};
	
	PKSafeEvents::safeEventMutex.wait();
	
	InstallApplicationEventHandler(NewEventHandlerUPP(SafeEventHandlerProc),
								   1,
								   &safeEventSpec,
								   NULL,
								   &PKSafeEvents::safeEventHandler);
	
	PKSafeEvents::safeEventMutex.release();
	
#endif
}

void PKSafeEvents::deinitialize()
{
#ifdef WIN32
	
	if(PKSafeEvents::hwnd == NULL)
	{
		return;
	}
	
	DestroyWindow(PKSafeEvents::hwnd);
	
#endif

#ifdef MACOSX
	
	if(PKSafeEvents::safeEventHandler == NULL)
	{
		return;
	}
	
	PKSafeEvents::safeEventMutex.wait();
	
	// Remove the event handler
	// so that it ain't called 
	// after this point on
	
	RemoveEventHandler(PKSafeEvents::safeEventHandler);
	PKSafeEvents::safeEventHandler = NULL;
	
	PKSafeEvents::safeEventMutex.release();
	
#endif
}

void PKSafeEvents::postSafeEvent(IPKSafeEventHandler *handler,
                                 std::string          eventName,
                                 void                *payload,
                                 void                *record)
{
#ifdef WIN32

    if(PKSafeEvents::hwnd != NULL)
    {
        PKSafeEventContext *context = new PKSafeEventContext;

        context->handler   = handler;
        context->eventName = eventName;
        context->payload   = payload;
        context->record    = record;

        PostMessage(PKSafeEvents::hwnd, 
                    WM_PK_SAFE_EVENT, 
                    NULL, (LPARAM) context);
    }

#endif

#ifdef LINUX

    PKSafeEventContext *context = new PKSafeEventContext;

    if(context == NULL)
    {
        return;
    }

    context->handler   = handler;
    context->eventName = eventName;
    context->payload   = payload;
    context->record    = record;

    gtk_timeout_add(0, PKSafeEvents::gtk_safe_event_function, context);

#endif

#ifdef MACOSX

	PKSafeEventContext *pevent = new PKSafeEventContext;
	
	if(pevent == NULL)
	{
		return;
	}
	
	pevent->handler   = handler;
	pevent->eventName = eventName;
	pevent->payload   = payload;
    pevent->record    = record;
	
	OSErr    err;
	EventRef myEvent;
	
	err = CreateEvent(kCFAllocatorDefault,
					  safeEventClass, 
					  safeEventKind,
					  0,
					  kEventAttributeNone,
					  &myEvent);
	
	if(err != noErr)
	{
		delete pevent;
		return;
	}
	
	err = SetEventParameter(myEvent,
							safeParamName,
							safeParamType,
							sizeof(void *),
							(void*) &pevent);	
	
	if(err != noErr)
	{
		delete pevent;
		return;
	}
	
	PostEventToQueue(GetMainEventQueue(), myEvent, kEventPriorityStandard);	
	
#endif
}

#ifdef LINUX

gboolean PKSafeEvents::gtk_safe_event_function(gpointer data)
{
    PKSafeEventContext *pevent = (PKSafeEventContext *) data;

    if(pevent == NULL)
    {
        return FALSE;
    }

    if(pevent->handler == NULL)
    {
        return FALSE;
    }

    pevent->handler->processSafeEvent(pevent->eventName, pevent->payload);

    if(pevent->record != NULL)
    {
        PKSafeEventRecord *rec = (PKSafeEventRecord *) pevent->record;

        PKSafeEvents::delayedEventMutex.wait();

        std::vector<PKSafeEventRecord *> newRecords;

        for(uint32_t i=0; i<PKSafeEvents::delayedEvents.size(); i++)
        {
            PKSafeEventRecord *thisRec = PKSafeEvents::delayedEvents[i];

            if(thisRec != rec)
            {
                newRecords.push_back(thisRec);
            }
        }

        PKSafeEvents::delayedEvents = newRecords;

        delete rec->thread;
        delete rec;

        PKSafeEvents::delayedEventMutex.release();
    }
                    
    delete pevent;

    return FALSE;
}

#endif

void PKSafeEvents::postSafeEventWithDelay(IPKSafeEventHandler *handler,
                                          double               delayInSeconds,
                                          std::string          eventName,
  	                                      void                *payload)
{
    if(handler == NULL || eventName == "")
    {
        return;
    }

    PKSafeEventRecord *newEvent = new PKSafeEventRecord();

    if(newEvent == NULL)
    {
        return;
    }

    PKThread *thread = new PKThread(newEvent);

    if(thread == NULL)
    {
        delete newEvent;
        return;
    }

    newEvent->thread    = thread;
    newEvent->handler   = handler;
    newEvent->delay     = delayInSeconds;
    newEvent->eventName = eventName;
    newEvent->payload   = payload;

    PKSafeEvents::delayedEventMutex.wait();
    PKSafeEvents::delayedEvents.push_back(newEvent);
    PKSafeEvents::delayedEventMutex.release();

    thread->start();
}

void PKSafeEvents::cancelDelayedEventsWithName(std::string name)
{
    PKSafeEvents::delayedEventMutex.wait();

    std::vector<PKSafeEventRecord *> newEvents;

    for(uint32_t i=0; i<PKSafeEvents::delayedEvents.size(); i++)
    {
        PKSafeEventRecord *eventRecord = PKSafeEvents::delayedEvents[i];

        if(eventRecord->eventName == name)
        {
            if(eventRecord->thread->isRunning())
            {
                eventRecord->thread->stop();
            }

            delete eventRecord->thread;
            delete eventRecord;
        }
        else
        {
            newEvents.push_back(eventRecord);
        }
    }

    PKSafeEvents::delayedEvents = newEvents;

    PKSafeEvents::delayedEventMutex.release();
}

//
// PKSafeEventRecord
//

void PKSafeEventRecord::runThread(PKThread *thread)
{
    double start_time = PKTime::getTimeInSeconds();

    while(!thread->getExitFlag())
    {
        // Check the time
        double time_now = PKTime::getTimeInSeconds();

        if((time_now - start_time) >= this->delay) // We should fire
        {
            if(thread->getExitFlag())
            {
                break;
            }

            // Fire the event
            PKSafeEvents::postSafeEvent(this->handler, 
                                        this->eventName, 
                                        this->payload, this);

            // Exit
            break;
        }

        PKTime::sleep(25); // 25ms resolution should be fine
    }

    // The record will be cleanup'd when the event is processed
}
