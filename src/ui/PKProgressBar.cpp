//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKProgressBar.h"

//
// Properties
//

std::string PKProgressBar::VALUE_INT32_PROPERTY      = "value";
std::string PKProgressBar::RANGE_INT32RANGE_PROPERTY = "range";

//
// PKProgressBar
//

PKProgressBar::PKProgressBar()
{
    PKInt32Range r = PKInt32RangeCreate(0, 100);

    PKOBJECT_ADD_INT32_PROPERTY(value, 0);
    PKOBJECT_ADD_INT32RANGE_PROPERTY(range, r);
}

PKProgressBar::~PKProgressBar()
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

void PKProgressBar::build()
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

    PKVariant *v  = this->get(PKProgressBar::VALUE_INT32_PROPERTY);
    int32_t value = PKVALUE_INT32(v);

    PKVariant *r = this->get(PKProgressBar::RANGE_INT32RANGE_PROPERTY);
    PKInt32Range range = PKVALUE_INT32RANGE(r);

#ifdef LINUX

    this->widget = gtk_progress_bar_new();
    
    if(this->widget)
    {
        if(range.max != range.min)
        {
            gdouble fraction = ((gdouble) (value - range.min)) *1.0 / (range.max - range.min);
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(this->widget), fraction);
        }
    }

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(PROGRESS_CLASS, L"",
							  WS_CHILD | WS_VISIBLE, 0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

    if(this->hwnd)
    {
    	SendMessage(this->hwnd, PBM_SETRANGE, 0, MAKELONG(range.min, range.max));
	    SendMessage(this->hwnd, PBM_SETPOS, value, 0);
    }

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OSStatus err = CreateProgressBarControl(window, &rect, 0, 0, 100, false, &this->controlRef);

	if(err == noErr)
	{
		SetControlReference(this->controlRef, (SInt32) this);
    	SetControl32BitValue(this->controlRef, value);
	    SetControlMinimum(this->controlRef, range.min);
	    SetControlMaximum(this->controlRef, range.max);
	}

#endif
}

void PKProgressBar::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKProgressBar::VALUE_INT32_PROPERTY)
    {
        PKVariant *v  = this->get(PKProgressBar::VALUE_INT32_PROPERTY);
        int32_t value = PKVALUE_INT32(v);

#ifdef WIN32

        if(this->hwnd != NULL)
	    {
		    SendMessage(this->hwnd, PBM_SETPOS, value, 0);
	    }

#endif

#ifdef LINUX
   
    if(this->widget)
    {
        PKVariant *r = this->get(PKProgressBar::RANGE_INT32RANGE_PROPERTY);
        PKInt32Range range = PKVALUE_INT32RANGE(r);

        if(range.max != range.min)
        {
            gdouble fraction = ((gdouble) (value - range.min)) *1.0 / (range.max - range.min);
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(this->widget), fraction);
        }
    }

#endif


#ifdef MACOSX

    	SetControl32BitValue(this->controlRef, value);

#endif
    }

    if(prop->getName() == PKProgressBar::RANGE_INT32RANGE_PROPERTY)
    {
        PKVariant *r = this->get(PKProgressBar::RANGE_INT32RANGE_PROPERTY);
        PKInt32Range range = PKVALUE_INT32RANGE(r);

#ifdef WIN32

        if(this->hwnd != NULL)
    	{
	    	SendMessage(this->hwnd, PBM_SETRANGE, 0, MAKELONG(range.min, range.max));
	    }
#endif

#ifdef LINUX
        this->invalidate();
#endif

#ifdef MACOSX

	    SetControlMinimum(this->controlRef, range.min);
	    SetControlMaximum(this->controlRef, range.max);

#endif
    }

    PKControl::selfPropertyChanged(prop, oldValue);
}

void PKProgressBar::setMinMax(int32_t min, 
							  int32_t max)
{
    PKVariantInt32Range range(min, max);
    this->set(PKProgressBar::RANGE_INT32RANGE_PROPERTY, &range);
}

void PKProgressBar::setValue(int32_t value)
{
    PKVariantInt32 v(value);
    this->set(PKProgressBar::VALUE_INT32_PROPERTY, &v);
}

int32_t PKProgressBar::getValue()
{
#ifdef WIN32
	if(this->hwnd != NULL)
	{
		return SendMessage(this->hwnd, PBM_GETPOS, 0, 0);
	}
#endif

#ifdef LINUX
    
    PKVariant *r = this->get(PKProgressBar::RANGE_INT32RANGE_PROPERTY);
    PKInt32Range range = PKVALUE_INT32RANGE(r);

    if(this->widget != NULL)
    {
        gdouble fraction = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(this->widget));
        return range.min + fraction*(range.max - range.min);
    }

#endif

#ifdef MACOSX
	return GetControl32BitValue(this->controlRef);
#endif

	return 0;
}

uint32_t PKProgressBar::getMinimumWidth()
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

uint32_t PKProgressBar::getMinimumHeight()
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
