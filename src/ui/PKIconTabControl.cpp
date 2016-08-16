

#include "PKIconTabControl.h"
#include "PKImageLoader.h"
#include "PKCursorTools.h"
#include "PKDialog.h"

#include "PKEdit.h"
#include "PKCheckBox.h"
#include "PKComboBox.h"
#include "PKButton.h"

//
// Properties
//

std::string PKIconTabControl::ICONS_WSTRINGLIST_PROPERTY    = "icons";
std::string PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY   = "currentTab";
std::string PKIconTabControl::TOOLTIPS_WSTRINGLIST_PROPERTY = "tooltips";

//
// PKIconTabControl
//

PKIconTabControl::PKIconTabControl()
{
    this->built = false;

    std::vector<std::wstring> empty;
    PKOBJECT_ADD_WSTRINGLIST_PROPERTY(icons, empty);
    PKOBJECT_ADD_UINT32_PROPERTY(currentTab, 0);
    PKOBJECT_ADD_WSTRINGLIST_PROPERTY(tooltips, empty);
    
#ifdef LINUX
    this->tab  = NULL;
    this->hand = gdk_cursor_new(GDK_HAND2);
#endif
}

PKIconTabControl::~PKIconTabControl()
{
#ifdef WIN32
    for(uint32_t i=0; i<this->hicons.size(); i++)
    {
        if(this->hicons[i])
        {
            DestroyIcon(this->hicons[i]);
        }
    }

    this->hicons.clear();
#endif

#ifdef MACOSX
    for(uint32_t i=0; i<this->bitmaps.size(); i++)
    {
        if(this->bitmaps[i])
        {
            delete this->bitmaps[i];
        }
    }
	
    this->bitmaps.clear();
#endif

#ifdef LINUX
    gdk_cursor_unref(this->hand);
#endif
}

