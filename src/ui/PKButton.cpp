//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKButton.h"
#include "PKDialog.h"
#include "PKStr.h"

#ifdef WIN32
#ifndef BCM_GETIDEALSIZE 
#define BCM_GETIDEALSIZE 0x1601
#endif
#endif 

//
// Properties
//

std::string PKButton::CAPTION_WSTRING_PROPERTY        = "caption";
std::string PKButton::DEFAULT_BOOL_PROPERTY           = "default";
std::string PKButton::OVERRIDE_MIN_SIZE_BOOL_PROPERTY = "overrideMinSize";

//
// PKButton
//

PKButton::PKButton()
{
	PKOBJECT_ADD_WSTRING_PROPERTY(caption, L"NOT_SET");
    PKOBJECT_ADD_BOOL_PROPERTY(default, false);
    PKOBJECT_ADD_BOOL_PROPERTY(overrideMinSize, false);
}

PKButton::~PKButton()
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

void PKButton::build()
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
	
	PKVariant *c = this->get(CAPTION_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(c);

	PKVariant *d  = this->get(DEFAULT_BOOL_PROPERTY);
	bool bDefault = PKVALUE_BOOL(d);

#ifdef LINUX

    this->widget = gtk_button_new();
    
    if(this->widget != NULL)
    {
        // Set caption
    
        std::string caption = PKStr::wStringToUTF8string(text);
        
        if(caption.size() < 8)
        {
            int diff = 8 - caption.size();
            caption = std::string(diff, ' ') + caption + std::string(diff, ' ');
        }

        gtk_button_set_label(GTK_BUTTON(this->widget), caption.c_str());
        
        // Connect
        
        g_signal_connect(G_OBJECT(this->widget), "clicked", 
                         G_CALLBACK(PKButton::gtk_pkbutton_clicked), this);
    }

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;

    if(bDefault)
    {
        style |= BS_DEFPUSHBUTTON;
    }

	this->hwnd = CreateWindow(L"BUTTON", text.c_str(),
							  style, 
							  0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

	if(this->hwnd)
	{
		SendMessage(this->hwnd, WM_SETFONT, 
				   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	
	}

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OSStatus err = CreatePushButtonControl(window, &rect, NULL, &this->controlRef);

	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);

		CFStringRef title = PKStr::wStringToCFString(text);
		SetControlTitleWithCFString(this->controlRef, title);
		CFRelease(title);	

		if(bDefault)
		{
			SetWindowDefaultButton(GetControlOwner(this->controlRef), this->controlRef);		
		}
	}
	
#endif
}

void PKButton::initialize(PKDialog *dialog)
{
    PKControl::initialize(dialog);

#ifdef LINUX

	PKVariant *d  = this->get(DEFAULT_BOOL_PROPERTY);
	bool bDefault = PKVALUE_BOOL(d);
    
    if(this->widget != NULL)
    {
        if(bDefault)
        {
            GtkWindow *window = GTK_WINDOW(dialog->getWindowHandle());
            GTK_WIDGET_SET_FLAGS(this->widget, GTK_CAN_DEFAULT);
            gtk_window_set_default(window, this->widget);
            gtk_widget_grab_focus(this->widget);
        }
    }

#endif
}


void PKButton::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == CAPTION_WSTRING_PROPERTY)
	{
		PKVariant *c = this->get(CAPTION_WSTRING_PROPERTY);
		std::wstring text = PKVALUE_WSTRING(c);

#ifdef WIN32
	
		if(this->hwnd != NULL)
		{
			SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) text.c_str());
		}
	
#endif

#ifdef LINUX
    if(this->widget != NULL)
    {
        // Set caption
    
        std::string caption = PKStr::wStringToUTF8string(text);
        
        if(caption.size() < 8)
        {
            int diff = 8 - caption.size();
            caption = std::string(diff, ' ') + caption + std::string(diff, ' ');
        }
        
        gtk_button_set_label(GTK_BUTTON(this->widget), caption.c_str());
    }
#endif

#ifdef MACOSX

		if(this->controlRef != NULL)
		{
			CFStringRef title = PKStr::wStringToCFString(text);
			SetControlTitleWithCFString(this->controlRef, title);
			CFRelease(title);	
		}
	
