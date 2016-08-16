//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKLabel.h"
#include "PKStr.h"
#include "PKFont.h"
#include "PKDialog.h"

//
// Properties
//

std::string PKLabel::MULTILINE_BOOL_PROPERTY          = "multiline";
std::string PKLabel::TEXT_WSTRING_PROPERTY            = "text";
std::string PKLabel::FONTNAME_STRING_PROPERTY         = "fontName";
std::string PKLabel::FONTSIZE_UINT32_PROPERTY         = "fontSize";
std::string PKLabel::RELATIVEFONTSIZE_STRING_PROPERTY = "relativeFontSize";
std::string PKLabel::COLOR_COLOR_PROPERTY             = "color";
std::string PKLabel::BOLD_BOOL_PROPERTY               = "bold";
std::string PKLabel::ITALICS_BOOL_PROPERTY            = "italics";
std::string PKLabel::ALIGN_STRING_PROPERTY            = "align";
std::string PKLabel::MAXWIDTH_UINT32_PROPERTY         = "maxWidth";
std::string PKLabel::FIXED_SIZE_BOOL_PROPERTY         = "fixedSize";


//
// PKLabel
//

PKLabel::PKLabel()
{
    this->dialog = NULL;

    PKColor c = PKColorCreate(0, 0, 0, 255);

    PKOBJECT_ADD_BOOL_PROPERTY(fixedSize, false);
    PKOBJECT_ADD_BOOL_PROPERTY(multiline, false);
    PKOBJECT_ADD_UINT32_PROPERTY(maxWidth, 32768);
    PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
    PKOBJECT_ADD_STRING_PROPERTY(fontName, "");
    PKOBJECT_ADD_UINT32_PROPERTY(fontSize, 0);
    PKOBJECT_ADD_COLOR_PROPERTY(color, c);
    PKOBJECT_ADD_BOOL_PROPERTY(bold, false);
    PKOBJECT_ADD_BOOL_PROPERTY(italics, false);
    PKOBJECT_ADD_STRING_PROPERTY(align, "left");
    PKOBJECT_ADD_STRING_PROPERTY(relativeFontSize, "NOT_SET");
	
#ifdef WIN32
    this->font = NULL;
#endif

    this->numLines = 1;
}

PKLabel::~PKLabel()
{
#ifdef WIN32

	if(this->hwnd != NULL)
	{
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
		DisposeControl(this->controlRef);
	}

#endif

}

void PKLabel::build()
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
	
	PKVariant *t = this->get(PKLabel::TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);

    PKVariant *m = this->get(PKLabel::MULTILINE_BOOL_PROPERTY);
    bool multiline = PKVALUE_BOOL(m);

    PKVariant *n = this->get(PKLabel::FONTNAME_STRING_PROPERTY);
    std::string fontName = PKVALUE_STRING(n);

    PKVariant *s = this->get(PKLabel::FONTSIZE_UINT32_PROPERTY);
    uint32_t fontSize = PKVALUE_UINT32(s);

    PKVariant *rs = this->get(PKLabel::RELATIVEFONTSIZE_STRING_PROPERTY);
    std::string relativeSize = PKVALUE_STRING(rs);

    PKVariant *b = this->get(PKLabel::BOLD_BOOL_PROPERTY);
    bool bold = PKVALUE_BOOL(b);

	PKVariant *i = this->get(PKLabel::ITALICS_BOOL_PROPERTY);
    bool italics = PKVALUE_BOOL(i);
	
    PKVariant *a = this->get(PKLabel::ALIGN_STRING_PROPERTY);
    std::string align = PKVALUE_STRING(a);

	PKVariant *fs = this->get(PKLabel::FIXED_SIZE_BOOL_PROPERTY);
    bool fixedSize = PKVALUE_BOOL(fs);

    // TODO : implement fixed Size for WIN/MAC?

