//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKLink.h"
#include "PKStr.h"
#include "PKBrowserTool.h"
#include "PKCursorTools.h"
#include "PKDialog.h"
#include "PKFont.h"

//
// MACOSX
//

#ifdef MACOSX

pascal OSStatus PKLink::PKLinkEventHandler(EventHandlerCallRef inHandlerRef, EventRef inEvent, void *inUserData)
{
    OSStatus result = eventNotHandledErr;
    
	PKLink *link = (PKLink *) inUserData;
	
	if(link == NULL)
	{
		return result;
	}
	
    UInt32 eventClass = GetEventClass(inEvent);
	
    switch(eventClass)
    {
		case kEventClassControl:
		{
			UInt32 eventKind = GetEventKind(inEvent);
			
			if(eventKind == kEventControlSetCursor)
			{
				PKCursorTools::setCursor(PK_CURSOR_HAND);
				result = noErr;
			}
			
			if(eventKind == kEventControlClick)
			{
				PKVariant *u = link->get(PKLink::URL_STRING_PROPERTY);
				std::string url = PKVALUE_STRING(u);

                if(url != "")
                {
    				PKBrowserTool::openURL(url);
                }
                else
                {
                    std::string id = PKVALUE_STRING(link->get(PKControl::ID_STRING_PROPERTY));
                    link->dialog->linkClicked(id);
                }
			}
		}
		break;			
	}
	
	
	return result;
}


enum {
	kPKCSignature   = FOUR_CHAR_CODE('PKCS'),
	kPKCInColorProc = FOUR_CHAR_CODE('PKCC'),
	kPKHasColorProc = FOUR_CHAR_CODE('PKHC'),
};


static pascal OSStatus ControlColorProc(ControlRef inControl, SInt16 inMessage, SInt16 inDrawDepth, Boolean inDrawInColor)
{
	bool weHandled = false;
	ControlFontStyleRec style;
	Size actualSize;
	OSStatus error;
	
	bool alreadyBeenHere = false;
	
	// don't let ourselves get call recursively
	error = GetControlProperty(inControl, kPKCSignature, kPKCInColorProc, sizeof(bool), NULL, &alreadyBeenHere);
	
	if ((error == noErr) && alreadyBeenHere)
		return -1;
	
	error = GetControlData(inControl, kControlEntireControl, kControlStaticTextStyleTag, sizeof(ControlFontStyleRec), &style, &actualSize);
	
	if (error == noErr)
	{
		switch (inMessage)
		{
			case kControlMsgApplyTextColor:
				if (style.flags & (kControlUseForeColorMask | kControlUseFaceMask | kControlUseModeMask))
				{
					alreadyBeenHere = true;
					verify_noerr(SetControlProperty(inControl, kPKCSignature, kPKCInColorProc, sizeof(bool), &alreadyBeenHere));
					
					SetUpControlTextColor(inControl, inDrawDepth, inDrawInColor);
					
					RemoveControlProperty(inControl, kPKCSignature, kPKCInColorProc);
					
					if (style.flags & kControlUseForeColorMask)
						RGBForeColor(&style.foreColor);
					
					if (style.flags & kControlUseFaceMask)
						TextFace(style.style);
					
					if (style.flags & kControlUseModeMask)
						TextMode(style.mode); // i don't think this was working
					
					if ((IsControlEnabled(inControl) == false) || (IsControlActive(inControl) == false))
						TextMode(grayishTextOr);
					
					weHandled = true;
					
				}
				
				break;
				
			case kControlMsgSetUpBackground:
				break;
				
			default:
				break;
		}
	}
	
	return (weHandled ? noErr : -1);
}

// Utility

OSStatus SetStaticTextControlColor(ControlRef control, const RGBColor *color)
{
	OSStatus error = paramErr;
	
	check(IsValidControlHandle(control));
	
	if (control != NULL)
	{
		ControlFontStyleRec style;
		
		Size actualSize;
		
		error = GetControlData(control, kControlEntireControl, kControlStaticTextStyleTag, sizeof(ControlFontStyleRec), &style, &actualSize);
		
		if (error == noErr)
		{
			style.flags |= kControlUseForeColorMask;
			style.foreColor = *color;
			
			error = SetControlFontStyle(control, &style);
			
			UInt32 actualSize;
			bool hasIt = false;
			OSStatus error = GetControlProperty(control, kPKCSignature, kPKHasColorProc, sizeof(bool), &actualSize, &hasIt);
			
			if ((error != noErr) || (hasIt == false))
			{
				static ControlColorUPP colorProc = NewControlColorUPP(ControlColorProc);
				hasIt = true;
				
				SetControlColorProc(control, colorProc);
				SetControlProperty(control, kPKCSignature, kPKHasColorProc, sizeof(bool), &hasIt);
			}
			
			if (error == noErr)
				DrawOneControl(control);
		}
		
	}
	
	check_noerr(error);
	
	return error;
}

