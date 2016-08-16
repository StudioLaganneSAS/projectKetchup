//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKControl.h"
#include "PKDialog.h"
#include "PKLayout.h"
#include "PKStr.h"
#include "PKCustomControl.h"

//
// Properties
//

std::string PKControl::ID_STRING_PROPERTY       = "id";
std::string PKControl::FRAME_RECT_PROPERTY      = "frame";
std::string PKControl::HIDDEN_BOOL_PROPERTY     = "hidden";
std::string PKControl::ENABLED_BOOL_PROPERTY    = "enabled";
std::string PKControl::LAYOUTX_INT32_PROPERTY   = "layoutx";
std::string PKControl::LAYOUTY_INT32_PROPERTY   = "layouty";
std::string PKControl::TOOLTIP_WSTRING_PROPERTY = "tooltip";	

//
// PKControl    
//

int32_t PKControl::nextControlId = 0;
std::vector <PKControl *> PKControl::controls;

PKControl *PKControl::getControlWithId(int32_t id)
{
    if(id >= nextControlId)
    {
        return NULL;
    }

	return PKControl::controls[id];
}


PKControl::PKControl()
{
    this->dialog = NULL;
    this->controlId = PKControl::nextControlId;
    this->showing = true;
    this->focused = false;
    this->focusDirection = PK_DIR_FORWARD;
	
#ifdef WIN32
	this->hwnd      = NULL;
    this->tooltip   = NULL;
    this->dontTrack = false;
    this->oldWindowLongPtr = NULL;
    this->oldWndProc = NULL;
#endif

#ifdef LINUX
    this->widget = NULL;
#endif
    this->noWidget = false;

#ifdef MACOSX
	this->controlRef = NULL;
	this->macId.signature = (OSType) this->controlId;
	this->macId.id        = (SInt32) this->controlId;
	this->fHelpContentProviderUPP = NULL;
	this->_focusedPart = 0;
	this->haveFocus = false;
	this->handler = NULL;
#endif

	PKControl::controls.push_back(this);
	PKControl::nextControlId++;

	PKRect fr = PKRectCreate(0, 0, 20, 20);

	PKOBJECT_ADD_STRING_PROPERTY(id, "NOT_SET");
	PKOBJECT_ADD_RECT_PROPERTY(frame, fr);
	PKOBJECT_ADD_BOOL_PROPERTY(hidden, false);
	PKOBJECT_ADD_BOOL_PROPERTY(enabled, true);
	PKOBJECT_ADD_INT32_PROPERTY(layoutx, PK_LAYOUT_MIN);
	PKOBJECT_ADD_INT32_PROPERTY(layouty, PK_LAYOUT_MIN);
    PKOBJECT_ADD_WSTRING_PROPERTY(tooltip, L"NOT_SET");
}

PKControl::~PKControl()
{
#ifdef WIN32

    if(this->oldWndProc)
    {
        SubclassWindow(this->hwnd, this->oldWndProc);
        this->oldWndProc = NULL;
        SetWindowLongPtr(this->hwnd, GWLP_USERDATA, this->oldWindowLongPtr);
    }

    if(this->tooltip != NULL)
    {
        DestroyWindow(this->tooltip);
    }

#endif

#ifdef MACOSX
	
	if(this->handler)
	{
		RemoveEventHandler(this->handler);
		this->handler = NULL;
	}
	
#endif
	
    this->clearTooltips();
}

void PKControl::build()
{
#ifdef LINUX
        
    if(this->widget != NULL)
    {
        // Set size if needed
        int32_t gw = -1;
        int32_t gh = -1;
        
        if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
        {
            gw = this->getW();
        }        

        if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
        {
            gh = this->getH();
        }        
        
        if(gw != -1 || gh != -1)
        {
            gtk_widget_set_size_request(this->widget, gw, gh);
        }                
    }
#endif
}

void PKControl::initialize(PKDialog *dialog)
{
    this->dialog = dialog;

    PKVariant *t = this->get(PKControl::TOOLTIP_WSTRING_PROPERTY);
    std::wstring tt = PKVALUE_WSTRING(t);

#ifdef WIN32

    // See if we have a HWND

    if(this->hwnd != NULL && 
       this->oldWndProc == NULL)
    {
        // Subclass so we can process a few messages
        this->oldWindowLongPtr = GetWindowLongPtr(this->hwnd, GWLP_USERDATA);
        SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG_PTR)(this));
        LONG_PTR newVal = GetWindowLongPtr(this->hwnd, GWLP_USERDATA);

        this->oldWndProc = SubclassWindow(this->hwnd, PKControl::PKControlWindowProc);
    }

	// See if we are enabled

	PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
	bool enabled = PKVALUE_BOOL(e);

	if(!enabled)
	{
		this->disable();
	}

    // Add a global tooltip if we have one

    if(tt != L"NOT_SET" && tt != L"")
    {
        MEMORY_BASIC_INFORMATION mbi;
        static int dummy = 0;
        VirtualQuery(&dummy, &mbi, sizeof(mbi));

        HMODULE hInst = (HMODULE)(mbi.AllocationBase);

        // Create the tooltip.

        HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                                  WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  dialog->getWindowHandle(), NULL, 
                                  hInst, NULL);
                                  
        if(!hwndTip)
        {
            return;
        }                              
                                  
        // Associate the tooltip with the tool.

        TOOLINFO toolInfo = { 0 };
        toolInfo.cbSize = sizeof(toolInfo);
        toolInfo.hwnd = dialog->getWindowHandle();
        toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        toolInfo.uId = (UINT_PTR)this->hwnd;
        toolInfo.lpszText = (LPWSTR) tt.c_str();

        SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
        SendMessage(hwndTip, TTM_SETDELAYTIME, (WPARAM) TTDT_INITIAL, (LPARAM) MAKELONG(1000, 0));
        SendMessage(hwndTip, TTM_SETDELAYTIME, (WPARAM) TTDT_AUTOPOP, (LPARAM) MAKELONG(10000, 0));

        this->tooltip = hwndTip;
    }

