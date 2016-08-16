//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKLineControl.h"
#include "PKLayout.h"

//
// Properties
//

std::string PKLineControl::COLOR_COLOR_PROPERTY     = "color";
std::string PKLineControl::HORIZONTAL_BOOL_PROPERTY = "horizontal";

//
// PKPanelControl
//

PKLineControl::PKLineControl()
{
    PKColor c = PKColorCreate(0, 0, 0, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(color, c);
    PKOBJECT_ADD_BOOL_PROPERTY(horizontal, true);

	this->setLayoutPolicy(PK_LAYOUT_MAX,
						  PK_LAYOUT_MIN);
}

PKLineControl::~PKLineControl()
{
}

void PKLineControl::build()
{
    PKCustomControl::build();

#ifdef LINUX

    PKVariant *h = this->get(PKLineControl::HORIZONTAL_BOOL_PROPERTY);
    bool horiz   = PKVALUE_BOOL(h);
    
    if(horiz)
    {
        gtk_widget_set_size_request(this->widget, -1, 1);        
    }    
    else    
    {
        gtk_widget_set_size_request(this->widget, 1, -1);        
    }    

#endif
}


#ifdef WIN32

void PKLineControl::drawWin32(PKDrawingDevice device, 
                              PKRect          rect)
{
    PKVariant *c = this->get(PKLineControl::COLOR_COLOR_PROPERTY);
    PKColor color = PKVALUE_COLOR(c);

    HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
    HBRUSH oldBrush = (HBRUSH) SelectObject(device, brush);

    HPEN pen;
    HPEN oldPen;

    pen = CreatePen(PS_SOLID, 1, 
                RGB(color.r, color.g, color.b));

    oldPen = (HPEN) SelectObject(device, pen);

    int x = 0;
    int y = 0;
    int w = this->getW();
    int h = this->getH();

    Rectangle(device, x, y, w, h);

    SelectObject(device, oldPen);
    SelectObject(device, oldBrush);

    DeleteObject(brush);
    DeleteObject(pen);
}

#endif

#ifdef LINUX
void PKLineControl::drawLinux(GtkWidget   *widget,
                              GdkDrawable *drawable,
                              GdkGC       *gc,
                              PKRect       rect)    
{
    PKVariant *c = this->get(PKLineControl::COLOR_COLOR_PROPERTY);
    PKColor color = PKVALUE_COLOR(c);

    GdkColor fg;
    fg.red   = ((uint32_t)color.r)*65535/255;
    fg.green = ((uint32_t)color.g)*65535/255;
    fg.blue  = ((uint32_t)color.b)*65535/255;

    GdkGCValues values;
    gdk_gc_get_values(gc, &values);

    gdk_gc_set_rgb_fg_color(gc, &fg);

    GtkAllocation all;
    gtk_widget_get_allocation(widget, &all);

    int x = 0;
    int y = 0;
    int w = all.width;
    int h = all.height;

    gdk_draw_rectangle(drawable,
                       gc, TRUE, 
                       x, y, w, h);                               

    gdk_gc_set_foreground(gc, &values.foreground);    
}                                
#endif


#ifdef MACOSX

void PKLineControl::drawMacOSX(RgnHandle	 inLimitRgn,
							   CGContextRef inContext)
{
    PKVariant *bc = this->get(PKLineControl::COLOR_COLOR_PROPERTY);
    PKColor color = PKVALUE_COLOR(bc);
	
    CGColorSpaceRef   colorspace;
    colorspace = CGColorSpaceCreateDeviceRGB();

	CGRect  rect;
	float   bkg[4];
	
	bkg[0] = (float)color.r / 255.0;
	bkg[1] = (float)color.g / 255.0;
	bkg[2] = (float)color.b / 255.0;
	bkg[3] = (float)color.a / 255.0;
	
	rect.origin.x = 0;
	rect.origin.y = 0;
	
	rect.size.width  = this->getW();
	rect.size.height = this->getH();
	
	CGContextSaveGState(inContext);
	
	CGContextSetFillColorSpace(inContext, colorspace);
	CGContextSetFillColor(inContext, bkg);
	CGContextFillRect(inContext, rect);

	CGContextRestoreGState(inContext);

    CGColorSpaceRelease(colorspace);
}

#endif

uint32_t PKLineControl::getMinimumWidth()
{
    PKVariant *h = this->get(PKLineControl::HORIZONTAL_BOOL_PROPERTY);
    bool horiz   = PKVALUE_BOOL(h);

    if(!horiz)
    {
        return 1;
    }

	return 0;
}

uint32_t PKLineControl::getMinimumHeight()
{
    PKVariant *h = this->get(PKLineControl::HORIZONTAL_BOOL_PROPERTY);
    bool horiz   = PKVALUE_BOOL(h);

    if(horiz)
    {
        return 1;
    }

	return 0;
}
