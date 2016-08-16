//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKSlider.h"
#include "PKDialog.h"


#ifdef MACOSX

pascal void PKSlider::SliderAction( ControlRef theControl, ControlPartCode partCode )
{
	if(partCode == kControlIndicatorPart)
	{
		PKSlider *slider = (PKSlider *) GetControlReference(theControl);
		
		if(slider == NULL)
		{
			return;
		}

		// Send a message to the parent
		// window so it knows the slider
		// value has changed
		
		OSErr	 err;
		EventRef newEvent;
    
		if(CreateEvent(NULL, kEventClassPKSlider, kEventPKSliderTrack,
					   0, kEventAttributeNone, &newEvent) != noErr)
		{
			return;
		}
		
		PKObject *parent = slider->getParent();
		
		if(parent == NULL)
		{
			return;
		}
		
		PKView *parentView = dynamic_cast<PKView*>(parent);
		
		if(parentView == NULL)
		{
			return;
		}
		
		EventTargetRef target = GetWindowEventTarget(GetWindowFromPort(parentView->getWindowHandle()));
	
		err = SetEventParameter(newEvent, kEventParamPostTarget,
								typeEventTargetRef, sizeof(target), &target);
							
		void *param = (void *) slider->getInternalControlId();
							
		err = SetEventParameter(newEvent, kEventParamUserData,
								typeVoidPtr, sizeof(param), &param);

		if (err == noErr) 
		{
			err = PostEventToQueue(GetMainEventQueue(), newEvent, kEventPriorityStandard);
			ReleaseEvent(newEvent);
		}
	}
}

#endif

//
// Properties
//

std::string PKSlider::RANGE_INT32RANGE_PROPERTY = "range";
std::string PKSlider::VALUE_INT32_PROPERTY      = "value";

//
// PKSlider
//

PKSlider::PKSlider()
{
    PKInt32Range r = PKInt32RangeCreate(0, 100);

    PKOBJECT_ADD_INT32RANGE_PROPERTY(range, r);
    PKOBJECT_ADD_INT32_PROPERTY(value, 0);
}

PKSlider::~PKSlider()
{
#ifdef WIN32

	if(this->hwnd != NULL)
	{
		DestroyWindow(this->hwnd);
	}

#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		DisposeControl(this->controlRef);
	}

#endif

}

