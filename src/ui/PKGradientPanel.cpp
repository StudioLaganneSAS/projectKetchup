//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKGradientPanel.h"
#include "PKControl.h"

#ifdef LINUX
#include <cairo.h>
#endif

//
// Properties
//

std::string PKGradientPanel::BORDER_COLOR_PROPERTY       = "borderColor";
std::string PKGradientPanel::BACKGROUND_COLOR1_PROPERTY  = "backgroundColor1";
std::string PKGradientPanel::BACKGROUND_COLOR2_PROPERTY  = "backgroundColor2";
std::string PKGradientPanel::BORDER_SIZE_UINT32_PROPERTY = "borderSize";
std::string PKGradientPanel::HORIZONTAL_BOOL_PROPERTY    = "horizontal";

//
// PKPanelControl
//

PKGradientPanel::PKGradientPanel()
{
    PKColor c  = PKColorCreate(0, 0, 0, 255);
    PKColor b1 = PKColorCreate(255, 255, 255, 255);
    PKColor b2 = PKColorCreate(128, 128, 128, 255);

    PKOBJECT_ADD_COLOR_PROPERTY(borderColor, c);
    PKOBJECT_ADD_COLOR_PROPERTY(backgroundColor1, b1);
    PKOBJECT_ADD_COLOR_PROPERTY(backgroundColor2, b2);
    PKOBJECT_ADD_BOOL_PROPERTY(horizontal, false);

    PKOBJECT_ADD_UINT32_PROPERTY(borderSize, 1);

#ifdef WIN32

    bkg = NULL;

#endif
}

PKGradientPanel::~PKGradientPanel()
{
#ifdef WIN32

    if(bkg != NULL)
    {
        DeleteObject(bkg);
    }

#endif
}

void PKGradientPanel::build()
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
#ifdef LINUX

            GtkWidget *w = root->getWindowHandle();

            GtkRequisition req;
            gtk_widget_size_request(w, &req);

            gtk_layout_set_size(GTK_LAYOUT(this->layout), req.width, req.height);
            gtk_widget_set_size_request(this->layout, req.width, req.height);
            gtk_widget_set_size_request(this->widget, req.width, req.height);

            gtk_layout_put(GTK_LAYOUT(this->layout),
                           w, 0, 0);
                       
#endif		
		}
	}
}

void PKGradientPanel::initialize(PKDialog *dialog)
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->initialize(dialog);
		}
	}
	
#ifdef LINUX
    PKCustomContainer::initialize(dialog);
#else
    PKCustomControl::initialize(dialog);
#endif // LINUX

#ifdef LINUX

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(root)
		{
            GtkWidget *w = root->getWindowHandle();

            GtkRequisition req;
            gtk_widget_size_request(w, &req);

            gtk_layout_set_size(GTK_LAYOUT(this->layout), req.width, req.height);
            gtk_widget_set_size_request(this->layout, req.width, req.height);
            gtk_widget_set_size_request(this->widget, req.width, req.height);
        }
    }
#endif // LINUX
}

void PKGradientPanel::destroy()
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


PKWindowHandle PKGradientPanel::getWindowHandle()
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

#ifdef WIN32

HBRUSH PKGradientPanel::getBackgroundBrush()
{
    if(bkg == NULL)
    {
        // Get our DC
        HDC hDC = GetDC(this->hwnd);

        // Create a compatible DC
        HDC hDCMem = CreateCompatibleDC(hDC);

        HBITMAP hBmp = CreateCompatibleBitmap(hDC, 
                                              this->getW(), 
											  this->getH());

        HBITMAP hBmpOld = (HBITMAP)SelectObject(hDCMem, hBmp);

        // Tell the tab control to paint in our DC

        SendMessage(this->hwnd, 
                    WM_PRINTCLIENT, 
                    (WPARAM)(hDCMem), 
                    (LPARAM)(PRF_CLIENT));

        // Create a pattern brush from 
		// the bitmap selected in our DC

		bkg = CreatePatternBrush(hBmp);

        // Restore the bitmap
        SelectObject(hDCMem, hBmpOld);

        // Cleanup
        DeleteObject(hBmp);
        DeleteDC(hDCMem);
        ReleaseDC(this->hwnd, hDC);		
    }

    return bkg;
}

bool PKGradientPanel::drawsItsBackground()
{
    return true;
}