#endif
	}

	if(prop->getName() == DEFAULT_BOOL_PROPERTY)
	{
        PKVariant *def = this->get(PKButton::DEFAULT_BOOL_PROPERTY);
        bool defaultB  = PKVALUE_BOOL(def);

#ifdef WIN32
        DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;

        if(defaultB)
        {
            style |= BS_DEFPUSHBUTTON;
        }

        if(this->hwnd != NULL)
        {
            SetWindowLongPtr(this->hwnd, GWL_STYLE, style);
        }
#endif

#ifdef LINUX
        if(defaultB && this->dialog)
        {
            GtkWindow *window = GTK_WINDOW(this->dialog->getWindowHandle());
            GTK_WIDGET_SET_FLAGS(this->widget, GTK_CAN_DEFAULT);
            gtk_window_set_default(window, this->widget);
            gtk_widget_grab_focus(this->widget);
        }
#endif

#ifdef MACOSX

		if(defaultB && this->controlRef)
		{
			SetWindowDefaultButton(GetControlOwner(this->controlRef), this->controlRef);		
		}

#endif
    }

	PKControl::selfPropertyChanged(prop, oldValue);
}


void PKButton::setCaption(std::wstring text)
{
	PKVariantWString caption(text);
	this->set(CAPTION_WSTRING_PROPERTY, &caption);
}

uint32_t PKButton::getMinimumWidth()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
		return this->getW();
	}
	
	PKVariant *c = this->get(CAPTION_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(c);

	PKVariant *m = this->get(PKButton::OVERRIDE_MIN_SIZE_BOOL_PROPERTY);
    bool overrideW = PKVALUE_BOOL(m);

#ifdef WIN32
	SIZE size;
	size.cx = size.cy = 0;

	SendMessage(this->hwnd, BCM_GETIDEALSIZE, 0, (LPARAM)(LPVOID) &size);

	if(size.cx == 0)
	{
		// Use another method

		HDC dc = GetDC(this->hwnd);
	
		HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
		HFONT old  = (HFONT) SelectObject(dc, font);

		GetTextExtentPoint32(dc, 
							 text.c_str(), 
							 text.size(), 
							 &size);
	
		SelectObject(dc, old);

		ReleaseDC(this->hwnd, dc);
	}

	if(overrideW)
	{
		return 0;
	}

	return pk_max(size.cx + 16, 70);
#endif

#ifdef MACOSX
	
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);

	if(overrideW)
	{
		return 0;
	}

	return (rect.right - rect.left);

#endif

    return 0;
}

uint32_t PKButton::getMinimumHeight()
{
	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

	PKVariant *c = this->get(CAPTION_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(c);

	// Return accroding to our
	// current text & font

#ifdef WIN32
	SIZE size;
	size.cx = size.cy = 0;

	SendMessage(this->hwnd, BCM_GETIDEALSIZE, 0, (LPARAM)(LPVOID) &size);

	if(size.cy == 0)
	{
		// Use another method

		HDC dc = GetDC(this->hwnd);
	
		HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
		HFONT old  = (HFONT) SelectObject(dc, font);

		GetTextExtentPoint32(dc, 
							 text.c_str(), 
							 text.size(), 
							 &size);
	
		SelectObject(dc, old);

		ReleaseDC(this->hwnd, dc);
		size.cy += 10;
	}

	return (size.cy + 4);
#endif

#ifdef MACOSX
	
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.bottom - rect.top);

#endif

    return 0;
}

#ifdef LINUX

void PKButton::gtk_pkbutton_clicked(GtkButton *b,
                                    gpointer   user_data)
{
    PKButton *button = (PKButton *) user_data;
    
    if(button != NULL)
    {
        button->linuxButtonClicked();   
    }    
}                     
                          
void PKButton::linuxButtonClicked()
{
    if(this->dialog != NULL)
    {
        PKVariant   *i = this->get(PKControl::ID_STRING_PROPERTY);
        std::string id = PKVALUE_STRING(i);
    
        dialog->buttonClicked(id);
    }    
}

#endif

