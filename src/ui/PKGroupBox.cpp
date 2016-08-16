//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//


#include "PKGroupBox.h"
#include "PKLayout.h"
#include "PKStr.h"

//
// Properties
//

std::string PKGroupBox::CAPTION_WSTRING_PROPERTY = "caption";
std::string PKGroupBox::MARGIN_INT32_PROPERTY    = "margin";
std::string PKGroupBox::TOPMARGIN_INT32_PROPERTY = "topMargin";
std::string PKGroupBox::CHECKBOX_BOOL_PROPERTY   = "checkbox";
std::string PKGroupBox::CHECKED_BOOL_PROPERTY    = "checked";

//
// WIN32 stuff
//

#ifdef WIN32

#include <windowsx.h>

//
// This is the dialog window proc
// we override the original window
// proc from DWPWindow to be able to
// catch WM_COMMAND messages and then
// we call the original proc
//

LRESULT CALLBACK PKGroupBox::GroupWinProc(HWND hWnd, 
										  UINT msg, 
										  WPARAM wParam, 
										  LPARAM lParam)
{
	PKGroupBox *group = (PKGroupBox *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(group == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	PKObject *parent = group->getParent();

	if(parent == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	PKView *parentView = dynamic_cast<PKView*>(parent);

	if(parentView == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

    HWND ghWnd = parentView->getWindowHandle();
/*
    switch(msg)
    {
    case WM_ERASEBKGND:
        {
            HBRUSH  hBrush, hOldBrush;
            HPEN    hPen, hOldPen;
            RECT    rect;
            HDC     hDC;

            hDC = GetDC(hWnd);

            // Obtain a handle to the parent window's background brush.
            hBrush = (HBRUSH) CreateSolidBrush(GetSysColor(COLOR_3DFACE));
            hOldBrush = (HBRUSH) SelectObject(hDC, hBrush);

            // Create a background-colored pen to draw the rectangle
            // borders, where gWindowColor is some globally defined
            // COLORREF variable used to paint the window's background
            hPen = (HPEN) GetStockObject(NULL_PEN);
            hOldPen = (HPEN) SelectObject(hDC, hPen);

            // Erase the group box's background.
            GetClientRect(hWnd, &rect);
            Rectangle(hDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);

            // Restore the original objects before releasing the DC.
            SelectObject(hDC, hOldPen);
            SelectObject(hDC, hOldBrush);

            // Delete the created object.
            DeleteObject(hPen);

            ReleaseDC(hWnd, hDC);

            // Instruct Windows to paint the group box text and frame.
            InvalidateRect(hWnd, NULL, FALSE);

            // Insert code here to instruct the contents of the group box
            // to repaint as well.

            return TRUE; // Background has been erased.
        }
    }
*/
   	return group->lpOldProc(hWnd, msg, wParam, lParam);
}

#endif

//
// PKGroupBox
//

PKGroupBox::PKGroupBox()
{
#ifdef WIN32
    this->checkbox = NULL;
#endif
    
    PKOBJECT_ADD_WSTRING_PROPERTY(caption, L"");
	PKOBJECT_ADD_INT32_PROPERTY(margin, 10);
	PKOBJECT_ADD_INT32_PROPERTY(topMargin, 14);
	PKOBJECT_ADD_BOOL_PROPERTY(checkbox, false);
	PKOBJECT_ADD_BOOL_PROPERTY(checked, false);
}

PKGroupBox::~PKGroupBox()
{
#ifdef WIN32

	SubclassWindow(this->hwnd, (WNDPROC) this->lpOldProc);

    if(this->hwnd != NULL)
	{
		DestroyWindow(this->hwnd);
	}

	if(this->checkbox != NULL)
	{
		DestroyWindow(this->checkbox);
	}

#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		DisposeControl(this->controlRef);
	}

#endif
}

PKWindowHandle PKGroupBox::getWindowHandle()
{
#ifdef LINUX
    return this->widget;
#endif

	// We forward to our parent

	PKObject *parent = this->getParent();

	if(parent)
	{
		PKView *parentView = dynamic_cast<PKView*>(parent);

		if(parentView)
		{
			return parentView->getWindowHandle();
		}
	}

	return NULL;
}

void PKGroupBox::build()
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

	PKVariant *cap = this->get(CAPTION_WSTRING_PROPERTY);
	std::wstring caption = PKVALUE_WSTRING(cap);

	PKVariant *c = this->get(PKGroupBox::CHECKED_BOOL_PROPERTY);
	bool checked = PKVALUE_BOOL(c);

#ifdef LINUX

    std::string cpt = PKStr::wStringToUTF8string(caption);

    this->widget = gtk_frame_new(cpt.c_str());

    if(this->widget != NULL)
	{
	    // Setup ?
	}    

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(L"BUTTON", caption.c_str(),
							  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_GROUPBOX, 
							  0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) 0xF000 + this->controlId, 
							  hInst, 0);

	if(this->hwnd)
	{
		SendMessage(this->hwnd, WM_SETFONT, 
				   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	
	
    }

    // Install the new window proc

    this->lpOldProc = SubclassWindow(this->hwnd, (WNDPROC) GroupWinProc);
	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID) (PKGroupBox *) this);

    // Checkbox control

	this->checkbox = NULL;
	this->checkw   = 0;
	this->checkh   = 0;

	PKVariant *check = this->get(PKGroupBox::CHECKBOX_BOOL_PROPERTY);
	bool checkbox = PKVALUE_BOOL(check);

	if(checkbox)
	{
		this->checkbox = CreateWindow(L"BUTTON", caption.c_str(),
								  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 
								  0, 0, 100, 80,
								  parentView->getWindowHandle(), 
								  (HMENU) this->controlId, 
								  hInst, 0);

		if(this->checkbox)
		{
			SendMessage(this->checkbox, WM_SETFONT, 
					   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	

			SendMessage(this->checkbox, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
		}
	}

	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	HFONT old  = (HFONT) SelectObject(dc, font);

	GetTextExtentPoint32(dc, 
						 L"HELLO WORLD", 
						 5, 
						 &size);

	SelectObject(dc, old);

	ReleaseDC(this->hwnd, dc);

	PKVariantInt32 top(8 + size.cy);
	this->set(PKGroupBox::TOPMARGIN_INT32_PROPERTY, &top);

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OSStatus err;
	
	PKVariant *check = this->get(PKGroupBox::CHECKBOX_BOOL_PROPERTY);
	bool checkbox = PKVALUE_BOOL(check);

	if(checkbox)
	{
		err = CreateCheckGroupBoxControl(window, &rect, NULL, 0, false, true, &this->controlRef);
		PKVariantInt32 top(30);
		this->set(PKGroupBox::TOPMARGIN_INT32_PROPERTY, &top);
	}
	else
	{
		err = CreateGroupBoxControl(window, &rect, NULL, true, &this->controlRef);
		PKVariantInt32 top(20);
		this->set(PKGroupBox::TOPMARGIN_INT32_PROPERTY, &top);
	}
	
	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);
		SetControl32BitValue(this->controlRef, checked ? 1 : 0);

		CFStringRef title = PKStr::wStringToCFString(caption);
		SetControlTitleWithCFString(this->controlRef, title);
		CFRelease(title);	
	}
	
#endif

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->build();
		
#ifdef LINUX

            if(this->widget != NULL)
		    {
                gtk_container_add(GTK_CONTAINER(this->widget), 
                                  root->getWindowHandle());                
		    }

#endif
		
		}
	}
}

