//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKEdit.h"
#include "PKStr.h"
#include "PKDialog.h"

#ifdef WIN32
#include <windowsx.h>
#endif

#ifdef MACOSX

//
// Key/Text filters for edit boxes
//

pascal void ControlTextValidationProc(ControlRef controlRef)
{
	SInt32 value  = GetControlReference(controlRef);
	PKEdit *edit  = (PKEdit *) value;
	
	// Fire notification
	
	PKSafeEvents::postSafeEvent(edit, "VALUE_CHANGED", NULL);
}

pascal ControlKeyFilterResult ControlKeyFilterProcNum(ControlHandle theControl,
													  SInt16* keyCode,
													  SInt16* charCode,
													  EventModifiers* modifiers)
{
	// Get handle to control
	
	SInt32 value  = GetControlReference(theControl);
	PKEdit *edit  = (PKEdit *) value;
	
	// Fire notification

	PKSafeEvents::postSafeEvent(edit, "VALUE_CHANGED", NULL);
	
	// Get control type
	
	PKVariant *t = edit->get(PKEdit::TYPE_INT32_PROPERTY);
	int32_t type = PKVALUE_INT32(t);
	
	if(type != PK_EDIT_TYPE_NUMBER)
	{
		return kControlKeyFilterPassKey;
	}
	
	if(((char)*charCode >= '0') && ((char)*charCode <= '9'))
	{
		return kControlKeyFilterPassKey;
	}
	
	switch ( *charCode )
	{
    case kLeftArrowCharCode:
    case kRightArrowCharCode:
    case kUpArrowCharCode:
    case kDownArrowCharCode:
    case kBackspaceCharCode:
    case kDeleteCharCode:
      return kControlKeyFilterPassKey;

    default:
      return kControlKeyFilterBlockKey;
	}
}

#endif

//
// Properties
//

std::string PKEdit::TEXT_WSTRING_PROPERTY   = "text";
std::string PKEdit::TYPE_INT32_PROPERTY     = "type";
std::string PKEdit::MULTILINE_BOOL_PROPERTY = "multiline";
std::string PKEdit::VSCROLL_BOOL_PROPERTY   = "vScroll";
std::string PKEdit::HSCROLL_BOOL_PROPERTY   = "hScroll";
std::string PKEdit::BORDER_BOOL_PROPERTY    = "border";

//
// PKEdit
//

PKEdit::PKEdit()
{
    PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
    PKOBJECT_ADD_INT32_PROPERTY(type, PK_EDIT_TYPE_TEXT);
    PKOBJECT_ADD_BOOL_PROPERTY(multiline, false);
    PKOBJECT_ADD_BOOL_PROPERTY(vScroll, false);
    PKOBJECT_ADD_BOOL_PROPERTY(hScroll, false);
    PKOBJECT_ADD_BOOL_PROPERTY(border, true);
}

PKEdit::~PKEdit()
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