void PKGradientPanel::drawWin32(PKDrawingDevice device, 
                               PKRect          rect)
{
    PKVariant *bc = this->get(PKGradientPanel::BORDER_COLOR_PROPERTY);
    PKColor borderColor = PKVALUE_COLOR(bc);

    PKVariant *bk1 = this->get(PKGradientPanel::BACKGROUND_COLOR1_PROPERTY);  
    PKColor bkgColor1 = PKVALUE_COLOR(bk1);

    PKVariant *bk2 = this->get(PKGradientPanel::BACKGROUND_COLOR2_PROPERTY);  
    PKColor bkgColor2 = PKVALUE_COLOR(bk2);

    PKVariant *bs = this->get(PKGradientPanel::BORDER_SIZE_UINT32_PROPERTY);
    uint32_t borderSize = PKVALUE_UINT32(bs);

    PKVariant *hv = this->get(PKGradientPanel::HORIZONTAL_BOOL_PROPERTY);
    bool horizontal = PKVALUE_BOOL(hv);

    // Do the border if necessary

    int x = 0;
    int y = 0;
    int w = this->getW();
    int h = this->getH();

    if(borderSize > 0)
    {
        HBRUSH brush = (HBRUSH) GetStockObject(HOLLOW_BRUSH);
        HBRUSH oldBrush = (HBRUSH) SelectObject(device, brush);

        HPEN pen;
        HPEN oldPen;

        pen = CreatePen(PS_SOLID, borderSize, 
                        RGB(borderColor.r, borderColor.g, borderColor.b));

        oldPen = (HPEN) SelectObject(device, pen);

        Rectangle(device, x, y, w, h);

        SelectObject(device, oldPen);
        SelectObject(device, oldBrush);

        DeleteObject(pen);
    }

    // Then draw the gradient

    TRIVERTEX  vertexes[2];
    GRADIENT_RECT grect;

    uint32_t r1 = bkgColor1.r*0xFF00/0xFF;
    uint32_t g1 = bkgColor1.g*0xFF00/0xFF;
    uint32_t b1 = bkgColor1.b*0xFF00/0xFF;

    uint32_t r2 = bkgColor2.r*0xFF00/0xFF;
    uint32_t g2 = bkgColor2.g*0xFF00/0xFF;
    uint32_t b2 = bkgColor2.b*0xFF00/0xFF;

    vertexes[0].x = borderSize;
    vertexes[0].y = borderSize;
    vertexes[0].Red   = r1;
    vertexes[0].Green = g1;
    vertexes[0].Blue  = b1;
    vertexes[0].Alpha = 0xFF00;

    vertexes[1].x = w - borderSize;
    vertexes[1].y = h - borderSize;
    vertexes[1].Red   = r2;
    vertexes[1].Green = g2;
    vertexes[1].Blue  = b2;
    vertexes[1].Alpha = 0xFF00;

    grect.UpperLeft  = 0;
    grect.LowerRight = 1;

    GradientFill(device, vertexes, 2, &grect, 1, 
                (horizontal ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V));
}

#endif

#ifdef LINUX
void PKGradientPanel::drawLinux(GtkWidget   *widget,
                                GdkDrawable *drawable,
                                GdkGC       *gc,
                                PKRect       rect)
{
    PKVariant *bc = this->get(PKGradientPanel::BORDER_COLOR_PROPERTY);
    PKColor borderColor = PKVALUE_COLOR(bc);

    PKVariant *bk1 = this->get(PKGradientPanel::BACKGROUND_COLOR1_PROPERTY);  
    PKColor bkgColor1 = PKVALUE_COLOR(bk1);

    PKVariant *bk2 = this->get(PKGradientPanel::BACKGROUND_COLOR2_PROPERTY);  
    PKColor bkgColor2 = PKVALUE_COLOR(bk2);

    PKVariant *bs = this->get(PKGradientPanel::BORDER_SIZE_UINT32_PROPERTY);
    uint32_t borderSize = PKVALUE_UINT32(bs);

    PKVariant *hv = this->get(PKGradientPanel::HORIZONTAL_BOOL_PROPERTY);
    bool horizontal = PKVALUE_BOOL(hv);

    cairo_t *cr = gdk_cairo_create(drawable);
    
    cairo_reset_clip(cr);
    
    // Draw background
    
    cairo_pattern_t *gradient;
    
    if(horizontal)
    {
        gradient = cairo_pattern_create_linear ((double) rect.x, 
                                                (double) rect.y,
                                                (double) rect.x+rect.w,
                                                (double) rect.y);
    }
    else
    {
        gradient = cairo_pattern_create_linear ((double) rect.x, 
                                                (double) rect.y,
                                                (double) rect.x,
                                                (double) rect.y+rect.h);
    }    
                                            
    cairo_pattern_add_color_stop_rgba (gradient, 0, 
                                       (double) bkgColor1.r/255.0, 
                                       (double) bkgColor1.g/255.0,
                                       (double) bkgColor1.b/255.0,
                                       (double) bkgColor1.a/255.0);

    cairo_pattern_add_color_stop_rgba (gradient, 1.00, 
                                       (double) bkgColor2.r/255.0, 
                                       (double) bkgColor2.g/255.0,
                                       (double) bkgColor2.b/255.0,
                                       (double) bkgColor2.a/255.0);

    cairo_rectangle(cr, rect.x, rect.y, rect.w, rect.h);
    cairo_set_source(cr, gradient);
    cairo_fill(cr);
    
    cairo_pattern_destroy(gradient);
    
    if(borderSize > 0)
    {
        // Draw border
    }
    
    cairo_destroy(cr);
}
#endif