#ifdef LINUX

    std::string caption = PKStr::wStringToUTF8string(text);

    this->widget = gtk_label_new(caption.c_str());
    
    if(this->widget)
    {
        PKVariant *c = this->get(PKLabel::COLOR_COLOR_PROPERTY);
        PKColor fontColor = PKVALUE_COLOR(c);

        // Set font
        
        PangoAttrList *list = pango_attr_list_new();
        
        if(fontName != "")
        {
            PangoAttribute *family = pango_attr_family_new(fontName.c_str());
            pango_attr_list_insert(list, family);
        }
        
        if(italics)
        {
            PangoAttribute *it = pango_attr_style_new(PANGO_STYLE_ITALIC);
            pango_attr_list_insert(list, it);
        }
        
        if(bold)
        {
            PangoAttribute *bd = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
            pango_attr_list_insert(list, bd);
        }

        if(relativeSize != "NOT_SET")
        {
            int32_t diff = PKStr::stringToInt32(relativeSize);
            fontSize     = PKFont::getSystemFontSize() + diff;
        }
        
        if(fontSize != 0)
        {
            PangoAttribute *sz = pango_attr_size_new((fontSize-2)*PANGO_SCALE);
            pango_attr_list_insert(list, sz);
        }
        
        guint16 r = ((guint32) fontColor.r)*65535/255;
        guint16 g = ((guint32) fontColor.g)*65535/255;
        guint16 b = ((guint32) fontColor.b)*65535/255;
        
        PangoAttribute *fg = pango_attr_foreground_new(r, g, b);
        pango_attr_list_insert(list, fg);
 
        gtk_label_set_attributes(GTK_LABEL(this->widget), list);
        pango_attr_list_unref(list);
    
        // Set text
                
        gtk_label_set_text(GTK_LABEL(this->widget), caption.c_str());
    
        // Set align
        
        if(PKStr::toLower(align) == "left")
        {
            gtk_misc_set_alignment(GTK_MISC(this->widget), 0, 0.5);
        }

        if(PKStr::toLower(align) == "right")
        {
            gtk_misc_set_alignment(GTK_MISC(this->widget), 1, 0.5);
        }

        if(PKStr::toLower(align) == "center")
        {
            gtk_misc_set_alignment(GTK_MISC(this->widget), 0.5, 0.5);
        }
        
        // Set options
        
        gtk_label_set_single_line_mode(GTK_LABEL(this->widget), multiline == false ? TRUE : FALSE);  
                
        // Set minimum size if needed

        if(fixedSize)
        {        
            if(fontSize != 0)
            {
                GtkRequisition req;
                gtk_widget_size_request(this->widget, &req);

                gtk_widget_set_size_request(this->widget, req.width, 2*fontSize);
            }                
            else
            {
                GtkRequisition req;
                gtk_widget_size_request(this->widget, &req);
                
                gtk_widget_set_size_request(this->widget, req.width, req.height);
            }                
        }                        
        else
        {        
            if(multiline)
            {
                GtkRequisition req;
                gtk_widget_size_request(this->widget, &req);

                PKVariant *m = this->get(PKLabel::MAXWIDTH_UINT32_PROPERTY);
                uint32_t max = PKVALUE_UINT32(m);

                // TODO: calculate real numLines
                int numLines = 3;

                gtk_widget_set_size_request(this->widget, max, numLines*(((fontSize!=0)?fontSize:req.height)));            
                gtk_label_set_line_wrap(GTK_LABEL(this->widget), TRUE);
            }
            else
            {
                if(fontSize != 0)
                {
                    GtkRequisition req;
                    gtk_widget_size_request(this->widget, &req);

                    gtk_widget_set_size_request(this->widget, -1, 2*fontSize);
                }                
                else
                {
                    GtkRequisition req;
                    gtk_widget_size_request(this->widget, &req);
                    
                    gtk_widget_set_size_request(this->widget, -1, req.height);
                }                
            }
        }                                
    }

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    DWORD style = WS_CHILD | WS_VISIBLE | SS_EDITCONTROL;

    if(PKStr::toLower(align) == "left")
    {
        style |= SS_LEFT;
    }

    if(PKStr::toLower(align) == "right")
    {
        style |= SS_RIGHT;
    }

    if(PKStr::toLower(align) == "center")
    {
        style |= SS_CENTER;
    }

    this->hwnd = CreateWindow(L"STATIC", text.c_str(),
							  style, 0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

	if(this->hwnd)
	{
        // Set the font

        if(fontName == "" && 
           fontSize == 0  && 
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

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

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
	
	if(PKStr::toLower(align) == "left")
	{
		fontRec.flags |= kControlUseJustMask;
		fontRec.just = teFlushLeft;
	}
	
	if(PKStr::toLower(align) == "right")
	{
		fontRec.flags |= kControlUseJustMask;
		fontRec.just = teFlushRight;
	}

	if(PKStr::toLower(align) == "center")
	{
		fontRec.flags |= kControlUseJustMask;
		fontRec.just = teCenter;
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
	
    PKVariant *c = this->get(PKLabel::COLOR_COLOR_PROPERTY);
    PKColor fontColor = PKVALUE_COLOR(c);
	
	fontRec.flags |= kControlUseForeColorMask;
	
	fontRec.foreColor.red   = fontColor.r*65535/255;
	fontRec.foreColor.green = fontColor.g*65535/255;
	fontRec.foreColor.blue  = fontColor.b*65535/255;
	
	OSStatus err = CreateStaticTextControl(window, &rect, NULL, &fontRec, &this->controlRef);

	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);
	
		// Make it multiline or not
		
		Boolean multiLine = multiline;
		
		SetControlData(this->controlRef, 
					   kControlEntireControl,
					   kControlStaticTextIsMultilineTag,
					   sizeof(multiLine), &multiLine);
					   
		CFStringRef title = PKStr::wStringToCFString(text);

		SetControlData(this->controlRef, kControlEntireControl, 
					   kControlStaticTextCFStringTag,
					   sizeof(title), &title);

		CFRelease(title);	
	}

#endif
}

// From PKObject
void PKLabel::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKLabel::TEXT_WSTRING_PROPERTY)
    {
	    PKVariant *t = this->get(PKLabel::TEXT_WSTRING_PROPERTY);
	    std::wstring text = PKVALUE_WSTRING(t);

	    PKVariant *fs = this->get(PKLabel::FIXED_SIZE_BOOL_PROPERTY);
        bool fixedSize = PKVALUE_BOOL(fs);

        // TODO : implement fixed Size for WIN/MAC?

#ifdef WIN32
		
		if(this->hwnd)
		{
			SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) text.c_str());

			if(this->getLayoutPolicyX() == PK_LAYOUT_MIN)
			{
				int minWidth = this->getMinimumWidth();
				this->resize(minWidth, this->getH());
			}
		}
		
        this->invalidate();