void PKIconTabControl::build()
{
    if(this->built)
    {
        return;
    }

#ifdef LINUX
    PKCustomContainer::build();
#else
    PKCustomControl::build();
#endif

#ifdef WIN32

    PKVariant *i = this->get(PKIconTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);

    // Create HICONS for the icons we have

    for(uint32_t i=0; i<icons.size(); i++)
    {
		PKImage *bitmap = NULL;
	
		PKImageURIType type;
		std::wstring   image;
	
		PKImage::processURI(icons[i], &type, &image);
	
		if(type == PKIMAGE_URI_RESOURCE)
		{
			bitmap = PKImageLoader::loadBitmapFromResource(image);
		}
	
		if(type == PKIMAGE_URI_FILENAME)
		{
			bitmap = PKImageLoader::loadBitmapFromFilename(image);
		}
	
		if(bitmap != NULL)
		{
            // Create HICON from it

            int32_t w = bitmap->getWidth();
            int32_t h = bitmap->getHeight();

            // Create DIB bitmap first

            BITMAPV5HEADER bi;
            void *lpBits = NULL;
            HBITMAP hBitmap = NULL;

            ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
            
            bi.bV5Size        = sizeof(BITMAPV5HEADER);
            bi.bV5Width       = w;
            bi.bV5Height      = -h;
            bi.bV5Planes      = 1;
            bi.bV5BitCount    = 32;
            bi.bV5Compression = BI_BITFIELDS;

            bi.bV5RedMask   =  0x00FF0000;
            bi.bV5GreenMask =  0x0000FF00;
            bi.bV5BlueMask  =  0x000000FF;
            bi.bV5AlphaMask =  0xFF000000; 

            HDC hDC = GetDC(NULL);

            hBitmap = CreateDIBSection(hDC, 
                                       (BITMAPINFO *)&bi, 
                                       DIB_RGB_COLORS, 
                                       (void **)&lpBits, 
                                       NULL, (DWORD)0);

            ReleaseDC(NULL, hDC);
                
            if(hBitmap != NULL && 
               lpBits  != NULL)
            {
                memcpy(lpBits, bitmap->getPixels(), w*h*4);
            }

            // Create Mask

            HBITMAP hMonoBitmap = CreateBitmap(w, h, 1, 1, NULL);

            ICONINFO iinfo;
            memset(&iinfo, 0, sizeof(ICONINFO));

            iinfo.fIcon    = TRUE;
            iinfo.hbmColor = hBitmap;
            iinfo.hbmMask  = hMonoBitmap;

            HICON icon = CreateIconIndirect(&iinfo);
            this->hicons.push_back(icon);

            DeleteObject(hBitmap);          
            DeleteObject(hMonoBitmap); 

            delete bitmap;
        }
    }

#endif

#ifdef MACOSX
	
    PKVariant *i = this->get(PKIconTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);
	
    // Create HICONS for the icons we have
	
    for(uint32_t i=0; i<icons.size(); i++)
    {
		PKImage *bitmap = NULL;
		
		PKImageURIType type;
		std::wstring   image;
		
		PKImage::processURI(icons[i], &type, &image);
		
		if(type == PKIMAGE_URI_RESOURCE)
		{
			bitmap = PKImageLoader::loadBitmapFromResource(image);
		}
		
		if(type == PKIMAGE_URI_FILENAME)
		{
			bitmap = PKImageLoader::loadBitmapFromFilename(image);
		}
		
		if(bitmap != NULL)
		{
			this->bitmaps.push_back(bitmap);
		}
	}

#endif	

#ifdef LINUX

    PKVariant *i = this->get(PKIconTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);
	
    // Create HICONS for the icons we have
	
    for(uint32_t i=0; i<icons.size(); i++)
    {
		PKImage *bitmap = NULL;
		
		PKImageURIType type;
		std::wstring   image;
		
		PKImage::processURI(icons[i], &type, &image);
		
		if(type == PKIMAGE_URI_RESOURCE)
		{
			bitmap = PKImageLoader::loadBitmapFromResource(image);
		}
		
		if(type == PKIMAGE_URI_FILENAME)
		{
			bitmap = PKImageLoader::loadBitmapFromFilename(image);
		}
		
		if(bitmap != NULL)
		{
            // Invert R & B

            uint32_t width  = bitmap->getWidth();
            uint32_t height = bitmap->getWidth();
            
            uint8_t *pdata  = (uint8_t *) bitmap->getPixels();

            for(unsigned int j=0; j < height; j++)
            {
                for(unsigned int i=0; i < width; i++)
                {
                    uint8_t temp = pdata[0];
                     
	                pdata[0] = pdata[2];
	                pdata[2] = temp;

	                pdata +=4;
                }
            }

            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*) bitmap->getPixels(),
                                                    GDK_COLORSPACE_RGB,
                                                    TRUE, 8, width, height, 
                                                    width*4, 
                                                    NULL, NULL);
                                                    
            this->bitmaps.push_back(pixbuf);
		}
	}

    // Create a tab child and add children to it
    
    this->tab = gtk_notebook_new();
    
    if(this->tab != NULL)
	{
        gtk_notebook_set_show_tabs(GTK_NOTEBOOK(this->tab), FALSE);
        gtk_notebook_set_show_border(GTK_NOTEBOOK(this->tab), FALSE);
        
        gtk_widget_show(this->tab);
        gtk_layout_put(GTK_LAYOUT(this->layout), this->tab, 0, 24);
	}

#endif
	
    // Build children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->build();

#ifdef LINUX		
		    gtk_notebook_append_page(GTK_NOTEBOOK(this->tab), 
		                             child->getWindowHandle(),
		                             NULL);
#endif
		}
	}

    // Force switch to first tab
    this->switchToTab(0);

    // All done!
    this->built = true;
}

void PKIconTabControl::initialize(PKDialog *dialog)
{
#ifdef LINUX
    PKCustomContainer::initialize(dialog);
#else
    PKCustomControl::initialize(dialog);
#endif

    // Init children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->initialize(dialog);			
		}
	}

#ifdef LINUX
        GtkRequisition req;
        gtk_widget_size_request(this->tab, &req);            
        gtk_widget_set_size_request(this->widget, req.width, req.height+25);        
#endif


    // Add tooltips

    PKVariant *t = this->get(PKIconTabControl::TOOLTIPS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> tooltips = PKVALUE_WSTRINGLIST(t);

    for(uint32_t i=0; i<tooltips.size(); i++)
    {
        PKRect rect;
        rect.x = i*25;
        rect.y = 0;
        rect.w = 25;
        rect.h = 25;

        this->addTooltip(rect, tooltips[i]);
    }
}