#ifdef MACOSX

void PKGradientPanel::drawMacOSX(RgnHandle	 inLimitRgn,
								CGContextRef inContext)
{
	// Commented out since this is only available in MacOSX 10.5 !
	
	// TODO: Find an alternative, this is Leopard+ only
		
    PKVariant *bc = this->get(PKGradientPanel::BORDER_COLOR_PROPERTY);
    PKColor borderColor = PKVALUE_COLOR(bc);
	
    PKVariant *bk1 = this->get(PKGradientPanel::BACKGROUND_COLOR1_PROPERTY);  
    PKColor bkgColor1 = PKVALUE_COLOR(bk1);

    PKVariant *bk2 = this->get(PKGradientPanel::BACKGROUND_COLOR2_PROPERTY);  
    PKColor bkgColor2 = PKVALUE_COLOR(bk2);
	
    PKVariant *bs = this->get(PKGradientPanel::BORDER_SIZE_UINT32_PROPERTY);
    uint32_t borderSize = PKVALUE_UINT32(bs);
	
    PKVariant *hv = this->get(PKGradientPanel::HORIZONTAL_BOOL_PROPERTY);
    bool horizontal = PKVALUE_BOOL(hv);
	
    int w = this->getW();
    int h = this->getH();

    CGColorSpaceRef   colorspace;
    colorspace = CGColorSpaceCreateDeviceRGB();

	CGRect  rect;
	float   bkg[4];
	float   brd[4];
	
	bkg[0] = (float)bkgColor1.r / 255.0;
	bkg[1] = (float)bkgColor1.g / 255.0;
	bkg[2] = (float)bkgColor1.b / 255.0;
	bkg[3] = (float)bkgColor1.a / 255.0;
	
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

	CGPoint start;
	CGPoint end;
	
	if(horizontal)
	{
		start.x = 0;
		start.y = 0;
		end.x   = w;
		end.y   = 0;
	}
	else
	{
		start.x = 0;
		start.y = 0;
		end.x   = 0;
		end.y   = h;
	}	
	
#ifdef OSX_10_5
	
	CGFloat colors[8];
	CGFloat locations[] = {0, 1};	

	colors[0] = (float)bkgColor1.r / 255.0;
	colors[1] = (float)bkgColor1.g / 255.0;
	colors[2] = (float)bkgColor1.b / 255.0;
	colors[3] = (float)bkgColor1.a / 255.0;

	colors[4] = (float)bkgColor2.r / 255.0;
	colors[5] = (float)bkgColor2.g / 255.0;
	colors[6] = (float)bkgColor2.b / 255.0;
	colors[7] = (float)bkgColor2.a / 255.0;
	
	CGGradientRef grad = CGGradientCreateWithColorComponents(colorspace, colors, locations, 2);
	
	CGContextDrawLinearGradient(inContext, grad, start, end, 0);
	CGContextStrokeRect(inContext, rect);

	CGGradientRelease(grad);
#else
	
	// TODO: for Tiger
	
#endif
	
	CGContextRestoreGState(inContext);
    CGColorSpaceRelease(colorspace);
}

#endif

void PKGradientPanel::move(int32_t x, 
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

void PKGradientPanel::resize(uint32_t w, 
					         uint32_t h)
{
	PKControl::resize(w, h);

#ifdef WIN32

    if(bkg != NULL)
    {
        DeleteObject((HGDIOBJ)bkg);
        bkg = NULL; // Force re-creation
    }

#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->resize(w, h);
		}
	}
}

void PKGradientPanel::show()
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

void PKGradientPanel::hide()
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

void PKGradientPanel::enable()
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

void PKGradientPanel::disable()
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

uint32_t PKGradientPanel::getMinimumWidth()
{
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

uint32_t PKGradientPanel::getMinimumHeight()
{
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