#endif

//
// WIN32
//

#ifdef WIN32

#include <windowsx.h>

LRESULT CALLBACK PKLink::LinkWinProc(HWND hWnd, 
								     UINT msg, 
								     WPARAM wParam, 
								     LPARAM lParam)
{
	PKLink *ctrl = (PKLink *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(ctrl == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch(msg)
	{
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        {
            PKCursorTools::setCursor(PK_CURSOR_HAND);
        }
        break;
	}

	return ctrl->lpOldProc(hWnd, msg, wParam, lParam);
}

#endif

//
// Properties
//

std::string PKLink::URL_STRING_PROPERTY               = "url";
std::string PKLink::TEXT_WSTRING_PROPERTY             = "text";
std::string PKLink::COLOR_COLOR_PROPERTY              = "color";
std::string PKLink::FONTNAME_STRING_PROPERTY          = "fontName";
std::string PKLink::FONTSIZE_UINT32_PROPERTY          = "fontSize";
std::string PKLink::RELATIVEFONTSIZE_STRING_PROPERTY  = "relativeFontSize";
std::string PKLink::BOLD_BOOL_PROPERTY                = "bold";
std::string PKLink::ITALICS_BOOL_PROPERTY             = "italics";

//
// PKLink
//

PKLink::PKLink()
{
    PKColor c = PKColorCreate(0, 0, 255, 255);
    PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
    PKOBJECT_ADD_COLOR_PROPERTY(color, c);
    PKOBJECT_ADD_STRING_PROPERTY(url, "");
    PKOBJECT_ADD_STRING_PROPERTY(fontName, "");
    PKOBJECT_ADD_UINT32_PROPERTY(fontSize, 0);
    PKOBJECT_ADD_BOOL_PROPERTY(bold, false);
    PKOBJECT_ADD_BOOL_PROPERTY(italics, false);
    PKOBJECT_ADD_STRING_PROPERTY(relativeFontSize, "NOT_SET");
	
#ifdef WIN32
    this->font = NULL;
#endif
}

PKLink::~PKLink()
{
#ifdef WIN32

	if(this->hwnd != NULL)
	{
    	SubclassWindow(this->hwnd, (WNDPROC) this->lpOldProc);
		DestroyWindow(this->hwnd);
	}

    if(this->font != NULL)
    {
        DeleteObject(this->font);
    }

#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		RemoveEventHandler(this->handler);
		DisposeControl(this->controlRef);
	}

#endif

}

void PKLink::build()
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
	
	PKVariant *t = this->get(PKLink::TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);

    PKVariant *n = this->get(PKLink::FONTNAME_STRING_PROPERTY);
    std::string fontName = PKVALUE_STRING(n);

    PKVariant *s = this->get(PKLink::FONTSIZE_UINT32_PROPERTY);
    uint32_t fontSize = PKVALUE_UINT32(s);

    PKVariant *rs = this->get(PKLink::RELATIVEFONTSIZE_STRING_PROPERTY);
    std::string relativeSize = PKVALUE_STRING(rs);

	PKVariant *b = this->get(PKLink::BOLD_BOOL_PROPERTY);
    bool bold = PKVALUE_BOOL(b);

	PKVariant *i = this->get(PKLink::ITALICS_BOOL_PROPERTY);
    bool italics = PKVALUE_BOOL(i);

#ifdef LINUX

    PKVariant *u = this->get(PKLink::URL_STRING_PROPERTY);
    std::string url = PKVALUE_STRING(u);

    this->widget = gtk_link_button_new(url.c_str());

    if(this->widget != NULL)
	{
	    // Set caption
        std::string caption = PKStr::wStringToUTF8string(text);    
        gtk_button_set_label(GTK_BUTTON(this->widget), caption.c_str());
    }

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    this->hwnd = CreateWindow(L"STATIC", text.c_str(),
							  WS_CHILD | WS_VISIBLE | SS_LEFT | SS_EDITCONTROL | SS_NOTIFY, 0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

	if(this->hwnd)
	{
        this->lpOldProc = SubclassWindow(this->hwnd, (WNDPROC) LinkWinProc);
    	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID) (PKLink *) this);

        // Set the font

        if(fontName == "" && 
           fontSize == 0 &&
           relativeSize == "NOT_SET" &&
           !bold && !italics)
        {
            // Default font

    		SendMessage(this->hwnd, WM_SETFONT, 
	    			   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	
        }
        else
        {
            if(relativeSize != "NOT_SET")
            {
                int32_t diff = PKStr::stringToInt32(relativeSize);
                fontSize     = PKFont::getSystemFontSize() + diff;
            }

            if(fontName == "")
            {
                // Default font with different size

                HFONT guiFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
                HDC   guiDC   = GetDC(this->hwnd);

                HFONT old = (HFONT) SelectObject(guiDC, guiFont);

                wchar_t name[1024];
                GetTextFace(guiDC, 1024, name);

                SelectObject(guiDC, old);

                HFONT newFont = CreateFont(-MulDiv(fontSize, GetDeviceCaps(guiDC, LOGPIXELSY), 72),
                                           0, 0, 0, (bold ? FW_BOLD : FW_NORMAL), italics, FALSE, FALSE, DEFAULT_CHARSET,
                                           OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
                                           DEFAULT_PITCH, name);

                if(newFont == NULL)
                {
                    // Falback to default font

    		        SendMessage(this->hwnd, WM_SETFONT, 
	    			           (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	
                }
                else
                {
                    this->font = newFont;
                    SendMessage(this->hwnd, WM_SETFONT, 
                               (WPARAM)newFont, FALSE);	
                }
            }
            else
            {
                HDC guiDC = GetDC(this->hwnd);
                
                // Both size and font are set here

                std::wstring wFontName = PKStr::stringToWString(fontName);

                HFONT newFont = CreateFont(-MulDiv(fontSize, GetDeviceCaps(guiDC, LOGPIXELSY), 72),
                                           0, 0, 0, (bold ? FW_BOLD : FW_NORMAL), italics, FALSE, FALSE, DEFAULT_CHARSET,
                                           OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY/*ANTIALIASED_QUALITY*/, 
                                           DEFAULT_PITCH, wFontName.c_str());

                if(newFont == NULL)
                {
                    // Falback to default font

    		        SendMessage(this->hwnd, WM_SETFONT, 
	    			           (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	
                }
                else
                {
                    this->font = newFont;
                    SendMessage(this->hwnd, WM_SETFONT, 
                               (WPARAM)newFont, FALSE);	
                }
            }
        }
	}

#endif

#ifdef MACOSX

	PKVariant *c  = this->get(PKLink::COLOR_COLOR_PROPERTY);
	PKColor color = PKVALUE_COLOR(c);
	
	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	RGBColor foreColor;
	
	foreColor.red   = ((uint32_t) ((uint32_t)color.r * 0xFFFF) / 255);
	foreColor.green = ((uint32_t) ((uint32_t)color.g * 0xFFFF) / 255);
	foreColor.blue  = ((uint32_t) ((uint32_t)color.b * 0xFFFF) / 255);
	
	ControlFontStyleRec fontRec;
	memset(&fontRec, 0, sizeof(fontRec));
	
	if(fontSize != 0)
	{
		fontRec.flags |= kControlUseSizeMask;
		fontRec.size   = fontSize;
	}	
	
	if(relativeSize != "NOT_SET")
	{
		fontRec.flags |= kControlUseSizeMask;
		fontRec.size   = PKFont::getSystemFontSize() + PKStr::stringToInt32(relativeSize);
	}
	
	if(bold)
	{
		fontRec.flags |= kControlUseFaceMask;
		fontRec.style |= 0x01; // bold 
	}
	
	if(italics)
	{
		fontRec.flags |= kControlUseFaceMask;
		fontRec.style |= 0x02; // italics 
	}
	
	if(fontName != "")
	{
		std::wstring wFontName = PKStr::stringToWString(fontName);
		CFStringRef cfFontName = PKStr::wStringToCFString(wFontName);
		
		ATSFontFamilyRef fontRef = ATSFontFamilyFindFromName(cfFontName, kATSOptionFlagsDefault);
		
		CFRelease(cfFontName);
		
		fontRec.flags |= kControlUseFontMask;
		fontRec.font   = (short) fontRef;
	}
	
	fontRec.flags |= kControlUseForeColorMask;
	
	fontRec.foreColor.red   = color.r*65535/255;
	fontRec.foreColor.green = color.g*65535/255;
	fontRec.foreColor.blue  = color.b*65535/255;

	OSStatus err = CreateStaticTextControl(window, &rect, NULL, &fontRec, &this->controlRef);

	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);
	
		CFStringRef title = PKStr::wStringToCFString(text);

		SetControlData(this->controlRef, kControlEntireControl, 
					   kControlStaticTextCFStringTag,
					   sizeof(title), &title);

		// Single line here
		
		Boolean multiLine = false;
		
		SetControlData(this->controlRef, 
					   kControlEntireControl,
					   kControlStaticTextIsMultilineTag,
					   sizeof(multiLine), &multiLine);

		// Set the font color
		
		SetStaticTextControlColor(this->controlRef, &foreColor);

		// Install event handlers
		
		EventTypeSpec    eventTypes[2];
		
		eventTypes[0].eventClass = kEventClassControl;
		eventTypes[0].eventKind  = kEventControlSetCursor;

		eventTypes[1].eventClass = kEventClassControl;
		eventTypes[1].eventKind  = kEventControlClick;
		
		OSStatus err;
		
		err = InstallEventHandler(GetControlEventTarget(this->controlRef),
							NewEventHandlerUPP(PKLink::PKLinkEventHandler),
							2,
							eventTypes,
							(void *) this,
							&this->handler);	
		
		
		CFRelease(title);	
	}

#endif
}

// Fire the URL
void PKLink::clicked()
{
    PKVariant *u = this->get(PKLink::URL_STRING_PROPERTY);
    std::string url = PKVALUE_STRING(u);

    if(url != "")
    {
        PKBrowserTool::openURL(url);
    }
    else
    {
        std::string id = PKVALUE_STRING(this->get(PKControl::ID_STRING_PROPERTY));
        this->dialog->linkClicked(id);
    }
}

// From PKObject
void PKLink::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKLink::COLOR_COLOR_PROPERTY)
    {
#ifdef WIN32
        // Just trigger a redraw
        if(this->hwnd)
        {
            RECT rect;
            GetClientRect(this->hwnd, &rect);
            InvalidateRect(this->hwnd, &rect, TRUE);
        }
#endif

#ifdef LINUX

        if(this->widget)
        {
            // TODO
        }

#endif

#ifdef MACOSX

		if(this->controlRef)
		{
			PKVariant *c  = this->get(PKLink::COLOR_COLOR_PROPERTY);
			PKColor color = PKVALUE_COLOR(c);
		
			RGBColor foreColor;
		
			foreColor.red   = ((uint32_t) ((uint32_t)color.r * 0xFFFF) / 255);
			foreColor.green = ((uint32_t) ((uint32_t)color.g * 0xFFFF) / 255);
			foreColor.blue  = ((uint32_t) ((uint32_t)color.b * 0xFFFF) / 255);

			SetStaticTextControlColor(this->controlRef, &foreColor);

			this->invalidate();
		}
		
#endif
    }

    if(prop->getName() == PKLink::TEXT_WSTRING_PROPERTY)
    {
	    PKVariant *t = this->get(PKLink::TEXT_WSTRING_PROPERTY);
	    std::wstring text = PKVALUE_WSTRING(t);

#ifdef WIN32
        SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) text.c_str());