void PKEdit::build()
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

    PKVariant *t = this->get(PKEdit::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

    this->_startText = text;

	PKVariant *tp = this->get(PKEdit::TYPE_INT32_PROPERTY);
	int32_t type = PKVALUE_INT32(tp);

    PKVariant *m = this->get(PKEdit::MULTILINE_BOOL_PROPERTY);
    bool multiline = PKVALUE_BOOL(m);

    PKVariant *vs = this->get(PKEdit::VSCROLL_BOOL_PROPERTY);
    bool vScroll = PKVALUE_BOOL(vs);

    PKVariant *hs = this->get(PKEdit::HSCROLL_BOOL_PROPERTY);
    bool hScroll = PKVALUE_BOOL(hs);

    PKVariant *b = this->get(PKEdit::BORDER_BOOL_PROPERTY);
    bool border = PKVALUE_BOOL(b);

#ifdef LINUX

    this->widget = gtk_entry_new();
    
    if(this->widget != NULL)
    {
        // Set text
        std::string caption = PKStr::wStringToUTF8string(text);
        gtk_entry_set_text(GTK_ENTRY(this->widget), caption.c_str());
        
        // TODO: other properties

        // Connect

        g_signal_connect(G_OBJECT(this->widget), "changed",
                         G_CALLBACK (PKEdit::gtk_entry_text_changed_event), this);      
                         
        // Setup size
        
        if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    	{
            gtk_widget_set_size_request(this->widget, this->getW(), -1);    	    
    	}        
    }

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	DWORD flags = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_AUTOVSCROLL;

	if(type == PK_EDIT_TYPE_PASSWORD)
	{
		flags |= ES_PASSWORD;
	}

	if(type == PK_EDIT_TYPE_NUMBER)
	{
		flags |= ES_NUMBER;
	}

    if(multiline)
    {
        flags |= (ES_MULTILINE | ES_WANTRETURN);
    }

    if(vScroll)
    {
        flags |= WS_VSCROLL;
    }

    if(hScroll)
    {
        flags |= WS_HSCROLL;
    }

    this->hwnd = CreateWindowEx((border ? WS_EX_CLIENTEDGE : 0),
								L"EDIT", text.c_str(),
							    flags, 
							    0, 0, 100, 15,
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

	bool password = false;
	
	if(type == PK_EDIT_TYPE_PASSWORD)
	{
		password = true;
	}
	
	OSStatus err = CreateEditUnicodeTextControl(window, &rect, NULL, 
												password, NULL, &this->controlRef);

	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);
		
		// Setup the proc for value changed

		ControlKeyFilterUPP myControlKeyFilterUPP; 
		myControlKeyFilterUPP = NewControlKeyFilterUPP(ControlKeyFilterProcNum);
		
		SetControlData(this->controlRef, 
					   0, kControlKeyFilterTag, 
					   sizeof(myControlKeyFilterUPP), 
					   (Ptr) &myControlKeyFilterUPP);			
		
		
		ControlEditTextValidationUPP myControlTextValidationUPP;
		myControlTextValidationUPP = NewControlEditTextValidationUPP(ControlTextValidationProc);
		
		SetControlData(this->controlRef, 
					   0, kControlEditTextValidationProcTag, 
					   sizeof(myControlTextValidationUPP), 
					   (Ptr) &myControlTextValidationUPP);			
		
		
		SetControlReference(this->controlRef, (SInt32) this);

		// Setup text
		
		CFStringRef title = PKStr::wStringToCFString(text);
		
		SetControlData(this->controlRef,
					   kControlEntireControl,
					   kControlEditTextCFStringTag,
					   sizeof(title), &title);
		
		CFRelease(title);			
	}

#endif
}

// From PKControl

bool PKEdit::hasSelection()
{
#ifdef WIN32
    if(this->hwnd)
    {
        DWORD start = 0;
        DWORD end   = 0;
        SendMessage(this->hwnd, EM_GETSEL, (WPARAM) &start, (LPARAM) &end);

        return ((end - start) > 0);
    }
#endif

#ifdef LINUX
    if(this->widget)
    {
        gint start = 0;
        gint end   = 0;

        gtk_editable_get_selection_bounds(GTK_EDITABLE(this->widget), &start, &end);

        return ((end - start) > 0);
    }
#endif

#ifdef MACOSX
	if(this->controlRef)
	{
		Size theSize = 0; 
		ControlEditTextSelectionRec sel;
		
		GetControlData(controlRef, kControlEditTextPart, 
					   kControlEditTextSelectionTag, 
					   sizeof(sel), &sel, 
					   &theSize); 

		return ((sel.selEnd - sel.selStart) > 0);
	}

#endif

	return false;
}

// Notification

void PKEdit::processSafeEvent(std::string  eventName,
							  void        *payload)
{
	if(eventName == "VALUE_CHANGED")
	{			
#ifdef MACOSX	

		CFStringRef title = NULL;
		
		GetControlData(this->controlRef, 
					   kControlEntireControl, 
					   kControlEditTextCFStringTag,
					   sizeof(title), &title, NULL);
		
		std::wstring str = PKStr::cfStringToWString(title);
		
		if(title)
		{
			CFRelease(title);
		}
		
		PKVariantWString te(str);
		this->set(PKEdit::TEXT_WSTRING_PROPERTY, &te);
		
		if(this->dialog)
		{
			PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
			std::string eid = PKVALUE_STRING(id);
			
			this->dialog->editTextChanged(eid, str);
		}
	
#endif
	}
	
	PKControl::processSafeEvent(eventName, payload);
}

// From PKObject

void PKEdit::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKEdit::TEXT_WSTRING_PROPERTY)
    {		
        PKVariant *t = this->get(PKEdit::TEXT_WSTRING_PROPERTY);
        std::wstring text = PKVALUE_WSTRING(t);
		
		if(text != this->getText())
		{
#ifdef WIN32
	        SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) text.c_str());
#endif

#ifdef LINUX
			if(this->widget != NULL)
			{
				// Set text
				std::string caption = PKStr::wStringToUTF8string(text);
				gtk_entry_set_text(GTK_ENTRY(this->widget), caption.c_str());
			}
#endif

#ifdef MACOSX
			if(this->controlRef != NULL)
			{
				CFStringRef title = PKStr::wStringToCFString(text);

				SetControlData(this->controlRef,
							   kControlEntireControl,
							   kControlEditTextCFStringTag,
							   sizeof(title), &title);

				CFRelease(title);	
			}