void PKSlider::build()
{
	PKObject *parent = this->getParent();

	if(parent == NULL)
	{
		return;
	}

	PKView *parentView = dynamic_cast<PKView*>(parent);

	if(parentView == NULL)
	{
		return;
	}

    PKVariant *v  = this->get(PKSlider::VALUE_INT32_PROPERTY);
    int32_t value = PKVALUE_INT32(v);

    PKVariant *r = this->get(PKSlider::RANGE_INT32RANGE_PROPERTY);
    PKInt32Range range = PKVALUE_INT32RANGE(r);

#ifdef LINUX

    this->widget = gtk_hscale_new_with_range(range.min, range.max, 1);
    
    if(this->widget)
    {
        gtk_range_set_value(GTK_RANGE(this->widget), value);
        gtk_scale_set_draw_value(GTK_SCALE(this->widget), FALSE);
        
        if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    	{
            gtk_widget_set_size_request(this->widget, this->getW(), -1);    	    
    	}        
    	
    	// Connect
    	
        g_signal_connect(G_OBJECT(this->widget), "value-changed",
                         G_CALLBACK (PKSlider::gtk_slider_value_changed), this);      

    }

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(TRACKBAR_CLASS, L"",
							  WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS, 0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

    if(this->hwnd)
    {   
        SendMessage(this->hwnd, TBM_SETRANGE, TRUE, MAKELONG(range.min, range.max));
   		SendMessage(this->hwnd, TBM_SETPOS, TRUE, value);
    }

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OSStatus err = CreateSliderControl(window, &rect, 0, 0, 100, kControlSliderPointsDownOrRight,
									   0, true, PKSlider::SliderAction, &this->controlRef);

	if(err == noErr)
	{
		SetControlReference(this->controlRef, (SInt32) this);

        SetControlMinimum(this->controlRef, range.min);
	    SetControlMaximum(this->controlRef, range.max);

        SetControl32BitValue(this->controlRef, value);
	}

#endif
}

// From PKObject
void PKSlider::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKSlider::VALUE_INT32_PROPERTY)
    {
        PKVariant *v  = this->get(PKSlider::VALUE_INT32_PROPERTY);
        int32_t value = PKVALUE_INT32(v);

		if(value != this->getValue())
		{
#ifdef WIN32
			if(this->hwnd != NULL)
			{
    			SendMessage(this->hwnd, TBM_SETPOS, TRUE, value);
			}
#endif

#ifdef LINUX
			if(this->widget)
			{
				gtk_range_set_value(GTK_RANGE(this->widget), value);
			}
#endif

#ifdef MACOSX
	    	SetControl32BitValue(this->controlRef, value);
#endif
		}
    }

    if(prop->getName() == PKSlider::RANGE_INT32RANGE_PROPERTY)
    {
        PKVariant *r = this->get(PKSlider::RANGE_INT32RANGE_PROPERTY);
        PKInt32Range range = PKVALUE_INT32RANGE(r);

#ifdef WIN32

        if(this->hwnd != NULL)
    	{
    		SendMessage(this->hwnd, TBM_SETRANGE, TRUE, MAKELONG(range.min, range.max));
	    }
#endif

#ifdef LINUX

    if(this->widget)
    {
        gtk_range_set_range(GTK_RANGE(this->widget), range.min, range.max);
    }

#endif

#ifdef MACOSX

	    SetControlMinimum(this->controlRef, range.min);
	    SetControlMaximum(this->controlRef, range.max);

#endif
    }

    PKControl::selfPropertyChanged(prop, oldValue);
}

void PKSlider::setMinMax(int32_t min, 
						 int32_t max)
{
    PKVariantInt32Range range(min, max);
    this->set(PKSlider::RANGE_INT32RANGE_PROPERTY, &range);
}

void PKSlider::setValue(int32_t value)
{
    PKVariantInt32 v(value);
    this->set(PKSlider::VALUE_INT32_PROPERTY, &v);
}

int32_t PKSlider::getValue()
{
#ifdef WIN32
	if(this->hwnd != NULL)
	{
		return SendMessage(this->hwnd, TBM_GETPOS, 0, 0);
	}
#endif

#ifdef LINUX

    if(this->widget != NULL)
    {
        return gtk_range_get_value(GTK_RANGE(this->widget));
    }

#endif

#ifdef MACOSX

	return GetControl32BitValue(this->controlRef);

#endif

	return 0;
}

uint32_t PKSlider::getMinimumWidth()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
		return this->getW();
	}

#ifdef WIN32

	return 50;

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.right - rect.left);
#endif
}

uint32_t PKSlider::getMinimumHeight()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

#ifdef WIN32

	return 10;

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.bottom - rect.top);
#endif

}

#ifdef LINUX
    
void PKSlider::gtk_slider_value_changed(GtkRange *range,
                                        gpointer  user_data)
{
    PKSlider *slider = (PKSlider *) user_data;
    
    if(slider == NULL)
    {
        return;
    }
    
    slider->linuxValueChanged();
}
                                     
void PKSlider::linuxValueChanged()
{
    if(this->dialog != NULL)
    {
        PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
        std::string sid = PKVALUE_STRING(id);
    
		int32_t previousValue = PKVALUE_INT32(this->get(PKSlider::VALUE_INT32_PROPERTY));
		int32_t newValue = this->getCurrentValue();

		PKVariantInt32 newV(newValue);
		this->set(PKSlider::VALUE_INT32_PROPERTY, &newV);

        this->dialog->sliderChanged(sid, previousValue);
    }
}
    
#endif