#endif

#ifdef LINUX
        
    if(this->widget != NULL)
    {
        // Connect focus events

        gtk_widget_set_events(this->widget, GDK_FOCUS_CHANGE_MASK);
        
        g_signal_connect(G_OBJECT(this->widget), "focus-in-event",
                         G_CALLBACK(PKControl::gtk_focus_in_event), this);  

        g_signal_connect(G_OBJECT(this->widget), "focus-out-event",
                         G_CALLBACK(PKControl::gtk_focus_out_event), this);  

        g_signal_connect(G_OBJECT(this->widget), "size-allocate",
                         G_CALLBACK(PKControl::gtk_size_allocation_event), this);  

        // Show the control
        gtk_widget_show(this->widget);

        // Add a global tooltip if we have one

        if(tt != L"NOT_SET" && tt != L"")
        {
            std::string ttUTF8 = PKStr::wStringToUTF8string(tt);
            gtk_widget_set_tooltip_text(this->widget, ttUTF8.c_str());
        }
    }    

#endif

#ifdef MACOSX
	
	if(this->controlRef != NULL)
	{
		if(this->handler == NULL)
		{
			// Install the event handler
			
			EventTypeSpec eventTypes[1];
			
			eventTypes[0].eventClass = kEventClassControl;
			eventTypes[0].eventKind  = kEventControlSetFocusPart;
			
			InstallEventHandler(GetControlEventTarget(this->controlRef),
								NewEventHandlerUPP(PKControl::PKControlEventHandler),
								1, eventTypes,
								(void *) this,
								&this->handler);	
			
		}
	}

    if(tt != L"NOT_SET" && tt != L"")
    {
		CFStringRef cfTip = PKStr::wStringToCFString(tt);
		
		OSStatus status;
		HMHelpContentRec helpTag;
	
		helpTag.version = kMacHelpVersion; 
		helpTag.tagSide = kHMDefaultSide; 
	
		SetRect (&helpTag.absHotRect, 0, 0, 0, 0); 
	
		helpTag.content[kHMMinimumContentIndex].contentType   = kHMCFStringLocalizedContent; 
		helpTag.content[kHMMinimumContentIndex].u.tagCFString = cfTip;
		helpTag.content[kHMMaximumContentIndex].contentType   = kHMNoContent; 
	
		status =  HMSetControlHelpContent(this->controlRef, &helpTag); 	

		CFRelease(cfTip);
	}
	
#endif
}

void PKControl::destroy()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            child->destroy();
            child = NULL;
		}
	}

    this->clearChildren();

    // Default impl
    this->release();
}

PKDialog *PKControl::getDialog()
{
    return this->dialog;
}

PKWindowHandle PKControl::getWindowHandle()
{
#ifdef WIN32
	return this->hwnd;
#endif

#ifdef LINUX
    return this->widget;
#endif

#ifdef MACOSX
	PKObject *parent = this->getParent();

	if(parent)
	{
		PKView *parentView = dynamic_cast<PKView*>(parent);

		if(parentView)
		{
			return parentView->getWindowHandle();
		}
	}
#endif

	return NULL;
}

#ifdef WIN32

HBRUSH PKControl::getBackgroundBrush()
{
    // By default, forward to parent

	PKObject *parent = this->getParent();

	if(parent)
	{
		PKView *parentView = dynamic_cast<PKView*>(parent);

		if(parentView)
		{
			return parentView->getBackgroundBrush();
		}
	}

    return NULL;
}

bool PKControl::drawsItsBackground()
{
    // default impl
    return false;
}

bool PKControl::parentDrawsItsBackground()
{
    // Ask parent if valid

    PKObject *p = this->getParent();

    if(p != NULL)
    {
        PKControl *control = dynamic_cast<PKControl*>(p);

        if(control)
        {
            bool drawsIt = control->drawsItsBackground();

            if(drawsIt)
            {
                return true;
            }
            else
            {
                return control->parentDrawsItsBackground();
            }
        }
        else
        {
            return false;
        }
    }

    return false;
}

void PKControl::fillDeviceWithBackground(PKDrawingDevice device,
                                         int x, int y, int w, int h)
{
    // default is to forward to parent
    // until one of those implement it

    PKObject *p = this->getParent();

    if(p != NULL)
    {
        PKControl *control = dynamic_cast<PKControl*>(p);

        if(control)
        {
            control->fillDeviceWithBackground(device, x, y, w, h);
        }
    }
}


#endif

#ifdef MACOSX

ControlRef PKControl::getControlRef()
{
	return this->controlRef;
}

#endif

void PKControl::setLayoutPolicy(PKLayoutPolicy layoutx,
								PKLayoutPolicy layouty)
{
	PKVariantInt32 lx(layoutx);
	PKVariantInt32 ly(layouty);

	this->set(LAYOUTX_INT32_PROPERTY, &lx);
	this->set(LAYOUTY_INT32_PROPERTY, &ly);
}

PKLayoutPolicy PKControl::getLayoutPolicyX()
{
	PKVariant *lx = this->get(LAYOUTX_INT32_PROPERTY);
	return (PKLayoutPolicy) PKVALUE_INT32(lx);
}

PKLayoutPolicy PKControl::getLayoutPolicyY()
{
	PKVariant *ly = this->get(LAYOUTY_INT32_PROPERTY);
	return (PKLayoutPolicy) PKVALUE_INT32(ly);
}

void PKControl::setControlId(std::string id)
{
	PKVariantString idv(id);
	this->set(ID_STRING_PROPERTY, &idv);
}

std::string PKControl::getControlId()
{
	PKVariant *id = this->get(ID_STRING_PROPERTY);
	return PKVALUE_STRING(id);
}

void PKControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    // TOOLTIP

    if(prop->getName() == TOOLTIP_WSTRING_PROPERTY)
    {

        PKVariant *t = this->get(PKControl::TOOLTIP_WSTRING_PROPERTY);
        std::wstring tt = PKVALUE_WSTRING(t);

		if(tt == L"NOT_SET")
		{
			return;
		}

        if(this->dialog == NULL)
        {
            return;
        }

#ifdef LINUX

        if(tt != L"NOT_SET" && tt != L"" && this->widget)
        {
            std::string ttUTF8 = PKStr::wStringToUTF8string(tt);
            gtk_widget_set_tooltip_text(this->widget, ttUTF8.c_str());
        }

#endif

#ifdef WIN32

		if(this->tooltip == NULL)
		{
			return;
		}

        TOOLINFO toolInfo = { 0 };
        toolInfo.cbSize = sizeof(toolInfo);
        toolInfo.hwnd = this->dialog->getWindowHandle();
        toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        toolInfo.uId = (UINT_PTR)this->hwnd;
        toolInfo.lpszText = (LPWSTR) tt.c_str();

        SendMessage(this->tooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);

#endif

#ifdef MACOSX

		if(tt != L"")
		{
			CFStringRef cfTip = PKStr::wStringToCFString(tt);
			
			OSStatus status;
			HMHelpContentRec helpTag;
			
			helpTag.version = kMacHelpVersion; 
			helpTag.tagSide = kHMDefaultSide; 
			
			SetRect (&helpTag.absHotRect, 0, 0, 0, 0); 
			
			helpTag.content[kHMMinimumContentIndex].contentType   = kHMCFStringLocalizedContent; 
			helpTag.content[kHMMinimumContentIndex].u.tagCFString = cfTip;
			helpTag.content[kHMMaximumContentIndex].contentType   = kHMNoContent; 
			
			status =  HMSetControlHelpContent(this->controlRef, &helpTag); 	
			
			CFRelease(cfTip);
		}

#endif
    }

	// FRAME

	if(prop->getName() == FRAME_RECT_PROPERTY)
	{
		PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
		PKRect frame  = PKVALUE_RECT(fr);

#ifdef WIN32
		if(this->hwnd != NULL)
		{
			MoveWindow(this->hwnd, 
					   frame.x, frame.y, 
					   frame.w, frame.h, 
					   TRUE);
		}
#endif

#ifdef LINUX

        if(this->widget != NULL)
        {
            gtk_widget_set_uposition(this->widget, frame.x, frame.y);
            gtk_widget_set_usize(this->widget, frame.w, frame.h);
        }

#endif

#ifdef MACOSX
		if(this->controlRef != NULL)
		{
			MoveControl(this->controlRef, frame.x, frame.y);
			SizeControl(this->controlRef, frame.w, frame.h);
		}
#endif
	}

	// ENABLED

	if(prop->getName() == ENABLED_BOOL_PROPERTY)
	{
		PKVariant *e = this->get(ENABLED_BOOL_PROPERTY);

		if(PKVALUE_BOOL(e))
		{
#ifdef WIN32
			if(this->hwnd != NULL)
			{
				EnableWindow(this->hwnd, TRUE);
			}
#endif

#ifdef LINUX
            if(this->widget != NULL)
            {
                gtk_widget_set_sensitive(this->widget, true);
            }
#endif

#ifdef MACOSX
			if(this->controlRef != NULL)
			{
				EnableControl(this->controlRef);
			}
#endif
		}
		else
		{
#ifdef WIN32
			if(this->hwnd != NULL)
			{
				EnableWindow(this->hwnd, FALSE);
			}
#endif

#ifdef LINUX
            if(this->widget != NULL)
            {
                gtk_widget_set_sensitive(this->widget, false);
            }
#endif

#ifdef MACOSX
			if(this->controlRef != NULL)
			{
				DisableControl(this->controlRef);
			}
#endif
		}
	}
}

void PKControl::doCut()
{
#ifdef WIN32
    if(this->hwnd)
    {
        SendMessage(this->hwnd, WM_CUT, 0, 0);
    }
#endif
	
#ifdef LINUX
    if(this->widget)
    {
        if(GTK_IS_EDITABLE(this->widget))
        {
            gtk_editable_cut_clipboard(GTK_EDITABLE(this->widget));
        }        
    }
#endif

#ifdef MACOSX
	if(this->controlRef)
	{
		OSErr    err;
		EventRef myEvent;
		
		err = CreateEvent(kCFAllocatorDefault,
						  kEventClassCommand, 
						  kEventProcessCommand,
						  0,
						  kEventAttributeNone,
						  &myEvent);
		
		if(err != noErr)
		{
			return;
		}
		
		HICommand command;
		memset(&command, 0, sizeof(HICommand));
		command.commandID = kHICommandCut;
		
		err = SetEventParameter(myEvent,
								kEventParamHICommand,
								typeHICommand,
								sizeof(HICommand),
								(void*) &command);
		
		err = SetEventParameter(myEvent,
								kEventParamDirectObject,
								typeHICommand,
								sizeof(HICommand),
								(void*) &command);
		
		SendEventToControl(myEvent, this->controlRef);
		
	}		
#endif
}