#endif
		}
    }

    PKControl::selfPropertyChanged(prop, oldValue);
}

void PKEdit::setText(std::wstring text)
{
    PKVariantWString val(text);
    this->set(PKEdit::TEXT_WSTRING_PROPERTY, &val);
}

std::wstring PKEdit::getText()
{
#ifdef WIN32
	
	TCHAR buffer[16384];
	GetWindowText(this->hwnd, buffer, 16384);

	std::wstring text = buffer;
	return text;

#endif

#ifdef LINUX

    if(this->widget != NULL)
    {
        const gchar *text = gtk_entry_get_text(GTK_ENTRY(this->widget));
        return PKStr::utf8StringToWstring((char *) text);
    }

#endif

#ifdef MACOSX

	if(this->controlRef == NULL)
	{
		return L"";
	}

	CFStringRef title = NULL;

	GetControlData(this->controlRef, 
				   kControlEntireControl, 
				   kControlEditTextCFStringTag,
				   sizeof(title), &title, NULL);
				   
	std::wstring str = PKStr::cfStringToWString(title);
				   
	if(title)
	{
		CFRelease(title);
	}
	
	return str;
	
#endif
}

void PKEdit::focusAcquired(PKDirection direction)
{
    PKUndoManager::getInstance()->addCommitCallback(this);

	this->_startText = this->getText();
	PKControl::focusAcquired(direction);

	if(this->getDialog())
	{
		this->getDialog()->editTextEditingStarted(this->getControlId());
	}
}

void PKEdit::focusLost()
{
    PKUndoManager::getInstance()->removeCommitCallback(this);

	PKControl::focusLost();

	if(this->getDialog())
	{
        if(this->_startText != this->getText())
        {
            std::wstring oldText = this->_startText;
    	    this->_startText = this->getText();

            this->getDialog()->editTextEditingEnded(this->getControlId(),
												    oldText,
												    this->getText());
        }
	}
}

void PKEdit::commitChanges()
{
	if(this->getDialog())
	{
        if(this->_startText != this->getText())
        {
            std::wstring oldText = this->_startText;
    	    this->_startText = this->getText();

		    this->getDialog()->editTextEditingEnded(this->getControlId(),
												    oldText,
												    this->getText());
        }
	}
}

void PKEdit::undoManagerCommitCallback()
{
	if(this->getDialog())
	{
		if(this->getDialog()->getFocusedControl() == this)
		{
		    this->commitChanges();
		}
	}
}

void PKEdit::resetChanges()
{
	this->_startText = this->getText();	
}

void PKEdit::selectText()
{
#ifdef WIN32
	
    if(this->hwnd)
    {
        SendMessage(this->hwnd, EM_SETSEL, 0, -1);
    }

#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		ControlEditTextSelectionRec rec;
		
		rec.selStart = 0;
		rec.selEnd   = 1000;

		SetControlData(this->controlRef,
					   kControlEntireControl,
					   kControlEditTextSelectionTag,
					   sizeof(rec), &rec);
	}

#endif
}


uint32_t PKEdit::getMinimumWidth()
{
	// Return accroding to our
	// current text & font

    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
		return this->getW();
	}

#ifdef WIN32

    PKVariant *t = this->get(PKEdit::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

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

	return size.cx;

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.right - rect.left);
#endif
}

uint32_t PKEdit::getMinimumHeight()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
        return this->getH();
	}

#ifdef WIN32

	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	HFONT old  = (HFONT) SelectObject(dc, font);

	GetTextExtentPoint32(dc, 
						 L"HELLO", 
						 5, 
						 &size);

	SelectObject(dc, old);

	ReleaseDC(this->hwnd, dc);

	return size.cy+6;

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	return (rect.bottom - rect.top);
#endif

}

#ifdef LINUX

void PKEdit::gtk_entry_text_changed_event(GtkEditable *editable,
                                          gpointer     user_data)
{
    PKEdit *edit = (PKEdit *) user_data;

    if(edit == NULL)
    {
        return;
    }          

    if(edit->getDialog() != NULL)
    {
        PKVariant *id = edit->get(PKControl::ID_STRING_PROPERTY);
        std::string sid = PKVALUE_STRING(id);

        const gchar *stext = gtk_entry_get_text(GTK_ENTRY(editable));
        std::wstring text  = PKStr::utf8StringToWstring((char *) stext);
        
  		PKVariantWString te(text);
		this->set(PKEdit::TEXT_WSTRING_PROPERTY, &te);

		edit->getDialog()->editTextChanged(sid, text);
    }                              
}
                                             
#endif