void PKGroupBox::initialize(PKDialog *dialog)
{
	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->initialize(dialog);
		}
	}
	
	PKControl::initialize(dialog);
}

void PKGroupBox::destroy()
{
	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->destroy();
            root = NULL;
		}
	}
    this->clearChildren();

    PKControl::destroy();
}


//
// From PKObject
//

void PKGroupBox::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKGroupBox::CAPTION_WSTRING_PROPERTY)
	{
		PKVariant *caption = this->get(PKGroupBox::CAPTION_WSTRING_PROPERTY);
		std::wstring text  = PKVALUE_WSTRING(caption);

#ifdef WIN32

		if(this->checkbox == NULL)
		{
			// Set the title of the group box
			SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) text.c_str());
		}
		else
		{
			// Set our title to "" and set the text on the checkbox
			SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "");
			SendMessage(this->checkbox, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) text.c_str());

			// Compute the check box size

			SIZE size;

			HDC dc = GetDC(this->checkbox);
			
			HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
			HFONT old  = (HFONT) SelectObject(dc, font);

			GetTextExtentPoint32(dc, 
								 text.c_str(), 
								 text.size(), 
								 &size);

			SelectObject(dc, old);

			ReleaseDC(this->checkbox, dc);

			this->checkw = size.cx + 20;
			this->checkh = size.cy;
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
	
	if(prop->getName() == PKGroupBox::CHECKED_BOOL_PROPERTY)
	{
		PKVariant *c = this->get(PKGroupBox::CHECKED_BOOL_PROPERTY);
		bool checked = PKVALUE_BOOL(c);

#ifdef WIN32

		if(this->checkbox != NULL)
		{
			SendMessage(this->checkbox, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
		}

#endif

#ifdef MACOSX

		if(this->controlRef != NULL)
		{
			SetControl32BitValue(this->controlRef, checked ? 1 : 0);
		}

#endif
	}

	PKControl::selfPropertyChanged(prop, oldValue);
}


