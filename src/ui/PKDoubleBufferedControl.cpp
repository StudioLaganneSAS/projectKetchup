//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"
#include "PKBlitter.h"

//
// PROPERTIES
//

std::string PKDoubleBufferedControl::ALPHA_DOUBLE_PROPERTY = "alpha";


//
// PKDoubleBufferedControl
//

PKDoubleBufferedControl::PKDoubleBufferedControl()
{
    PKOBJECT_ADD_DOUBLE_PROPERTY(alpha, 1);

    this->buffer = NULL;

#ifdef WIN32
    this->bmpBrush = 0;
    this->bmp      = 0;
    this->dcBrush  = 0;
#endif

#ifdef LINUX
    this->pix = NULL;
#endif

}

PKDoubleBufferedControl::~PKDoubleBufferedControl()
{
#ifdef WIN32
    
    if(this->dcBrush)
    {
        DeleteDC(this->dcBrush);
    }
    
    if(this->bmpBrush)
    {
        DeleteObject(this->bmpBrush);
    }

    if(this->bmp)
    {
        DeleteObject(this->bmp);
    }

    this->buffer = NULL;

#else

    if(this->buffer != NULL)
    {
        delete [] this->buffer;
    }

#endif

#ifdef LINUX

    if(this->pix != NULL)
	{
	    gdk_pixbuf_unref(this->pix);
	    this->pix = NULL;
	}        
    
#endif

}

void PKDoubleBufferedControl::build()
{
    PKCustomControl::build();
}

// From PKObject
void PKDoubleBufferedControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKDoubleBufferedControl::ALPHA_DOUBLE_PROPERTY)
    {
        this->invalidate();
    }

    PKCustomControl::selfPropertyChanged(prop, oldValue);
}


void PKDoubleBufferedControl::draw32bpp(uint8_t *buffer)
{
    // Subclasses will implement

    int w = this->getW();
    int h = this->getH();

    // Fill the buffer
    if(this->buffer != NULL)
    {
        memset(buffer, 255, w*h*4);
    }
}

void PKDoubleBufferedControl::resize(uint32_t w, 
			                         uint32_t h)
{
    PKCustomControl::resize(w, h);

#ifdef WIN32

    if(this->bmp != NULL)
    {
        DeleteObject(this->bmp);
        
        this->bmp    = NULL;
        this->buffer = NULL;
    }

    BITMAPINFO dibInfo;
    memset(&dibInfo, 0, sizeof(BITMAPINFO));

    dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
    dibInfo.bmiHeader.biWidth         =  w;
    dibInfo.bmiHeader.biHeight        =  - (int) (h+1);
    dibInfo.bmiHeader.biBitCount      =  32;
    dibInfo.bmiHeader.biClrUsed       =  0;
    dibInfo.bmiHeader.biClrImportant  =  0;
    dibInfo.bmiHeader.biCompression   =  BI_RGB;
    dibInfo.bmiHeader.biPlanes        =  1;
    dibInfo.bmiHeader.biSizeImage     =  w*(h+1)*4; // Add some padding
    dibInfo.bmiHeader.biXPelsPerMeter =  0;
    dibInfo.bmiHeader.biYPelsPerMeter =  0;
    
    this->bmp = CreateDIBSection(NULL, &dibInfo, 
                                 DIB_RGB_COLORS, 
                                 (void **) &this->buffer, 
                                 NULL, 0);

#else

    if(this->buffer != NULL)
    {
        delete [] this->buffer;
    }

    this->buffer = new uint8_t[w*h*4];

#endif

#ifdef LINUX

    // Update the pixbuf
    
    if(this->pix != NULL)
	{
	    gdk_pixbuf_unref(this->pix);
	    this->pix = NULL;
	}    
	
    this->pix = gdk_pixbuf_new_from_data((guchar*) this->buffer,
                                         GDK_COLORSPACE_RGB,
                                         TRUE, 8, w, h, 
                                         w*4, 
                                         NULL, NULL);
                                         
#endif


#ifdef WIN32

    if(this->dcBrush)
    {
        DeleteDC(this->dcBrush);
        this->dcBrush = NULL;
    }
    
    if(this->bmpBrush)
    {
        DeleteObject(this->bmpBrush);
        this->bmpBrush = NULL;
    }

#endif
}

void PKDoubleBufferedControl::premultiplyAlpha(PKImage *bitmap)
{
    if(bitmap == NULL)
    {
        return;
    }

#ifdef WIN32

	// alpha premultiply

    uint8_t *pdata = (uint8_t *) bitmap->getPixels();

    if(pdata == NULL)
    {
        return;
    }

    for(unsigned int j=0; j <  bitmap->getHeight(); j++)
	{
		for(unsigned int i=0; i < bitmap->getWidth(); i++)
		{
			if(pdata[3] != 255)
			{
				pdata[0] = (uint8_t) ((int)pdata[0]*(int)pdata[3]/255);
				pdata[1] = (uint8_t) ((int)pdata[1]*(int)pdata[3]/255);
				pdata[2] = (uint8_t) ((int)pdata[2]*(int)pdata[3]/255);
			}
		
			pdata +=4;
		}
	}

#endif
}

#ifdef WIN32

