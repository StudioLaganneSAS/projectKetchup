//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKLittleArrows.h"
#include "PKDialog.h"
#include "PKStr.h"
#include "PKEdit.h"

//
// Properties
//

std::string PKLittleArrows::RANGE_INT32RANGE_PROPERTY = "range";

//
// PKLittleArrows
//

PKLittleArrows::PKLittleArrows()
{
    PKInt32Range r = PKInt32RangeCreate(0, 100);
    PKOBJECT_ADD_INT32RANGE_PROPERTY(range, r);
}

PKLittleArrows::~PKLittleArrows()
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

void PKLittleArrows::build()
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
	
    PKVariant *r = this->get(PKLittleArrows::RANGE_INT32RANGE_PROPERTY);
    PKInt32Range range = PKVALUE_INT32RANGE(r);

#ifdef LINUX

	// TODO

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(UPDOWN_CLASS, L"",
							  WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT, 0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

	if(this->hwnd != NULL)
	{
		SendMessage(this->hwnd, UDM_SETRANGE, 0, MAKELONG(range.max, range.min));
	}

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OSStatus err = CreateLittleArrowsControl(window, &rect, 0, 0, 100, 1, &this->controlRef);

	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);
		
		SetControlMinimum(this->controlRef, range.min);
		SetControlMaximum(this->controlRef, range.max);
	}
	
	this->buddy = NULL;

#endif
}

void PKLittleArrows::initialize(PKDialog *dialog)
{
    PKControl::initialize(dialog);

#ifdef LINUX

	// TODO
	
#endif
}

void PKLittleArrows::setBuddyControl(PKControl *buddy)
{
#ifdef WIN32
	if(this->hwnd != NULL)
	{
		SendMessage(this->hwnd, UDM_SETBUDDY, (WPARAM) buddy->getWindowHandle(), 0);
	}
#endif

#ifdef MACOSX
	this->buddy = buddy;
#endif
}


void PKLittleArrows::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKLittleArrows::RANGE_INT32RANGE_PROPERTY)
	{
		PKVariant *r = this->get(PKLittleArrows::RANGE_INT32RANGE_PROPERTY);
	    PKInt32Range range = PKVALUE_INT32RANGE(r);

#ifdef WIN32
		if(this->hwnd != NULL)
		{
			SendMessage(this->hwnd, UDM_SETRANGE, 0, MAKELONG(range.max, range.min));
		}
#endif

#ifdef MACOSX
		if(this->controlRef)
		{
			SetControlMinimum(this->controlRef, range.min);
			SetControlMaximum(this->controlRef, range.max);
		}
#endif
	}

	PKControl::selfPropertyChanged(prop, oldValue);
}

#ifdef MACOSX

void PKLittleArrows::handleValueUp()
{
	int max = GetControlMaximum(this->controlRef);

	if(this->buddy != NULL)
	{
		PKEdit *edit = dynamic_cast<PKEdit *> (this->buddy);
		
		if(edit)
		{
			int32_t value = PKStr::wstringToInt32(edit->getText());

			if(value < max)
			{
				value++;
				edit->setText(PKStr::int32ToWstring(value));
				edit->doSelectAll();

				// trigger a value changed event
				PKSafeEvents::postSafeEvent(edit, "VALUE_CHANGED", NULL);			
			}
		}
	}
}

void PKLittleArrows::handleValueDown()
{
	int min = GetControlMinimum(this->controlRef);

	if(this->buddy != NULL)
	{
		PKEdit *edit = dynamic_cast<PKEdit *> (this->buddy);

		if(edit)
		{
			int32_t value = PKStr::wstringToInt32(edit->getText());

			if(value > min)
			{
				value--;
				edit->setText(PKStr::int32ToWstring(value));
				edit->doSelectAll();

				// trigger a value changed event
				PKSafeEvents::postSafeEvent(edit, "VALUE_CHANGED", NULL);			
			}
		}
	}
}

#endif


uint32_t PKLittleArrows::getMinimumWidth()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
		return this->getW();
	}
	
#ifdef WIN32

	return 16;

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.right - rect.left);
#endif

    return 0;
}

uint32_t PKLittleArrows::getMinimumHeight()
{
	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

	// Return accroding to our
	// current text & font

#ifdef WIN32

	return 20;

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.bottom - rect.top);
#endif

    return 0;
}
