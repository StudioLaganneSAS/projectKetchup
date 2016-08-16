//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKPanelControl.h"

//
// Properties
//

std::string PKPanelControl::BORDER_COLOR_PROPERTY       = "borderColor";
std::string PKPanelControl::BACKGROUND_COLOR_PROPERTY   = "backgroundColor";
std::string PKPanelControl::BORDER_SIZE_UINT32_PROPERTY = "borderSize";

//
// PKPanelControl
//

PKPanelControl::PKPanelControl()
{
    PKColor c = PKColorCreate(0, 0, 0, 255);
    PKColor b = PKColorCreate(255, 255, 255, 255);

    PKOBJECT_ADD_COLOR_PROPERTY(borderColor, c);
    PKOBJECT_ADD_COLOR_PROPERTY(backgroundColor, b);

    PKOBJECT_ADD_UINT32_PROPERTY(borderSize, 1);

#ifdef WIN32
    bkg = NULL;
#endif
}

PKPanelControl::~PKPanelControl()
{
#ifdef WIN32
    if(bkg != NULL)
    {
        DeleteObject(bkg);
    }
#endif
}

void PKPanelControl::build()
{
#ifdef LINUX
    PKCustomContainer::build();
#else
    PKCustomControl::build();
#endif // LINUX

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->build();
		}
	}
}

void PKPanelControl::initialize(PKDialog *dialog)
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->initialize(dialog);

#ifdef LINUX

            GtkWidget *w = root->getWindowHandle();

            gtk_layout_put(GTK_LAYOUT(this->layout),
                           w, 0, 0);

#endif // LINUX

		}
	}
	
#ifdef LINUX
    PKCustomContainer::initialize(dialog);
#else
    PKCustomControl::initialize(dialog);
#endif // LINUX

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));
	
#ifdef LINUX

        // TODO: we need to be able to update this, if our children size changes !

        GtkWidget *w = root->getWindowHandle();

        GtkRequisition req;
        gtk_widget_size_request(w, &req);

        gtk_layout_set_size(GTK_LAYOUT(this->layout), req.width, req.height);
        gtk_widget_set_size_request(this->layout, req.width, req.height);
        gtk_widget_set_size_request(this->widget, req.width, req.height);

        GtkAllocation alloc;
        alloc.x = alloc.y = 0;
        
        alloc.width  = req.width;
        alloc.height = req.height;

        gtk_widget_size_allocate(w, &alloc);
                       
#endif		
	}
}

void PKPanelControl::destroy()
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->destroy();
            root = NULL;
		}
	}
    this->clearChildren();

#ifdef LINUX
    PKCustomContainer::destroy();
#else
    PKCustomControl::destroy();
#endif
}