void PKIconTabControl::destroy()
{
    // Clear children first

    for(uint32_t i=0; i<this->getChildrenCount(); i++)
    {
        PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

        if(child)
        {
            child->destroy();
            child = NULL;
        }
    }
    this->clearChildren();

    // Then call parent class

#ifdef LINUX
    PKCustomContainer::destroy();
#else
    PKCustomControl::destroy();
#endif
}

PKWindowHandle PKIconTabControl::getWindowHandle()
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

void PKIconTabControlHasFocusChildrenWalk(PKObject *child, void *context)
{
    PKIconTabControl *icon = (PKIconTabControl *) context;

    if(icon && icon->getDialog())
    {
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl != NULL)
        {
            PKControl *focus = icon->getDialog()->getFocusedControl();

            if(focus)
            {
                if(focus == ctrl)
                {
                    icon->hasFocusChild = true;
                }
            }
        }
    }
}

void PKIconTabControlSetFocusChildrenWalk(PKObject *child, void *context)
{
    PKIconTabControl *icon = (PKIconTabControl *) context;

    if(icon && icon->getDialog())
    {
        if(icon->focusSet)
        {
            return;
        }

        PKControl *ctrl = dynamic_cast<PKControl*>(child);
        
        PKCheckBox *box = NULL;
        PKEdit    *edit = NULL;
        PKComboBox  *cb = NULL;
        PKButton    *bt = NULL;

        edit = dynamic_cast<PKEdit*>(child);
#ifndef MACOSX
        box = dynamic_cast<PKCheckBox*>(child);
#endif
        cb = dynamic_cast<PKComboBox*>(child);
        bt = dynamic_cast<PKButton*>(child);

        if(ctrl != NULL && (box != NULL || edit != NULL || cb != NULL || bt != NULL))
        {
            PKVariant *h = ctrl->get(PKControl::HIDDEN_BOOL_PROPERTY);
            bool hidden  = PKVALUE_BOOL(h);

            PKVariant *e   = ctrl->get(PKControl::ENABLED_BOOL_PROPERTY);
            bool disabledd = !PKVALUE_BOOL(e);

            if(!hidden && !disabledd && ctrl->isShowing())
            {
                PKVariant *id = ctrl->get(PKControl::ID_STRING_PROPERTY);
                std::string cid = PKVALUE_STRING(id);
                
                icon->getDialog()->setFocusToControl(cid);
                icon->focusSet = true;
            }
        }
    }
}

void PKIconTabControl::switchToTab(uint32_t tab)
{
    if(tab >= this->getChildrenCount())
    {
        return;
    }

    // See if one of our children has the docus
    // because if so, we must reset it to the 
    // first child of the new tab

    this->hasFocusChild = false;
    this->walkChildren(PKIconTabControlHasFocusChildrenWalk, this);

#ifdef LINUX

    if(this->tab != NULL)
    {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(this->tab), tab);

        // Change our property, this will redraw

        PKVariantUInt32 sel(tab);
        this->set(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY, &sel);
    }
    
    return;

#endif

    for(uint32_t i=0; i<this->getChildrenCount(); i++)
    {
        PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

        if(child)
        {
            if(i==tab)
            {
                child->hide();
                child->show();
            }
            else
            {
                child->hide();
            }
        }
    }   

    // Reset focus if needed

    if(this->hasFocusChild)
    {
        this->focusSet = false;
        this->walkChildren(PKIconTabControlSetFocusChildrenWalk, this);
    }

    // Change our property, this will redraw

    PKVariantUInt32 sel(tab);
    this->set(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY, &sel);
}