#endif

#ifdef LINUX

        if(this->widget != NULL)
        {
            std::string caption = PKStr::wStringToUTF8string(text);
            gtk_label_set_text(GTK_LABEL(this->widget), caption.c_str());
        
            if(!fixedSize)
            {
                if(this->dialog != NULL)
                {
                    // overkill
                    // this->dialog->resizeToFit();
                }                            
            }
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

            if(this->getLayoutPolicyX() == PK_LAYOUT_MIN)
		    {
			    int minWidth = this->getMinimumWidth();
			    this->resize(minWidth, this->getH());
		    }
        }
#endif
    }

    PKControl::selfPropertyChanged(prop, oldValue);
}


void PKLabel::setText(std::wstring text)
{
    PKVariantWString val(text);
    this->set(PKLabel::TEXT_WSTRING_PROPERTY, &val);
}

uint32_t PKLabel::getMinimumWidth()
{
	// Return accroding to our
	// current text & font

    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
        return this->getW();
	}

    PKVariant *t = this->get(PKLabel::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

	if(text == L"")
	{
		return 0;
	}

    uint32_t minSize;

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

	minSize = size.cx + 4;

#endif

#ifdef MACOSX

	Boolean multiLine = false;
	
	SetControlData(this->controlRef, 
				   kControlEntireControl,
				   kControlStaticTextIsMultilineTag,
				   sizeof(multiLine), &multiLine);

	Rect rect;
	SInt16 dummy;

	GetBestControlRect(this->controlRef, &rect, &dummy);
	minSize = (rect.right - rect.left);
	
    PKVariant *ml = this->get(PKLabel::MULTILINE_BOOL_PROPERTY);
    bool multiline = PKVALUE_BOOL(ml);

	multiLine = multiline;
	
	SetControlData(this->controlRef, 
				   kControlEntireControl,
				   kControlStaticTextIsMultilineTag,
				   sizeof(multiLine), &multiLine);
#endif

    PKVariant *m = this->get(PKLabel::MAXWIDTH_UINT32_PROPERTY);
    uint32_t max = PKVALUE_UINT32(m);

    if(minSize > max)
    {
        this->numLines = minSize / max + 2;
        minSize = max;
    }
    
    return minSize;
}

uint32_t PKLabel::getMinimumHeight()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

    PKVariant *t = this->get(PKLabel::TEXT_WSTRING_PROPERTY);
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

	return this->numLines*size.cy;

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	Boolean multiLine = false;
	
	SetControlData(this->controlRef, 
				   kControlEntireControl,
				   kControlStaticTextIsMultilineTag,
				   sizeof(multiLine), &multiLine);

	GetBestControlRect(this->controlRef, &rect, &dummy);
	
	
    PKVariant *m = this->get(PKLabel::MULTILINE_BOOL_PROPERTY);
    bool multiline = PKVALUE_BOOL(m);
	
	multiLine = multiline;
	
	SetControlData(this->controlRef, 
				   kControlEntireControl,
				   kControlStaticTextIsMultilineTag,
				   sizeof(multiLine), &multiLine);
	
	return this->numLines*(rect.bottom - rect.top);
#endif

    return 0;
}

int PKLabel::getTextWidth()
{
    PKVariant *t = this->get(PKLabel::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);
	
    if(text == L"")
	{
		return 0;
	}

    uint32_t minSize;

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

	minSize = size.cx;

#endif

#ifdef LINUX
    // TODO
#endif

#ifdef MACOSX

	Rect rect;
	SInt16 dummy;
	
	// Fix this ?
	
	GetBestControlRect(this->controlRef, &rect, &dummy);
	minSize = (rect.right - rect.left);
	
#endif

    PKVariant *m = this->get(PKLabel::MAXWIDTH_UINT32_PROPERTY);
    uint32_t max = PKVALUE_UINT32(m);

    if(minSize > max)
    {
        this->numLines = minSize / max + 2;
        minSize = max;
    }

    return minSize;
}

int PKLabel::getTextHeight()
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

	return this->numLines*size.cy;
#endif

#ifdef LINUX
    // TODO
#endif

#ifdef MACOSX

	Rect rect;
	SInt16 dummy;
	
	GetBestControlRect(this->controlRef, &rect, &dummy);
	return this->numLines*(rect.bottom - rect.top);
	
#endif
}