void PKControl::doCopy()
{
#ifdef WIN32
    if(this->hwnd)
    {
        SendMessage(this->hwnd, WM_COPY, 0, 0);
    }
#endif

#ifdef LINUX
    if(this->widget)
    {
        if(GTK_IS_EDITABLE(this->widget))
        {
            gtk_editable_copy_clipboard(GTK_EDITABLE(this->widget));
        }        
    }
#endif
	
#ifdef MACOSX
	if(this->controlRef)
	{
		if(this->controlRef)
		{
			OSErr    err;
			EventRef myEvent;
			
			err = CreateEvent(kCFAllocatorDefault,
							  kEventClassCommand, 
							  kEventProcessCommand,
							  0,
							  kEventAttributeNone,
							  &myEvent);
			
			if(err != noErr)
			{
				return;
			}
			
			HICommand command;
			memset(&command, 0, sizeof(HICommand));
			command.commandID = kHICommandCopy;
			
			err = SetEventParameter(myEvent,
									kEventParamHICommand,
									typeHICommand,
									sizeof(HICommand),
									(void*) &command);
			
			err = SetEventParameter(myEvent,
									kEventParamDirectObject,
									typeHICommand,
									sizeof(HICommand),
									(void*) &command);
			
			SendEventToControl(myEvent, this->controlRef);
			
		}		
	}		
#endif
}

void PKControl::doPaste()
{
#ifdef WIN32
    if(this->hwnd)
    {
        SendMessage(this->hwnd, WM_PASTE, 0, 0);
    }
#endif
	
#ifdef LINUX
    if(this->widget)
    {
        if(GTK_IS_EDITABLE(this->widget))
        {
            gtk_editable_paste_clipboard(GTK_EDITABLE(this->widget));
        }        
    }
#endif

#ifdef MACOSX
	if(this->controlRef)
	{
		OSErr    err;
		EventRef myEvent;
		
		err = CreateEvent(kCFAllocatorDefault,
						  kEventClassCommand, 
						  kEventProcessCommand,
						  0,
						  kEventAttributeNone,
						  &myEvent);
		
		if(err != noErr)
		{
			return;
		}
		
		HICommand command;
		memset(&command, 0, sizeof(HICommand));
		command.commandID = kHICommandPaste;
		
		err = SetEventParameter(myEvent,
								kEventParamHICommand,
								typeHICommand,
								sizeof(HICommand),
								(void*) &command);
		
		err = SetEventParameter(myEvent,
								kEventParamDirectObject,
								typeHICommand,
								sizeof(HICommand),
								(void*) &command);
		
		SendEventToControl(myEvent, this->controlRef);
		
	}		
#endif
}

void PKControl::doSelectAll()
{
#ifdef WIN32
    if(this->hwnd)
    {
        SendMessage(this->hwnd, EM_SETSEL, 0, -1);
    }
#endif
	
#ifdef LINUX
    if(this->widget)
    {
        if(GTK_IS_EDITABLE(this->widget))
        {
            gtk_editable_select_region(GTK_EDITABLE(this->widget), 0, -1);
        }        
    }
#endif

#ifdef MACOSX
	if(this->controlRef)
	{
		OSErr    err;
		EventRef myEvent;
		
		err = CreateEvent(kCFAllocatorDefault,
						  kEventClassCommand, 
						  kEventProcessCommand,
						  0,
						  kEventAttributeNone,
						  &myEvent);
		
		if(err != noErr)
		{
			return;
		}
		
		HICommand command;
		memset(&command, 0, sizeof(HICommand));
		command.commandID = kHICommandSelectAll;
		
		err = SetEventParameter(myEvent,
								kEventParamHICommand,
								typeHICommand,
								sizeof(HICommand),
								(void*) &command);
		
		err = SetEventParameter(myEvent,
								kEventParamDirectObject,
								typeHICommand,
								sizeof(HICommand),
								(void*) &command);
		
		SendEventToControl(myEvent, this->controlRef);
		
	}		
#endif
}

bool PKControl::hasSelection()
{
    return false;
}

void PKControl::move(int32_t x, 
			  	     int32_t y)
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	PKRect frame  = PKVALUE_RECT(fr);

	frame.x = x;
	frame.y = y;

	PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
	this->set(FRAME_RECT_PROPERTY, &newFrame);
}

void PKControl::moveBy(int32_t x, 
		               int32_t y)
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	PKRect frame  = PKVALUE_RECT(fr);

	frame.x += x;
	frame.y += y;

	PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
	this->set(FRAME_RECT_PROPERTY, &newFrame);

    // Move children by the same offset

    for(uint32_t i=0; i < this->getChildrenCount(); i++)
    {
        PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

        if(child)
        {
            child->moveBy(x, y);
        }
    }
}

void PKControl::resize(uint32_t w, 
					   uint32_t h)
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	PKRect frame  = PKVALUE_RECT(fr);

	frame.w = w;
	frame.h = h;

	PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
	this->set(FRAME_RECT_PROPERTY, &newFrame);
}

void PKControl::setHiddenRecursive(bool hidden)
{
	PKVariantBool h(hidden);
	this->set(HIDDEN_BOOL_PROPERTY, &h);

    for(uint32_t i=0; i<this->getChildrenCount(); i++)
    {
        PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

        if(child)
        {
            child->setHiddenRecursive(hidden);
        }
    }
}

void PKControl::setHidden(bool hidden)
{
	PKVariantBool h(hidden);
	this->set(HIDDEN_BOOL_PROPERTY, &h);
}

bool PKControl::getHidden()
{
	PKVariant *hidden = this->get(HIDDEN_BOOL_PROPERTY);
	return PKVALUE_BOOL(hidden);
}

bool PKControl::isShowing()
{
    return this->showing;
}

void PKControl::show()
{
	PKVariant *hidden = this->get(HIDDEN_BOOL_PROPERTY);

	if(PKVALUE_BOOL(hidden))
	{
		return;
	}

    this->showing = true;

#ifdef WIN32
	if(this->hwnd != NULL)
	{
		ShowWindow(this->hwnd, SW_SHOWNORMAL);
	}
#endif

#ifdef LINUX
    if(this->widget != NULL)
    {
        gtk_widget_show(this->widget);
    }
#endif

#ifdef MACOSX
	if(this->controlRef != NULL)
	{
		ShowControl(this->controlRef);
	}
#endif
}