#ifdef LINUX
void PKIconTabControl::drawLinux(GtkWidget   *widget,
                                 GdkDrawable *drawable,
                                 GdkGC       *gc,
                                 PKRect       rect)
{
    PKVariant *s = this->get(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

    int w = rect.w;
    int h = rect.h;

    GdkGCValues values;
    gdk_gc_get_values(gc, &values);

    // TODO: get colors from GTK theme

    // Draw top banner

    GdkColor fg1 = this->layout->style->bg[GTK_STATE_NORMAL];
    
    gdk_gc_set_rgb_fg_color(gc, &fg1);

    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID,
                               GDK_CAP_BUTT, GDK_JOIN_MITER);

    gdk_draw_rectangle(drawable,
                       gc, TRUE, 
                       0, 1, w, 12);                                       
                                       
    // Draw bottom banner
    
    GdkColor fg2;
    fg2.red   = pk_max(0, fg1.red-4000);
    fg2.green = pk_max(0, fg1.green-4000);
    fg2.blue  = pk_max(0, fg1.blue-4000);

    gdk_gc_set_rgb_fg_color(gc, &fg2);

    gdk_draw_rectangle(drawable,
                       gc, TRUE, 
                       0, 13, w, 12);                                       

    // Draw selection

    GdkColor sel1;
    sel1.red   = ((uint32_t)190)*65535/255;
    sel1.green = ((uint32_t)210)*65535/255;
    sel1.blue  = ((uint32_t)235)*65535/255;

    gdk_gc_set_rgb_fg_color(gc, &sel1);

    gdk_draw_rectangle(drawable,
                       gc, TRUE, 
                       sel*25+1, 0, 25, 13);                                       

    GdkColor sel2;
    sel2.red   = ((uint32_t)160)*65535/255;
    sel2.green = ((uint32_t)190)*65535/255;
    sel2.blue  = ((uint32_t)235)*65535/255;

    gdk_gc_set_rgb_fg_color(gc, &sel2);

    gdk_draw_rectangle(drawable,
                       gc, TRUE, 
                       sel*25+1, 13, 25, 13);                                       

    // Draw lines
    
    GdkColor line1;
    line1.red   = ((uint32_t)180)*65535/255;
    line1.green = ((uint32_t)180)*65535/255;
    line1.blue  = ((uint32_t)180)*65535/255;

    GdkColor line2;
    line2.red   = ((uint32_t)198)*65535/255;
    line2.green = ((uint32_t)198)*65535/255;
    line2.blue  = ((uint32_t)198)*65535/255;

    GdkColor line3;
    line3.red   = ((uint32_t)160)*65535/255;
    line3.green = ((uint32_t)160)*65535/255;
    line3.blue  = ((uint32_t)160)*65535/255;

    gdk_gc_set_rgb_fg_color(gc, &line1);

    gdk_draw_line(drawable, gc,
                  0, 0, w, 0);        

    gdk_draw_line(drawable, gc,
                  0, 0, 0, h);        

    gdk_draw_line(drawable, gc,
                  w-1, 0, w-1, h);        

    gdk_draw_line(drawable, gc,
                  0, 25, w, 25);        

    gdk_draw_line(drawable, gc,
                  0, h-1, w, h-1);        

    // Draw separator lines
	
    for(uint32_t i=0; i < this->bitmaps.size(); i++)
    {
        gdk_gc_set_rgb_fg_color(gc, &line2);
        gdk_draw_line(drawable, gc,
                      (i+1)*25, 1, (i+1)*25, 13);        

        gdk_gc_set_rgb_fg_color(gc, &line3);
        gdk_draw_line(drawable, gc,
                      (i+1)*25, 13, (i+1)*25, 25);        
    }
    
    // Draw icons

    for(uint32_t i=0; i < this->bitmaps.size(); i++)
    {
        gdk_draw_pixbuf(drawable, gc, 
                        this->bitmaps[i],
                        0, 0, 5+(i)*25, 5, 16, 16,
                        GDK_RGB_DITHER_NONE, 0, 0);
    }
    
    // Draw selection

    // Revert dc setup changes                                             

    gdk_gc_set_foreground(gc, &values.foreground); 
    
    gdk_gc_set_line_attributes(gc,
                               values.line_width,
                               values.line_style,
                               values.cap_style,
                               values.join_style);
}

#endif

#ifdef WIN32