void PKDoubleBufferedControl::drawWin32(PKDrawingDevice device, 
                                        PKRect          rect)
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

    int x = 0;
    int y = 0;

    int w = this->getW();
    int h = this->getH();

    HBRUSH brush = this->getBackgroundBrush();

    if(this->dcBrush == NULL)
    {
        this->dcBrush = CreateCompatibleDC(device);
    }

    if(this->bmpBrush == NULL)
    {
        this->bmpBrush = CreateCompatibleBitmap(device, w, h);
    }
    
    HGDIOBJ old = SelectObject(dcBrush, bmpBrush);

    if(this->bmpBrush == NULL)
    {
	    return;
    }

    if(brush == NULL)
    {
        // Fill directly trough parent

        PKControl *parentControl = dynamic_cast<PKControl*>(parentView);

        if(parentControl)
        {
            int ax = this->getX();
            int ay = this->getY();

            parentControl->fillDeviceWithBackground(dcBrush,
                                                    ax, ay, 
                                                    w, h);
        }
    }
    else
    {
        // Fill with brush

        RECT r;

        r.left   = rect.x;
        r.right  = rect.x + rect.w;
        r.top    = rect.y;
        r.bottom = rect.y + rect.h;

        int ax = this->getX();
        int ay = this->getY();

        SetBrushOrgEx(dcBrush, 
                      -ax, 
                      -ay, 
                      NULL);

        HBRUSH oldb = (HBRUSH) SelectObject(dcBrush, brush);
        FillRect(dcBrush, &r, brush);
        
        SetBrushOrgEx(dcBrush, 0, 0, NULL);
        SelectObject(dcBrush, oldb);
    }

    // Paint our ARGB buffer

    if(this->buffer == NULL)
    {
        return;
    }

    memset(this->buffer, 0, w*h*4);
    this->draw32bpp(this->buffer);

    // Alpha-Blit our buffer to the mem DC

    if(this->bmp == NULL)
    {
	    return;
    }
    
    PKVariant *a = this->get(PKDoubleBufferedControl::ALPHA_DOUBLE_PROPERTY);
    double alpha = PKVALUE_DOUBLE(a);

    BYTE al = (BYTE) ((double) ((double) alpha) * 255.0);

    if(alpha == 1)
    {
        al = 255;
    }

    HDC dcAlpha = CreateCompatibleDC(device);
    HBITMAP old2 = (HBITMAP) SelectObject(dcAlpha, bmp);

    BLENDFUNCTION blendfunction = { AC_SRC_OVER, 0, al, AC_SRC_ALPHA };

    AlphaBlend(this->dcBrush, 0, 0, w, h, 
		       dcAlpha, 0, 0, w, h,
		       blendfunction);

    SelectObject(dcAlpha, old2);

    // And finally, blit to screen

    BitBlt(device, x, y, w, h, this->dcBrush, 0, 0, SRCCOPY);

    // Cleanup

    SelectObject(this->dcBrush, old);
}

#endif

#ifdef LINUX
void PKDoubleBufferedControl::drawLinux(GtkWidget   *widget,
                                        GdkDrawable *drawable,
                                        GdkGC       *gc,
                                        PKRect       rect)
{
    if(this->buffer == NULL || this->pix == NULL)
    {
        return;
    }
    	
    int w = this->getW();
    int h = this->getH();

    PKVariant *a = this->get(PKDoubleBufferedControl::ALPHA_DOUBLE_PROPERTY);
    double alpha = PKVALUE_DOUBLE(a);

    // Paint our ARGB buffer
	
    memset(this->buffer, 0, w*h*4);
    this->draw32bpp(this->buffer);

	// Invert R & B
	
	uint8_t *pdata = (uint8_t *) this->buffer;

	for(unsigned int j=0; j < h; j++)
	{
		for(unsigned int i=0; i < w; i++)
		{
		    uint8_t temp = pdata[0];
		     
			pdata[0] = pdata[2];
			pdata[2] = temp;

			pdata +=4;
		}
	}

    // Now display on the widget
    
    gdk_pixbuf_render_to_drawable_alpha(this->pix, drawable, 
                                        0, 0, 0, 0, w, h, 
                                        GDK_PIXBUF_ALPHA_FULL,
                                        0, GDK_RGB_DITHER_NONE, 0, 0);     
}                                        
#endif


#ifdef MACOSX

void PKDoubleBufferedControl::drawMacOSX(RgnHandle	  inLimitRgn,
								         CGContextRef context)
{	
    int w = this->getW();
    int h = this->getH();

    // Paint our ARGB buffer
	
    memset(this->buffer, 0, w*h*4);
    this->draw32bpp(this->buffer);

    // Blit our buffer to screen

    CGDataProviderRef provider;
    CGColorSpaceRef   colorspace;
    CGImageRef        image;

    CGContextSaveGState (context);
    CGContextSetInterpolationQuality(context, kCGInterpolationHigh);

	CGContextScaleCTM(context, 1.0, -1.0);
	CGContextTranslateCTM(context, 0, -h);

    PKVariant *a = this->get(PKDoubleBufferedControl::ALPHA_DOUBLE_PROPERTY);
    double alpha = PKVALUE_DOUBLE(a);

	CGContextSetAlpha(context, (float) alpha);
	
    colorspace = CGColorSpaceCreateDeviceRGB();
    provider   = CGDataProviderCreateWithData(NULL, this->buffer, w*h*4, NULL);
    
    CGRect r;
    
    r.origin.x    = 0;
    r.origin.y    = 0;
    r.size.width  = w;
    r.size.height = h;

    image = CGImageCreate(w, h, 8, 32, w*4, 
                          colorspace,
                          kCGImageAlphaFirst,
                          provider,
                          NULL, false,
                          kCGRenderingIntentDefault);
    
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorspace);
	
    CGContextDrawImage(context, r, image);
    CGImageRelease(image);
    
    CGContextRestoreGState(context);                	
}

#endif