void PKGroupBox::setCaption(std::wstring cap)
{
	PKVariantWString caption(cap);
	this->set(PKGroupBox::CAPTION_WSTRING_PROPERTY, &caption);
}

void PKGroupBox::setChecked(bool checked)
{
	PKVariantBool c(checked);
	this->set(PKGroupBox::CHECKED_BOOL_PROPERTY, &c);
}

bool PKGroupBox::getChecked()
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

	if(this->checkbox != NULL)
	{
		DWORD result = SendMessage(this->checkbox, BM_GETCHECK, 0, 0);
		return (result == BST_CHECKED);
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

void PKGroupBox::move(int32_t x, 
					  int32_t y)
{
	PKVariant *m = this->get(PKGroupBox::MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *tm = this->get(PKGroupBox::TOPMARGIN_INT32_PROPERTY);
	int32_t topMargin = PKVALUE_INT32(tm);

	PKVariant *caption = this->get(PKGroupBox::CAPTION_WSTRING_PROPERTY);
	std::wstring text  = PKVALUE_WSTRING(caption);

	PKControl::move(x, y);

#ifdef WIN32

	if(this->checkbox != NULL)
	{
		MoveWindow(this->checkbox, 
				   x+10, 
				   y, 
				   this->checkw, 
				   this->checkh, FALSE);
	}

#endif

	if(this->getChildrenCount() == 1)
	{
		PKLayout *rootLayout = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(rootLayout)
		{
			rootLayout->move(x + margin,
					         y + (text != L"" ? topMargin : margin));
		}
        else
        {
		    PKControl *ctrl = dynamic_cast<PKControl*>(this->getChildAt(0));

            if(ctrl)
            {
    			ctrl->move(x + margin,
	    				   y + (text != L"" ? topMargin : margin));
            }
        }
	}
}

void PKGroupBox::resize(uint32_t w, 
					    uint32_t h)
{
	PKVariant *m = this->get(PKGroupBox::MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *tm = this->get(PKGroupBox::TOPMARGIN_INT32_PROPERTY);
	int32_t topMargin = PKVALUE_INT32(tm);

	PKVariant *caption = this->get(PKGroupBox::CAPTION_WSTRING_PROPERTY);
	std::wstring text  = PKVALUE_WSTRING(caption);

	PKControl::resize(w, h);

#ifdef WIN32

	if(this->checkbox != NULL)
	{
		MoveWindow(this->checkbox, 
				   this->getX()+10, 
				   this->getY(), 
				   this->checkw, 
				   this->checkh, FALSE);
	}

#endif

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->resize(w - 2*margin,
						 h - margin - (text != L"" ? topMargin : margin));
		}
	}
}

void PKGroupBox::show()
{
	PKControl::show();

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->show();
		}
	}

#ifdef WIN32
	if(this->checkbox != NULL)
	{
		ShowWindow(this->checkbox, SW_SHOWNORMAL);
	}
#endif
}

void PKGroupBox::hide()
{
	PKControl::hide();

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->hide();			
		}
	}

#ifdef WIN32
	if(this->checkbox != NULL)
	{
		ShowWindow(this->checkbox, SW_HIDE);
	}
#endif
}

void PKGroupBox::enable()
{
	PKControl::enable();

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->enable();
		}
	}

#ifdef WIN32
	if(this->checkbox != NULL)
	{
		EnableWindow(this->checkbox, TRUE);
	}
#endif
}

void PKGroupBox::disable()
{
	PKControl::disable();

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->disable();
		}
	}

#ifdef WIN32
	if(this->checkbox != NULL)
	{
		EnableWindow(this->checkbox, FALSE);
	}
#endif
}

uint32_t PKGroupBox::getMinimumWidth()
{
	PKVariant *m = this->get(PKGroupBox::MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumWidth() + 2*margin);
		}
	}

	return 0;
}

uint32_t PKGroupBox::getMinimumHeight()
{
	PKVariant *m = this->get(PKGroupBox::MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *tm = this->get(PKGroupBox::TOPMARGIN_INT32_PROPERTY);
	int32_t topMargin = PKVALUE_INT32(tm);

	PKVariant *caption = this->get(PKGroupBox::CAPTION_WSTRING_PROPERTY);
	std::wstring text  = PKVALUE_WSTRING(caption);

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumHeight() + margin + (text != L"" ? topMargin : margin));
		}
	}

	return 0;

}
