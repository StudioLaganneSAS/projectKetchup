//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKBlitter.h"

//
// PKBlitter
//

void PKBlitter::blitRGB32At(PKDrawingDevice device, 
						    void *bitmap,
						    unsigned int x,
						    unsigned int y,
						    unsigned int w,
						    unsigned int h)
{
    if(device == NULL)
    {
        return;
    }
    
	if(bitmap == NULL)
	{
		return;
	}

	char *pixels = (char *) bitmap;

#ifdef WIN32

	HDC     dc   = device;

	HBITMAP bmp  = CreateCompatibleBitmap(dc, w, h);
	HDC     bdc  = CreateCompatibleDC(dc);

	if(bmp == NULL)
	{
		return;
	}

	BITMAPINFO dibInfo;
	memset(&dibInfo, 0, sizeof(BITMAPINFO));

	dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
	dibInfo.bmiHeader.biWidth         =  w;
	dibInfo.bmiHeader.biHeight        =  - (int) h;
	dibInfo.bmiHeader.biBitCount      =  32;
	dibInfo.bmiHeader.biClrUsed       =  0;
	dibInfo.bmiHeader.biClrImportant  =  0;
	dibInfo.bmiHeader.biCompression   =  BI_RGB;
	dibInfo.bmiHeader.biPlanes        =  1;
	dibInfo.bmiHeader.biSizeImage     =  w*h*4;
	dibInfo.bmiHeader.biXPelsPerMeter =  0;
	dibInfo.bmiHeader.biYPelsPerMeter =  0;

	HGDIOBJ old = SelectObject(bdc, bmp);

	SetDIBits(bdc, bmp, 0, h, pixels, &dibInfo, 0);
	BitBlt(dc, x, y, w, h, bdc, 0, 0, SRCCOPY);

	SelectObject(bdc, old);
	DeleteObject(bmp);
	DeleteDC(bdc);

#endif

#ifdef MACOSX

	//
	// Async blitting to screen
	// should not be done on the mac
	// all drawing needs to go 
	// through the main event loop...
	// Only calls this when you know
    // that it is thread safe...
    //

    CGrafPtr oldPort;
    CGrafPtr mPort = device;
    
    Rect clip;
    GetPort(&oldPort);    
    SetPort(mPort);
    
    GetPortBounds(mPort, &clip);
    SetOrigin(0,0);
    
    CGRect r;
    
    r.origin.x    = x;
    r.origin.y    = 0;
    r.size.width  = w;
    r.size.height = h;

    CGrafPtr          grafPort = device;
    CGContextRef      context;
    CGDataProviderRef provider;
    CGColorSpaceRef   colorspace;
    CGImageRef        image;
    
    QDBeginCGContext(grafPort, &context);
    CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
    
    Rect rect;
    GetPortBounds(grafPort, &rect);
    
    // Translate to top-down coordinates
    
    CGContextSaveGState (context);
    CGContextTranslateCTM(context, 0, (float)(rect.bottom - rect.top) - (h+y));
    
    // Draw
    
    colorspace = CGColorSpaceCreateDeviceRGB();
    provider   = CGDataProviderCreateWithData(NULL, pixels, w*h*4, NULL);
    
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
    QDEndCGContext(grafPort, &context);                
    
    SetOrigin(clip.left, clip.top);
    SetPort(oldPort);
    
#endif
}