void PKControl::hide()
{
    this->showing = false;

#ifdef WIN32
	if(this->hwnd != NULL)
	{
		ShowWindow(this->hwnd, SW_HIDE);
	}
#endif

#ifdef LINUX
    if(this->widget != NULL)
    {
        gtk_widget_hide(this->widget);
    }
#endif

#ifdef MACOSX
	if(this->controlRef != NULL)
	{
		HideControl(this->controlRef);
	}
#endif
}

void PKControl::enable()
{
	PKVariantBool enabled(true);
	this->set(ENABLED_BOOL_PROPERTY, &enabled);
}

void PKControl::disable()
{
	PKVariantBool enabled(false);
	this->set(ENABLED_BOOL_PROPERTY, &enabled);
}

void PKControl::updateUIStrings()
{
    
}

void PKControl::relayout()
{
    // Subclasses will implement
}

void PKControl::invalidate(bool recurse)
{
    if(!this->dialog)
    {
        return;
    }
    
    if(!this->dialog->isShowing())
    {
        return;
    }
    
#ifdef WIN32

    if(this->hwnd)
    {
        RECT r;
        memset(&r, 0, sizeof(RECT));
        r.right  = this->getW();
        r.bottom = this->getH();
        InvalidateRect(this->hwnd, &r, FALSE);
    }

#endif
		
#ifdef LINUX
    
	// We need this to be thread safe
	PKSafeEvents::postSafeEvent(this, "INVALIDATE", NULL);
    
#endif
		
#ifdef MACOSX
	
	// We need this to be thread safe
	PKSafeEvents::postSafeEvent(this, "INVALIDATE", NULL);
	
#endif

	if(recurse)
	{
		for(uint32_t i=0; i < this->getChildrenCount(); i++)
		{
			PKControl *control = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(control)
			{
				control->invalidate();
			}
		}
	}
}

void PKControl::invalidateRect(PKRect client)
{
    if(!this->dialog)
    {
        return;
    }
    
    if(!this->dialog->isShowing())
    {
        return;
    }
    
#ifdef WIN32

    if(this->hwnd)
    {
        RECT r;
		r.left   = client.x;
		r.top    = client.y;
        r.right  = client.x + client.w + 1;
        r.bottom = client.y + client.h + 1;

        InvalidateRect(this->hwnd, &r, FALSE);
    }

#endif
		
#ifdef LINUX
    
	// We need this to be thread safe
	
	PKRect *rect = new PKRect;
	
	rect->x = client.x;
	rect->y = client.y;
	rect->w = client.w;
	rect->h = client.h;

	PKSafeEvents::postSafeEvent(this, "INVALIDATE_RECT", (void*)rect);
    
#endif
		
#ifdef MACOSX
	
	// We need this to be thread safe

	PKRect *rect = new PKRect;
	
	rect->x = client.x;
	rect->y = client.y;
	rect->w = client.w;
	rect->h = client.h;

	PKSafeEvents::postSafeEvent(this, "INVALIDATE_RECT", (void*)rect);

#endif
}

void PKControl::invalidateParentControl()
{
    PKObject *parent = this->getParent();

    if(parent != NULL)
    {
        PKLayout *layout = dynamic_cast<PKLayout*>(parent);

        if(layout)
        {
            layout->invalidateParentControl();
        }
        else
        {
            this->invalidate();
        }
    }
}

void PKControl::focusAcquired(PKDirection direction)
{
    this->focused = true;
    // Reimplement if needed
}

void PKControl::focusLost()
{
    this->focused = false;
    this->focusDirection = PK_DIR_NONE;
    // Reimplement if needed
}

bool PKControl::hasFocus()
{
    return this->focused;
}

bool PKControl::moveFocusToNextControlPart()
{
	// No next part by default
	return false;
}

bool PKControl::moveFocusToPreviousControlPart()
{
	// No previous part by default
	return false;
}

#ifdef MACOSX

pascal OSStatus PKControl::HelpContentProvider(ControlRef inControl, 
											   Point inGlobalMouse, 
											   HMContentRequest inRequest, 
											   HMContentProvidedType *outContentProvided, 
											   HMHelpContentPtr ioHelpContent)
{
    Point   localMouse;
    GrafPtr currPort;	
	
    if(inRequest != kHMSupplyContent)
	{
        return noErr;
	}
	    
    // Convert the global mouse to dialog relative
	
    GetPort(&currPort);
    SetPortWindowPort(GetControlOwner(inControl));
    
    localMouse = inGlobalMouse;
    GlobalToLocal(&localMouse);
    
	// Convert to control coordinates
	
	HIPoint point;
	point.x = 0;
	point.y = 0;
	
	WindowRef window = GetControlOwner(inControl);
	HIViewRef mContent;
	
	HIViewFindByID(HIViewGetRoot(window),
				   kHIViewWindowContentID, &mContent);		
	HIViewConvertPoint(&point, inControl, mContent);
	
	localMouse.h -= point.x;
	localMouse.v -= point.y;
	
    // Assume we have no content
    *outContentProvided = kHMContentNotProvided;	
	
	// Get hold of the control
	
	PKControl *ctrl = NULL;
	
	for(uint32_t i=0; i < PKControl::controls.size(); i++)
	{
		if(PKControl::controls[i]->getControlRef() == inControl)
		{
			ctrl = PKControl::controls[i];
			break;
		}
	}
	
	if(ctrl == NULL)
	{
		return noErr;
	}
	
	// See if we match inside any of the 
	// control's own tooltip rects
	
	for(uint32_t i=0; i < ctrl->tooltipRects.size(); i++)
	{
		PKRect rect = ctrl->tooltipRects[i];
		
		if((localMouse.h >= rect.x) && (localMouse.h <= (rect.x+rect.w)) &&
		   (localMouse.v >= rect.y) && (localMouse.v <= (rect.y+rect.h)))
		{
			CFStringRef cfTip = ctrl->tooltipTexts[i];
			
			ioHelpContent->version = kMacHelpVersion; 
			ioHelpContent->tagSide = kHMDefaultSide; 
			
			Point pt;
			pt.h = rect.x+point.x;
			pt.v = rect.y+point.y;
			LocalToGlobal(&pt);
			
			SetRect(&(ioHelpContent->absHotRect), pt.h, pt.v, pt.h+rect.w, pt.v+rect.h);
						
			ioHelpContent->content[kHMMinimumContentIndex].contentType   = kHMCFStringLocalizedContent; 
			ioHelpContent->content[kHMMinimumContentIndex].u.tagCFString = cfTip;
			ioHelpContent->content[kHMMaximumContentIndex].contentType   = kHMCFStringLocalizedContent; 
			ioHelpContent->content[kHMMaximumContentIndex].u.tagCFString = cfTip;
						
			*outContentProvided = kHMContentProvided;
			
			break;
		}
	}

	SetPort(currPort);
		
	return noErr;
}