void PKIconTabControl::drawWin32(PKDrawingDevice device, 
			                     PKRect          rect)
{
    PKVariant *s = this->get(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

    int w = this->getW();
    int h = this->getH();

    PKColor color1;
    PKColor color2;
    PKColor selcolor1;
    PKColor selcolor2;
    PKColor lineColor;

    // Draw bottom background

    HBRUSH bkgBrush = (HBRUSH) (COLOR_BTNFACE + 1);
    
    RECT bkgRect;
    bkgRect.top    = 25;
    bkgRect.left   = 0;
    bkgRect.right  = w;
    bkgRect.bottom = h;

    FillRect(device, &bkgRect, bkgBrush);
    DeleteObject(bkgBrush);

    // Draw top background

	DWORD c1 = GetSysColor(COLOR_3DHIGHLIGHT);
	DWORD c2 = GetSysColor(COLOR_3DFACE);
	DWORD c3 = GetSysColor(COLOR_3DSHADOW);

	color1.r = GetRValue(c1);
	color1.g = GetGValue(c1);
	color1.b = GetBValue(c1);

	color2.r = GetRValue(c2)-10;
	color2.g = GetGValue(c2)-10;
	color2.b = GetBValue(c2)-10;

	selcolor1.r = pk_max(0, GetRValue(c1)-55);
	selcolor1.g = pk_max(0, GetGValue(c1)-35);
	selcolor1.b = pk_min(255, GetBValue(c1)+15);

	selcolor2.r = pk_max(0, GetRValue(c2)-55);
	selcolor2.g = pk_max(0, GetGValue(c2)-35);
	selcolor2.b = pk_min(255, GetBValue(c2)+10);

    lineColor.r = pk_min(255, GetRValue(c3)+20);
	lineColor.g = pk_min(255, GetGValue(c3)+20);
	lineColor.b = pk_min(255, GetBValue(c3)+20);

    HBRUSH topBrush = CreateSolidBrush(RGB(color1.r, color1.g, color1.b));

    RECT topRect;
    topRect.top    = 1;
    topRect.left   = 0;
    topRect.right  = w;
    topRect.bottom = 13;

    FillRect(device, &topRect, topBrush);
    DeleteObject(topBrush);

    HBRUSH bottomBrush = CreateSolidBrush(RGB(color2.r, color2.g, color2.b));

    RECT bottomRect;
    bottomRect.top    = 13;
    bottomRect.left   = 0;
    bottomRect.right  = w;
    bottomRect.bottom = 24;

    FillRect(device, &bottomRect, bottomBrush);
    DeleteObject(bottomBrush);

    HPEN pen  = CreatePen(PS_SOLID, 1, RGB(lineColor.r, lineColor.g, lineColor.b));
    HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(lineColor.r+30, lineColor.g+30, lineColor.b+30));

    HPEN old = (HPEN) SelectObject(device, pen);
    
    MoveToEx(device, 0, 0, NULL);
    LineTo(device, w, 0);

    MoveToEx(device, 0, 0, NULL);
    LineTo(device, 0, h);

    MoveToEx(device, w-1, 0, NULL);
    LineTo(device, w-1, h);

    MoveToEx(device, 0, 24, NULL);
    LineTo(device, w, 24);

    MoveToEx(device, 0, h-1, NULL);
    LineTo(device, w, h-1);

    // Draw selected area

    HBRUSH selBrush1 = CreateSolidBrush(RGB(selcolor1.r, selcolor1.g, selcolor1.b));

    RECT selRect1;
    selRect1.top    = 1;
    selRect1.left   = sel*25+1;
    selRect1.right  = sel*25+25;
    selRect1.bottom = 13;

    FillRect(device, &selRect1, selBrush1);
    DeleteObject(selBrush1);

    HBRUSH selBrush2 = CreateSolidBrush(RGB(selcolor2.r, selcolor2.g, selcolor2.b));

    RECT selRect2;
    selRect2.top    = 13;
    selRect2.left   = sel*25+1;
    selRect2.right  = sel*25+25;
    selRect2.bottom = 24;

    FillRect(device, &selRect2, selBrush2);
    DeleteObject(selBrush2);

    // Draw separator lines

    for(uint32_t i=0; i < this->hicons.size(); i++)
    {
        SelectObject(device, pen2);
        MoveToEx(device, (i+1)*25, 1, NULL);
        LineTo(device, (i+1)*25, 13);
        SelectObject(device, pen);
        MoveToEx(device, (i+1)*25, 13, NULL);
        LineTo(device, (i+1)*25, 24);
    }

    SelectObject(device, old);
    DeleteObject(pen);
    DeleteObject(pen2);

    // Draw icons

    for(uint32_t i=0; i < this->hicons.size(); i++)
    {
        DrawIconEx(device, 
                   5+(i)*25,
                   5, this->hicons[i],
                   16, 16, 0, NULL,
                   DI_NORMAL);
    }
}

#endif

#ifdef MACOSX

