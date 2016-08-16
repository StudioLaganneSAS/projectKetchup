//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCheckBox.h"
#include "PKStr.h"
#include "PKDialog.h"

//
// Properties
//

std::string PKCheckBox::TEXT_WSTRING_PROPERTY     = "text";
std::string PKCheckBox::CHECKED_BOOL_PROPERTY     = "checked";
std::string PKCheckBox::TRANSPARENT_BOOL_PROPERTY = "transparent";


//
// PKCheckBox
//

PKCheckBox::PKCheckBox()
{
	PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
	PKOBJECT_ADD_BOOL_PROPERTY(checked, false);
    PKOBJECT_ADD_BOOL_PROPERTY(transparent, false);
}

PKCheckBox::~PKCheckBox()
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

void PKCheckBox::build()
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
	
	PKVariant *c = this->get(TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(c);

	PKVariant *ch = this->get(CHECKED_BOOL_PROPERTY);
	bool checked = PKVALUE_BOOL(ch);

#ifdef LINUX

    this->widget = gtk_check_button_new();
    
    if(this->widget)
    {
        // Set caption
        std::string caption = PKStr::wStringToUTF8string(text);
        gtk_button_set_label(GTK_BUTTON(this->widget), caption.c_str());
        
        // Set checked state
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(this->widget), checked ? TRUE : FALSE);

        // Connect        
        g_signal_connect(G_OBJECT(this->widget), "toggled", 
                         G_CALLBACK(PKCheckBox::gtk_pkcheckbox_toggled), this);
    }

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(L"BUTTON", text.c_str(),
							  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 
							  0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId,
							  hInst, 0);

	if(this->hwnd)
	{
		SendMessage(this->hwnd, WM_SETFONT, 
				   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	

		SendMessage(this->hwnd, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
	}

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OSStatus err = CreateCheckBoxControl(window, &rect, 
										 NULL, 0, true,
										 &this->controlRef);

	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);

		CFStringRef title = PKStr::wStringToCFString(text);
		SetControlTitleWithCFString(this->controlRef, title);
		CFRelease(title);	

		SetControl32BitValue(this->controlRef, checked ? 1 : 0);
	}

#endif
}

void PKCheckBox::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == TEXT_WSTRING_PROPERTY)
	{
		PKVariant *t = this->get(TEXT_WSTRING_PROPERTY);
		std::wstring text = PKVALUE_WSTRING(t);

#ifdef WIN32
		SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) text.c_str());
#endif

#ifdef LINUX
        if(this->widget)
        {
            std::string caption = PKStr::wStringToUTF8string(text);
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

	if(prop->getName() == CHECKED_BOOL_PROPERTY)
	{
		// Only update the OS control if the current
		// state of the control is different than 
		// our property or else we could be looping

		PKVariant *c = this->get(CHECKED_BOOL_PROPERTY);
		bool checked = PKVALUE_BOOL(c);

		if(checked != this->getChecked())
		{
#ifdef WIN32
			SendMessage(this->hwnd, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
#endif

#ifdef LINUX
			if(this->widget)
			{
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(this->widget), checked ? TRUE : FALSE);
			}
#endif

#ifdef MACOSX
			if(this->controlRef != NULL)
			{
				SetControl32BitValue(this->controlRef, checked ? 1 : 0);
			}
#endif
		}
	}

	PKControl::selfPropertyChanged(prop, oldValue);
}

void PKCheckBox::setText(std::wstring text)
{
	PKVariantWString newText(text);
	this->set(TEXT_WSTRING_PROPERTY, &newText);
}

void PKCheckBox::setChecked(bool checked)
{
	PKVariantBool c(checked);
	this->set(CHECKED_BOOL_PROPERTY, &c);
}

bool PKCheckBox::getChecked()
{
	PKObject *parent = this->getParent();

	if(parent == NULL)
	{
		return false;
	}

	PKView *parentView = dynamic_cast<PKView*>(parent);

	if(parentView == NULL)
	{
		return false;
	}

#ifdef WIN32

	if(this->hwnd != NULL)
	{
		DWORD result = SendMessage(this->hwnd, BM_GETCHECK, 0, 0);
		return (result == BST_CHECKED);
	}

#endif

#ifdef LINUX

    if(this->widget != NULL)
    {
        gboolean checked = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(this->widget));
        return (checked == TRUE);
    }

#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		return GetControl32BitValue(this->controlRef);
	}

#endif

	return false;
}

uint32_t PKCheckBox::getMinimumWidth()
{
	// Return accroding to our
	// current text & font

	PKVariant *t = this->get(TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);

#ifdef WIN32

	if(text == L"")
	{
		return 12;
	}

	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	HFONT old  = (HFONT) SelectObject(dc, font);

	GetTextExtentPoint32(dc, 
						 text.c_str(), 
						 text.size(), 
						 &size);

	SelectObject(dc, old);

	ReleaseDC(this->hwnd, dc);

	return (size.cx + 20);


#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.right - rect.left);
#endif

    return 0;
}

uint32_t PKCheckBox::getMinimumHeight()
{
	PKVariant *t = this->get(TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);

#ifdef WIN32

	if(text == L"")
	{
		return 12;
	}

	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	HFONT old  = (HFONT) SelectObject(dc, font);

	GetTextExtentPoint32(dc, 
						 text.c_str(), 
						 text.size(), 
						 &size);

	SelectObject(dc, old);

	ReleaseDC(this->hwnd, dc);

	return size.cy;

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

void PKCheckBox::gtk_pkcheckbox_toggled(GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    PKCheckBox *check = (PKCheckBox *) user_data;

    if(check != NULL)
    {
        check->linuxCheckAction();
    }    
}
                                                                    
void PKCheckBox::linuxCheckAction()
{
    if(this->dialog != NULL)
    {
		bool oldValue = PKVALUE_BOOL(this->get(PKCheckBox::CHECKED_BOOL_PROPERTY));

        PKVariant   *i = this->get(PKControl::ID_STRING_PROPERTY);
        std::string id = PKVALUE_STRING(i);
    
        dialog->checkBoxAction(id, oldValue);         
    }    
}

#endif 