#endif

void PKControl::clearTooltips()
{
#ifdef WIN32
    for(uint32_t i=0; i < this->tooltips.size(); i++)
    {
        DestroyWindow(this->tooltips[i]);
    }

    this->tooltips.clear();
#endif

#ifdef MACOSX
    for(uint32_t i=0; i < this->tooltipTexts.size(); i++)
    {
        CFRelease(this->tooltipTexts[i]);
    }
	
    this->tooltipTexts.clear();
    this->tooltipRects.clear();
#endif
}

void PKControl::addTooltip(PKRect rect, std::wstring text)
{
    if(this->dialog == NULL)
    {
        return;
    }

	if(text == L"NOT_SET")
	{
		return;
	}

#ifdef WIN32

    MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

    HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    // Create the tooltip.

    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                              WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              this->dialog->getWindowHandle(), NULL, 
                              hInst, NULL);
                              
    if(!hwndTip)
    {
        return;
    }                              
                              
    // Associate the tooltip with the tool.

    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize   = sizeof(toolInfo);
    toolInfo.hwnd     = this->hwnd;
    toolInfo.uFlags   = TTF_SUBCLASS;
    toolInfo.uId      = 0;
    toolInfo.lpszText = (LPWSTR) text.c_str();

    toolInfo.rect.left   = rect.x;
    toolInfo.rect.right  = rect.x + rect.w;
    toolInfo.rect.top    = rect.y;
    toolInfo.rect.bottom = rect.y + rect.h;

    SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
    SendMessage(hwndTip, TTM_SETDELAYTIME, (WPARAM) TTDT_INITIAL, (LPARAM) MAKELONG(1000, 0));
    SendMessage(hwndTip, TTM_SETDELAYTIME, (WPARAM) TTDT_AUTOPOP, (LPARAM) MAKELONG(10000, 0));

    this->tooltips.push_back(hwndTip);

#endif

#ifdef LINUX

	// TODO

#endif

#ifdef MACOSX

	if(text != L"")
	{
		this->tooltipRects.push_back(rect);
		this->tooltipTexts.push_back(PKStr::wStringToCFString(text));

		// Install the handler
		
		if(this->fHelpContentProviderUPP == NULL)
		{
			this->fHelpContentProviderUPP = NewHMControlContentUPP(PKControl::HelpContentProvider);
			HMInstallControlContentCallback(this->controlRef, this->fHelpContentProviderUPP);
		}		
	}

#endif 
}

// Child management

void PKControl::deleteAllChildControls()
{
	for(uint32_t i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->hide();
			delete child;
		}
	}

	this->clearChildren();
}

int32_t PKControl::getAbsoluteX()
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);

    if(this->parent == NULL)
    {
        return PKVALUE_RECT(fr).x;
    }
    else
    {
        PKControl *parentControl = dynamic_cast<PKControl*>(parent);

        if(parentControl)
        {
            return (PKVALUE_RECT(fr).x + parentControl->getAbsoluteX());
        }
        else
        {
            // Parent is a window
            return PKVALUE_RECT(fr).x;
        }
    }

    return 0;
}

int32_t PKControl::getAbsoluteY()
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);

    if(this->parent == NULL)
    {
        return PKVALUE_RECT(fr).y;
    }
    else
    {
        PKControl *parentControl = dynamic_cast<PKControl*>(parent);

        if(parentControl)
        {
            return (PKVALUE_RECT(fr).y + parentControl->getAbsoluteY());
        }
        else
        {
            // Parent is a window
            return PKVALUE_RECT(fr).y;
        }
    }

    return 0;
}

int32_t PKControl::getX()
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	return PKVALUE_RECT(fr).x;
}

int32_t PKControl::getY()
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	return PKVALUE_RECT(fr).y;
}

uint32_t PKControl::getW()
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	return PKVALUE_RECT(fr).w;
}

uint32_t PKControl::getH()
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	return PKVALUE_RECT(fr).h;
}

void PKControl::setNoWidget(bool nw)
{
    this->noWidget = nw;
}

// Child management