#endif

#ifdef LINUX

        if(this->widget)
        {
            std::string textUTF8 = PKStr::wStringToUTF8string(text);
            gtk_button_set_label(GTK_BUTTON(this->widget), textUTF8.c_str());
        }

#endif

#ifdef MACOSX
		if(this->controlRef != NULL)
		{
			CFStringRef title = PKStr::wStringToCFString(text);

			SetControlData(this->controlRef, kControlEntireControl, 
						   kControlStaticTextCFStringTag,
						   sizeof(title), &title);

			CFRelease(title);	
		}
#endif
    }

    PKControl::selfPropertyChanged(prop, oldValue);
}


void PKLink::setText(std::wstring text)
{
    PKVariantWString val(text);
    this->set(PKLink::TEXT_WSTRING_PROPERTY, &val);
}

void PKLink::setColor(PKColor c)
{
    PKVariantColor val(c.r, c.g, c.b, c.a);
    this->set(PKLink::COLOR_COLOR_PROPERTY, &val);
}

uint32_t PKLink::getMinimumWidth()
{
	// Return accroding to our
	// current text & font

    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
        return this->getW();
	}

    PKVariant *t = this->get(PKLink::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

	if(text == L"")
	{
		return 0;
	}

#ifdef WIN32

	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

    if(this->font != NULL)
    {
        font = this->font;
    }
    
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

    return 0;
}

uint32_t PKLink::getMinimumHeight()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

    PKVariant *t = this->get(PKLink::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

    if(text == L"")
	{
		return 0;
	}

#ifdef WIN32

	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

    if(this->font != NULL)
    {
        font = this->font;
    }
    
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

int PKLink::getTextWidth()
{
    PKVariant *t = this->get(PKLink::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);
	
    if(text == L"")
	{
		return 0;
	}

#ifdef WIN32

	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

    if(this->font != NULL)
    {
        font = this->font;
    }
    
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

    return 0;
}

int PKLink::getTextHeight()
{
#ifdef WIN32
	SIZE size;

	HDC dc = GetDC(this->hwnd);
	
	HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

    if(this->font != NULL)
    {
        font = this->font;
    }
    
    HFONT old  = (HFONT) SelectObject(dc, font);

	GetTextExtentPoint32(dc, 
						 L"HELLO", 
						 5, 
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