void PKIconTabControl::drawMacOSX(RgnHandle	 inLimitRgn,
								  CGContextRef inContext)
{
    PKVariant *s = this->get(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

    CGColorSpaceRef   colorspace;
    colorspace = CGColorSpaceCreateDeviceRGB();
	
    int w = this->getW();
    int h = this->getH();

	CGRect  rect;
	float   line1[4];
	float   line2[4];
	float   line3[4];
	float   bkg1[4];
	float   bkg2[4];
	float   sel1[4];
	float   sel2[4];
	
	bkg1[0] = 230/255.0; bkg1[1] = 230/255.0; bkg1[2] = 230/255.0; bkg1[3] = 1.0;
	bkg2[0] = 210/255.0; bkg2[1] = 210/255.0; bkg2[2] = 210/255.0; bkg2[3] = 1.0;
		
	line1[0] = 169/255.0; line1[1] = 169/255.0; line1[2] = 169/255.0; line1[3] = 1.0;
	line2[0] = 198/255.0; line2[1] = 198/255.0; line2[2] = 198/255.0; line2[3] = 1.0;
	line3[0] = 160/255.0; line3[1] = 160/255.0; line3[2] = 160/255.0; line3[3] = 1.0;

	sel1[0] = 190/255.0; sel1[1] = 210/255.0; sel1[2] = 235/255.0; sel1[3] = 1.0;
	sel2[0] = 160/255.0; sel2[1] = 190/255.0; sel2[2] = 235/255.0; sel2[3] = 1.0;
	
	CGContextSaveGState(inContext);	
	CGContextSetAllowsAntialiasing(inContext, false);
	CGContextSetShouldAntialias(inContext, false);
	CGContextSetFillColorSpace(inContext, colorspace);

	// Top bkg
	rect.origin.x = 0;
	rect.origin.y = 1;
	rect.size.width  = this->getW();
	rect.size.height = 12;	
	CGContextSetFillColor(inContext, bkg1);
	CGContextFillRect(inContext, rect);
	
	// Bottom bkg
	rect.origin.x = 0;
	rect.origin.y = 13;
	rect.size.width  = this->getW();
	rect.size.height = 12;	
	CGContextSetFillColor(inContext, bkg2);
	CGContextFillRect(inContext, rect);

    // Draw selected area
	
	rect.origin.x = sel*25+1;
	rect.origin.y = 13;
	rect.size.width  = 25;
	rect.size.height = 13;	
	CGContextSetFillColor(inContext, sel2);
	CGContextFillRect(inContext, rect);
		
	rect.origin.x = sel*25+1;
	rect.origin.y = 0;
	rect.size.width  = 25;
	rect.size.height = 13;	
	CGContextSetFillColor(inContext, sel1);
	CGContextFillRect(inContext, rect);

	// Lines
	CGContextSetStrokeColorSpace(inContext, colorspace);
	CGContextSetLineWidth(inContext, (float) 1);
	CGContextSetStrokeColor(inContext, line1);
	
	CGPoint points[2];

	/*
	points[0].x = 0;
	points[0].y = 1;
	points[1].x = w;
	points[1].y = 1;
	CGContextStrokeLineSegments(inContext, points, 2);
	*/
	
	points[0].x = 0;
	points[0].y = 0;
	points[1].x = 0;
	points[1].y = h;
	CGContextStrokeLineSegments(inContext, points, 2);

	points[0].x = w;
	points[0].y = 0;
	points[1].x = w;
	points[1].y = h;
	CGContextStrokeLineSegments(inContext, points, 2);

	points[0].x = 0;
	points[0].y = 26;
	points[1].x = w;
	points[1].y = 26;
	CGContextStrokeLineSegments(inContext, points, 2);
	
	/*
	points[0].x = 0;
	points[0].y = h;
	points[1].x = w;
	points[1].y = h;
	CGContextStrokeLineSegments(inContext, points, 2);
	*/
	
    // Draw separator lines
	
    for(uint32_t i=0; i < this->bitmaps.size(); i++)
    {
		points[0].x = (i+1)*25;
		points[0].y = 0;
		points[1].x = (i+1)*25;
		points[1].y = 13;
		CGContextSetStrokeColor(inContext, line2);
		CGContextStrokeLineSegments(inContext, points, 2);

		points[0].x = (i+1)*25;
		points[0].y = 13;
		points[1].x = (i+1)*25;
		points[1].y = 25;
		CGContextSetStrokeColor(inContext, line3);
		CGContextStrokeLineSegments(inContext, points, 2);
    }

	CGContextSetAllowsAntialiasing(inContext, true);
	CGContextSetShouldAntialias(inContext, true);

    // Draw images 
	
    for(uint32_t i=0; i < this->bitmaps.size(); i++)
    {
		CGRect imgRect;
		imgRect.origin.x = 5+(i)*25;
		imgRect.origin.y = 5;
		imgRect.size.width = 16;
		imgRect.size.height = 16;
		CGImageRef img = this->bitmaps[i]->getCGImageRef();
		CGContextDrawImage(inContext, imgRect, img);
    }

	CGContextRestoreGState(inContext);
    CGColorSpaceRelease(colorspace);
}

#endif

void PKIconTabControl::move(int32_t x, 
				            int32_t y)
{
#ifdef LINUX
    PKCustomContainer::move(x, y);
#else
    PKCustomControl::move(x, y);
#endif    

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->move(x, y+24);
		}
	}
}