PKControl *PKControl::findChildControlAt(int32_t x, 
								         int32_t y)
{
	PKVariant *fr = this->get(PKControl::FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(fr);

    // We need to walk children backwards, 
    // so that we traverse them back to front
    // to respect the possible natural Z-order

    for(int32_t i=this->getChildrenCount()-1; i>=0; i--)
    {
	    PKControl *child = dynamic_cast<PKControl *> (this->getChildAt(i));
		
	    if(child)
	    {
		    PKVariant *fr_ch = child->get(PKControl::FRAME_RECT_PROPERTY);
		    PKRect frame_ch  = PKVALUE_RECT(fr_ch);

		    int diff_x = (frame_ch.x - frame.x);
		    int diff_y = (frame_ch.y - frame.y);

		    PKControl *found = child->findChildControlAt(x - diff_x, y - diff_y);
			
		    if(found)
		    {
			    return found;
		    }
	    }
    }

	// If none of our child was hit, 
	// then let's try ourselves
	
	if((x >= 0 && (x <= (int)frame.w)) &&
	   (y >= 0 && (y <= (int)frame.h)))
	{
		return this;
	}
	
	return NULL;
}


PKView *PKControl::findObjectById(std::string id)
{
	PKVariant *_idv = this->get(ID_STRING_PROPERTY);
	std::string _id = PKVALUE_STRING(_idv);

	if(id == _id)
	{
		return this;
	}

	// Try children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKObject  *child = this->getChildAt(i);
		PKControl *childControl = dynamic_cast<PKControl*>(child);

		if(childControl)
		{
			PKVariant *c_idv = childControl->get(ID_STRING_PROPERTY);
			std::string c_id = PKVALUE_STRING(c_idv);

			if(c_id == id)
			{
				// found it
				return (PKView *) childControl;
			}
			else
			{
				// query it
				PKView *result = childControl->findObjectById(id);

				if(result != NULL)
				{
					return result;
				}
			}
		}
	}

	return NULL;
}

#ifdef LINUX
PKView *PKControl::findObjectByWidget(GtkWidget *w)
{
	if(this->widget == w)
	{
		return this;
	}

	// Try children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKObject  *child = this->getChildAt(i);
		PKControl *childControl = dynamic_cast<PKControl*>(child);

		if(childControl)
		{
			if(childControl->widget == w)
			{
				// found it
				return (PKView *) childControl;
			}
			else
			{
				// query it
				PKView *result = childControl->findObjectByWidget(w);

				if(result != NULL)
				{
					return result;
				}
			}
		}
	}

	return NULL;
}
#endif

#ifdef WIN32
PKView *PKControl::findObjectByWindow(HWND w)
{
	if(this->hwnd == w)
	{
		return this;
	}

	// Try children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKObject  *child = this->getChildAt(i);
		PKControl *childControl = dynamic_cast<PKControl*>(child);

		if(childControl)
		{
			if(childControl->hwnd == w)
			{
				// found it
				return (PKView *) childControl;
			}
			else
			{
				// query it
				PKView *result = childControl->findObjectByWindow(w);

				if(result != NULL)
				{
					return result;
				}
			}
		}
	}

	return NULL;
}

bool PKControl::relayWIN32Event(HWND   parent,
                                UINT   msg, 
				                WPARAM wParam, 
				                LPARAM lParam)
{
    if(this->hwnd == NULL)
    {
        return false;
    }

    bool enabled = PKVALUE_BOOL(this->get(PKControl::ENABLED_BOOL_PROPERTY));

    if(!enabled)
    {
        return false;
    }

    // Default implementation

    switch(msg)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MOUSEMOVE:
        {
            if(!this->isShowing())
            {
                return false;
            }

            // Let's see if it falls inside us

            int x = LOWORD(lParam); 
            int y = HIWORD(lParam); 

            if(x >= 32768) x = -1;
            if(y >= 32768) y = -1;

            POINT pt;
            pt.x = x;
            pt.y = y;

            ClientToScreen(parent, &pt);
            ScreenToClient(this->hwnd, &pt);

            if(pt.x >= 0 && pt.x <= (int) this->getW() &&
               pt.y >= 0 && pt.y <= (int) this->getH())
            {
                LPARAM newLParam;
                newLParam = (pt.y << 16 | pt.x);
                this->dontTrack = true;
                SendMessage(this->hwnd, msg, wParam, newLParam);
                this->dontTrack = false;
                return true;
            }
        }
        break;

    case WM_MOUSELEAVE:
        {
            if(!this->isShowing())
            {
                return false;
            }

            SendMessage(this->hwnd, WM_MOUSELEAVE, 0, 0);
        }
        break;
    }

    return false;
}

#endif

#ifdef MACOSX
PKView *PKControl::findObjectByControlRef(ControlRef cRef)
{
	if(this->controlRef == cRef)
	{
		return this;
	}
	
	// Try children
	
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKObject  *child = this->getChildAt(i);
		PKControl *childControl = dynamic_cast<PKControl*>(child);
		
		if(childControl)
		{
			if(childControl->controlRef == cRef)
			{
				// found it
				return (PKView *) childControl;
			}
			else
			{
				// query it
				PKView *result = childControl->findObjectByControlRef(cRef);
				
				if(result != NULL)
				{
					return result;
				}
			}
		}
	}
	
	return NULL;	
}
#endif


#ifdef MACOSX

int32_t PKControl::getInternalControlId()
{
	return this->controlId;
}

#endif

void PKControl::processSafeEvent(std::string  eventName,
							     void        *payload)
{
	if(eventName == "INVALIDATE")
	{
#ifdef MACOSX
		HIViewSetNeedsDisplay(this->controlRef, true);
#endif
		
#ifdef LINUX
    if(this->widget != NULL)
    {
        gtk_widget_queue_draw(this->widget);
    }
#endif
	}

	if(eventName == "INVALIDATE_RECT")
	{
		PKRect *rect = (PKRect *) payload;

#ifdef MACOSX
		Rect r;
		
		r.left   = rect->x;
		r.top    = rect->y;
		r.right  = rect->x + rect->w;
		r.bottom = rect->y + rect->h;

		RgnHandle region = NewRgn();
		RectRgn(region, &r);
		HIViewSetNeedsDisplayInRegion(this->controlRef, region, true);
		DisposeRgn(region);
#endif

#ifdef LINUX
		if(this->widget != NULL)
		{
			   gtk_widget_queue_draw(this->widget);
//			   gtk_widget_queue_draw_area(this->widget, rect->x, rect->y, rect->w, rect->h);
	    }
#endif

		delete rect;
	}

}