PKWindowHandle PKPanelControl::getWindowHandle()
{
	// We forward to our parent

#ifdef LINUX
    return this->widget;
#endif

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

#ifdef WIN32

HBRUSH PKPanelControl::getBackgroundBrush()
{
    if(bkg == NULL)
    {
        PKVariant *bk = this->get(PKPanelControl::BACKGROUND_COLOR_PROPERTY);  
        PKColor bkgColor = PKVALUE_COLOR(bk);

        bkg = CreateSolidBrush(RGB(bkgColor.r, bkgColor.g, bkgColor.b));
    }

    return bkg;
}

bool PKPanelControl::drawsItsBackground()
{
    return true;
}

void PKPanelControl::drawWin32(PKDrawingDevice device, 
                               PKRect          rect)
{
    PKVariant *bc = this->get(PKPanelControl::BORDER_COLOR_PROPERTY);
    PKColor borderColor = PKVALUE_COLOR(bc);

    PKVariant *bk = this->get(PKPanelControl::BACKGROUND_COLOR_PROPERTY);  
    PKColor bkgColor = PKVALUE_COLOR(bk);

    PKVariant *bs = this->get(PKPanelControl::BORDER_SIZE_UINT32_PROPERTY);
    uint32_t borderSize = PKVALUE_UINT32(bs);

    HBRUSH brush = CreateSolidBrush(RGB(bkgColor.r, bkgColor.g, bkgColor.b));
    HBRUSH oldBrush = (HBRUSH) SelectObject(device, brush);

    HPEN pen;
    HPEN oldPen;

    int borderOffset = 0;

    if(borderSize == 0)
    {
        pen = CreatePen(PS_SOLID, 1, 
                    RGB(bkgColor.r, bkgColor.g, bkgColor.b));
    }
    else
    {
        borderOffset = borderSize/2;
        pen = CreatePen(PS_SOLID, borderSize, 
                    RGB(borderColor.r, borderColor.g, borderColor.b));
    }

    oldPen = (HPEN) SelectObject(device, pen);

    int x = 0;
    int y = 0;
    int w = this->getW();
    int h = this->getH();

    Rectangle(device, x+borderOffset, y+borderOffset, 
		      w-borderOffset+(borderSize%2?0:1), 
			  h-borderOffset+(borderSize%2?0:1));

    SelectObject(device, oldPen);
    SelectObject(device, oldBrush);

    DeleteObject(brush);
    DeleteObject(pen);
}

#endif

#ifdef LINUX
void PKPanelControl::drawLinux(GtkWidget   *widget,
                               GdkDrawable *drawable,
                               GdkGC       *gc,
                               PKRect       rect)
{
    PKVariant *bc = this->get(PKPanelControl::BORDER_COLOR_PROPERTY);
    PKColor borderColor = PKVALUE_COLOR(bc);
	
    PKVariant *bk = this->get(PKPanelControl::BACKGROUND_COLOR_PROPERTY);  
    PKColor bkgColor = PKVALUE_COLOR(bk);
	
    PKVariant *bs = this->get(PKPanelControl::BORDER_SIZE_UINT32_PROPERTY);
    uint32_t borderSize = PKVALUE_UINT32(bs);

    GdkColor old;
    GdkColor fg;
    fg.red   = ((uint32_t)bkgColor.r)*65535/255;
    fg.green = ((uint32_t)bkgColor.g)*65535/255;
    fg.blue  = ((uint32_t)bkgColor.b)*65535/255;

    GdkGCValues values;
    gdk_gc_get_values(gc, &values);

    // Draw background

    gdk_gc_set_rgb_fg_color(gc, &fg);

    gdk_draw_rectangle(drawable,
                       gc, TRUE, 
                       rect.x, rect.y, rect.w, rect.h);                               

    // Draw border                                          

    if(borderSize > 0)
    {
        GdkColor fg2;
        fg2.red   = ((uint32_t)borderColor.r)*65535/255;
        fg2.green = ((uint32_t)borderColor.g)*65535/255;
        fg2.blue  = ((uint32_t)borderColor.b)*65535/255;
        
        gdk_gc_set_line_attributes(gc, borderSize, GDK_LINE_SOLID,
                                   GDK_CAP_BUTT, GDK_JOIN_MITER);
            
        gdk_gc_set_rgb_fg_color(gc, &fg2);
        
        gdk_draw_rectangle(drawable,
                           gc, FALSE, 
                           rect.x, rect.y, rect.w-1, rect.h-1);                                       
    }

    // Revert

    gdk_gc_set_foreground(gc, &values.foreground); 
    
    gdk_gc_set_line_attributes(gc,
                               values.line_width,
                               values.line_style,
                               values.cap_style,
                               values.join_style);

}
#endif


#ifdef MACOSX

void PKPanelControl::drawMacOSX(RgnHandle	 inLimitRgn,
								CGContextRef inContext)
{
    PKVariant *bc = this->get(PKPanelControl::BORDER_COLOR_PROPERTY);
    PKColor borderColor = PKVALUE_COLOR(bc);
	
    PKVariant *bk = this->get(PKPanelControl::BACKGROUND_COLOR_PROPERTY);  
    PKColor bkgColor = PKVALUE_COLOR(bk);
	
    PKVariant *bs = this->get(PKPanelControl::BORDER_SIZE_UINT32_PROPERTY);
    uint32_t borderSize = PKVALUE_UINT32(bs);
	
    CGColorSpaceRef   colorspace;
    colorspace = CGColorSpaceCreateDeviceRGB();

	CGRect  rect;
	float   bkg[4];
	float   brd[4];
	
	bkg[0] = (float)bkgColor.r / 255.0;
	bkg[1] = (float)bkgColor.g / 255.0;
	bkg[2] = (float)bkgColor.b / 255.0;
	bkg[3] = (float)bkgColor.a / 255.0;
	
	brd[0] = (float)borderColor.r / 255.0;
	brd[1] = (float)borderColor.g / 255.0;
	brd[2] = (float)borderColor.b / 255.0;
	brd[3] = (float)borderColor.a / 255.0;

	rect.origin.x = 0 + borderSize;
	rect.origin.y = 0 + borderSize;
	
	rect.size.width  = this->getW() - 2*borderSize;
	rect.size.height = this->getH() - 2*borderSize;
	
	CGContextSaveGState(inContext);
	
	CGContextSetFillColorSpace(inContext, colorspace);
	CGContextSetFillColor(inContext, bkg);
	CGContextSetStrokeColorSpace(inContext, colorspace);
	CGContextSetStrokeColor(inContext, brd);
	CGContextSetLineWidth(inContext, (float) borderSize);
	CGContextFillRect(inContext, rect);
	CGContextStrokeRect(inContext, rect);

	CGContextRestoreGState(inContext);

    CGColorSpaceRelease(colorspace);
}

#endif

void PKPanelControl::move(int32_t x, 
					      int32_t y)
{
	PKControl::move(x, y);

	if(this->getChildrenCount() == 1)
	{
	    PKControl *ctrl = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(ctrl)
        {
			ctrl->move(x, y);
        }
	}
}

void PKPanelControl::resize(uint32_t w, 
					        uint32_t h)
{
	PKControl::resize(w, h);

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->resize(w, h);
		}
	}
}

void PKPanelControl::show()
{
	PKControl::show();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->show();
		}
	}
}

void PKPanelControl::hide()
{
	PKControl::hide();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->hide();			
		}
	}
}

void PKPanelControl::enable()
{
	PKControl::enable();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->enable();
		}
	}
}

void PKPanelControl::disable()
{
	PKControl::disable();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->disable();
		}
	}
}

uint32_t PKPanelControl::getMinimumWidth()
{
    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    {
        return this->getW();
    }

    if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumWidth());
		}
	}

	return 0;
}

uint32_t PKPanelControl::getMinimumHeight()
{
    if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
    {
        return this->getH();
    }

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumHeight());
		}
	}

	return 0;
}