void PKIconTabControl::resize(uint32_t w, 
					          uint32_t h)
{
#ifdef LINUX
    PKCustomContainer::resize(w, h);
#else
    PKCustomControl::resize(w, h);
#endif    

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->resize(w, h-24);
		}
	}
}

void PKIconTabControl::relayout()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            uint32_t w = child->getW();
            uint32_t h = child->getH();

			child->resize(w, h);
		}
	}

    this->invalidate();
}


void PKIconTabControl::show()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->show();
		}
	}
}

void PKIconTabControl::hide()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->hide();
		}
	}
}

void PKIconTabControl::enable()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->enable();
		}
	}
}

void PKIconTabControl::disable()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->disable();
		}
	}
}

uint32_t PKIconTabControl::getMinimumWidth()
{
    PKVariant *i = this->get(PKIconTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);

    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    {
        return this->getW();
    }

    uint32_t iconW = (icons.size()*24-1);

    uint32_t maxW = 0;

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            uint32_t childW = child->getMinimumWidth();
            if(childW > maxW) maxW = childW;
		}
	}

    return pk_max(maxW, iconW);
}

uint32_t PKIconTabControl::getMinimumHeight()
{
    if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
    {
        return this->getH();
    }

    uint32_t maxH = 0;

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            uint32_t childH = child->getMinimumHeight();
            if(childH > maxH) maxH = childH;
		}
	}

    return maxH + 27;
}

void PKIconTabControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY)
    {
        // Just redraw
        this->invalidate();
    }

#ifdef LINUX
    PKCustomContainer::selfPropertyChanged(prop, oldValue);
#else
    PKCustomControl::selfPropertyChanged(prop, oldValue);
#endif
}

void PKIconTabControl::mouseDown(PKButtonType button,
					             int x,
					             int y,
					             unsigned int mod)
{
    if(y > 25)
    {
        return;
    }

    PKVariant *i = this->get(PKIconTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);

    PKVariant *s = this->get(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

    uint32_t numItems = icons.size();

    uint32_t index = x / 25;

    if(index != sel && index < numItems)
    {
        this->switchToTab(index);
    }
}

void PKIconTabControl::mouseMove(int x,
					             int y,
					             unsigned int mod)
{
    if(y > 25)
    {
#ifdef LINUX
        gdk_window_set_cursor(GTK_LAYOUT(this->layout)->bin_window, NULL);    
#endif
    
        PKCursorTools::setCursor(PK_CURSOR_ARROW);
        return;
    }

    PKVariant *i = this->get(PKIconTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);

    PKVariant *s = this->get(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

    uint32_t numItems = icons.size();

    uint32_t index = x / 25;

    if(index != sel && index < numItems)
    {
        PKCursorTools::setCursor(PK_CURSOR_HAND);
#ifdef LINUX
        gdk_window_set_cursor(GTK_LAYOUT(this->layout)->bin_window, this->hand);    
#endif
    }
    else
    {
        PKCursorTools::setCursor(PK_CURSOR_ARROW);
#ifdef LINUX
        gdk_window_set_cursor(GTK_LAYOUT(this->layout)->bin_window, NULL);    
#endif
    }
}