#ifdef LINUX

gboolean PKControl::gtk_focus_in_event(GtkWidget     *widget,
                                       GdkEventFocus *event,
                                       gpointer       user_data)
{
    PKControl *ctrl = (PKControl *) user_data;

    if(ctrl == NULL)
    {
        return FALSE;
    }
    
    if(ctrl->dialog)
    {
        ctrl->dialog->current_focus = ctrl;
    }
    
    ctrl->focusAcquired(ctrl->focusDirection);

    return FALSE;
}

gboolean PKControl::gtk_focus_out_event(GtkWidget     *widget,
                                        GdkEventFocus *event,
                                        gpointer       user_data)
{
    PKControl *ctrl = (PKControl *) user_data;

    if(ctrl == NULL)
    {
        return FALSE;
    }

    ctrl->focusLost();

    return FALSE;
}

void PKControl::gtk_size_allocation_event(GtkWidget     *widget,
                                          GtkAllocation *allocation,
                                          gpointer       user_data)
{
    PKControl *control = (PKControl *) user_data;
    
    if(control)
    {
        PKVariant *f = control->get(PKControl::FRAME_RECT_PROPERTY);
        PKRect frame = PKVALUE_RECT(f);
        
        frame.w = allocation->width;
        frame.h = allocation->height;
        
        PKVariantRect f2(frame.x, frame.y, frame.w, frame.h);
        
        control->getProperties()->setNoCallback(PKControl::FRAME_RECT_PROPERTY, &f2);
    }    
}
                                                       
#endif

#ifdef WIN32

LRESULT CALLBACK PKControl::PKControlWindowProc(HWND hWnd,
						 		                UINT uMsg,
							 	                WPARAM wParam,
								                LPARAM lParam)
{
	PKControl *control = (PKControl *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(control == NULL)
	{
		return DefDlgProc(hWnd, uMsg, wParam, lParam);
	}

    switch(uMsg)
    {
    // Focus

    case WM_SETFOCUS:
        {
            PKDialog *dialog = control->getDialog();

            if(!dialog)
            {
                break;
            }

            HWND lost = (HWND) wParam;
            HWND prev = GetNextDlgTabItem(dialog->getWindowHandle(), lost, TRUE);
            HWND next = GetNextDlgTabItem(dialog->getWindowHandle(), lost, FALSE);

            if(prev == next) // only two controls, can't know
            {
                control->focusAcquired(PK_DIR_FORWARD);
            }
            else
            {
                if(hWnd == prev)
                {
                    control->focusAcquired(PK_DIR_BACKWARD);
                }
                else
                {
                    control->focusAcquired(PK_DIR_FORWARD);
                }
            }
        }
        break;

    case WM_KILLFOCUS:
        {
            control->focusLost();
        }
        break;
    }

	return CallWindowProc(control->oldWndProc, hWnd, uMsg, wParam, lParam);
}

#endif

#ifdef MACOSX

pascal OSStatus PKControl::PKControlEventHandler(EventHandlerCallRef  nextHandler,
											     EventRef             inEvent, 
											     void                *userData)
{
    UInt32 eventClass = GetEventClass(inEvent);
	
    switch(eventClass)
    {
		case kEventClassControl:
		{
			ControlRef theControl;
			ControlID  controlID;
			
			GetEventParameter(inEvent, kEventParamDirectObject, typeControlRef, NULL, sizeof( ControlRef ), NULL, &theControl );
			GetControlID(theControl, &controlID);
			
			if(controlID.id != (SInt32) controlID.signature)
			{
				// Not one of our controls
				return eventNotHandledErr;
			}
			
			PKControl *obj = PKControl::getControlWithId(controlID.id);
			UInt32 eventKind = GetEventKind(inEvent);
			
			PKCustomControl *custom = dynamic_cast<PKCustomControl*> (obj);
			
			if(eventKind == kEventControlSetFocusPart && obj != NULL && custom == NULL)
			{
				// focus handler
				
				OSStatus            err = noErr;
				ControlPartCode		desiredFocus;
				ControlPartCode		actualFocus;
				
				GetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, NULL, sizeof( ControlPartCode ), NULL, &desiredFocus );
								
				if((desiredFocus == kControlFocusNextPart) || (desiredFocus == 5)) // 5 happens with edit controls when you click on them 
				{
					if(!obj->haveFocus)
					{
						obj->haveFocus    = true;
						obj->focusAcquired(PK_DIR_FORWARD);
						obj->_focusedPart = desiredFocus;
					}
					else
					{
						// We already have focus, let's ask
						// our control if it has more parts
						// to focus on
						
						if(obj->moveFocusToNextControlPart())
						{
							obj->haveFocus    = true;
							obj->_focusedPart = desiredFocus;
						}
						else
						{
							obj->haveFocus = false;
							obj->_focusedPart = kControlFocusNoPart;
							obj->focusLost();
						}
					}
				}
				else if((desiredFocus == kControlFocusPrevPart))
				{
					if(!obj->haveFocus)
					{
						obj->haveFocus    = true;
						obj->focusAcquired(PK_DIR_BACKWARD);
						obj->_focusedPart = desiredFocus;
					}
					else
					{
						// We already have focus, let's ask
						// our control if it has more parts
						// to focus on
						
						if(obj->moveFocusToPreviousControlPart())
						{
							obj->haveFocus    = true;
							obj->_focusedPart = desiredFocus;
						}
						else
						{
							obj->haveFocus = false;
							obj->_focusedPart = kControlFocusNoPart;
							obj->focusLost();
						}
					}
				}
				else if(desiredFocus == kControlFocusNoPart)
				{
					obj->haveFocus = false;
					obj->focusLost();
					obj->_focusedPart = desiredFocus;
				}				
			}			
		}
		break;
	}
			
	return eventNotHandledErr;
}

#endif
