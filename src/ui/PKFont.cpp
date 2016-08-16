//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//


#include "PKFont.h"
#include "PKBitmapOps.h"
#include "PKStr.h"

#ifdef WIN32
#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY 5
#endif
#endif

//
// PKFont
//

PKFont::PKFont()
{
	this->family = "[Default]";
	this->size = 0;
	this->bold = false;
	this->italic = false;
	this->underline = false;

#ifdef WIN32

	NONCLIENTMETRICS nct;
	nct.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nct, 0);
    this->font =  CreateFontIndirect(&nct.lfMenuFont);

    TEXTMETRIC metrics;
    HDC dc = GetDC(NULL);
    HFONT old = (HFONT) SelectObject(dc, this->font);
    GetTextMetrics(dc, &metrics);
    ReleaseDC(NULL, dc);

    this->height = metrics.tmHeight;

#endif

#ifdef MACOSX
    this->height = this->size;
    // TODO
#endif

#ifdef LINUX

    // TODO: Get default from GTK/pango ?

	this->family = "arial";
	this->size   = 14;
    this->height = this->size;
#endif
}

PKFont::PKFont(std::string  family,
			   unsigned int size,
			   bool			bold,
			   bool			italic,
			   bool			underline)
{

	this->family = family;

	this->size = size+3;
	this->bold = bold;
	this->italic = italic;
	this->underline = underline;
    this->height = this->size;

#ifdef WIN32

	std::wstring fname = PKStr::stringToWString(family);

	HDC hdc = GetDC(NULL);

	LONG nHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	DWORD quality = DEFAULT_QUALITY;
    quality = CLEARTYPE_QUALITY;

	this->font = CreateFont(nHeight, 0, 0, 0, (bold ? FW_BOLD : FW_NORMAL),
						    italic ? TRUE : FALSE, underline ? TRUE : FALSE, FALSE, DEFAULT_CHARSET,
							OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, quality, DEFAULT_PITCH | FF_DONTCARE,
							fname.c_str());

	ReleaseDC(NULL, hdc);

	this->height = abs(nHeight);

#endif
    
#ifdef MACOSX
    
    this->height = size+3;

    OSStatus status = noErr;
    
    // Locate font
    
    ATSUFontID fontID;
    
    status = ATSUFindFontFromName(family.c_str(), family.size(),
                                  kFontFullName, kFontNoPlatformCode, 
                                  kFontUnicodePlatform, kFontNoLanguageCode,
                                  &fontID);
								  
	if(status == kATSUInvalidFontErr)
	{
		status = ATSUFindFontFromName("Arial", 5,
									  kFontFullName, kFontNoPlatformCode, 
									  kFontUnicodePlatform, kFontNoLanguageCode,
									  &fontID);
	}
    
    // Create style
    
    status = ATSUCreateStyle (&defaultStyle);
    
    if(status == noErr)
    {
        ATSUAttributeTag  theTags[] = {kATSUSizeTag, kATSURGBAlphaColorTag, kATSUFontTag, kATSUStyleRenderingOptionsTag};
        ByteCount        theSizes[] = {sizeof(Fixed), sizeof(ATSURGBAlphaColor), sizeof(ATSUFontID), sizeof(ATSStyleRenderingOptions)};
        
        Fixed             atsuSize = IntToFixed(this->height);
        ATSURGBAlphaColor color    = {1, 1, 1, 1};
//        ATSStyleRenderingOptions o = kATSStyleNoAntiAliasing;
		// Use AA on the mac so it looks better !
		ATSStyleRenderingOptions o = kATSStyleApplyAntiAliasing;
            
        ATSUAttributeValuePtr theValues[] = {&atsuSize, &color, &fontID, &o};
        
        status = ATSUSetAttributes(defaultStyle, 4, 
                                   theTags, 
                                   theSizes, 
                                   theValues);                
        
        if(bold)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDBoldfaceTag};
            ByteCount        sizes[] = {sizeof(Boolean)};

            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
    }
    
#endif

	this->r = 0;
	this->g = 0;
	this->b = 0;
}

int32_t PKFont::getSystemFontSize()
{
#ifdef WIN32

    // TODO: Fix, this is crap

    /*

	NONCLIENTMETRICS nct;
	nct.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nct, 0);
    HFONT font = CreateFontIndirect(&nct.lfMenuFont);

    TEXTMETRIC metrics;
    HDC dc = GetDC(NULL);
    HFONT old = (HFONT) SelectObject(dc, font);
    GetTextMetrics(dc, &metrics);
    ReleaseDC(NULL, dc);

    */

    return 9; //metrics.tmHeight;
#endif

#ifdef LINUX
    // TODO: better
    return 10;
#endif

#ifdef MACOSX
    // TODO: better
    return 14;
#endif
}

PKFont::PKFont(PKFont *copy)
{
	if(copy == NULL)
	{
		return;
	}

	this->family = copy->family;

	this->size = copy->size;
	this->bold = copy->bold;
	this->italic = copy->italic;
	this->underline = copy->underline;

#ifdef WIN32

	std::wstring fname = PKStr::stringToWString(family);

	HDC hdc = GetDC(NULL);

	LONG nHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	this->font = CreateFont(nHeight, 0, 0, 0, (bold ? FW_BOLD : FW_NORMAL),
						    italic ? TRUE : FALSE, underline ? TRUE : FALSE, FALSE, DEFAULT_CHARSET,
							OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
							fname.c_str());

	ReleaseDC(NULL, hdc);

	this->height = abs(nHeight);

#endif
    
#ifdef MACOSX
    
    this->height = size+3;

    OSStatus status = noErr;
    
    // Locate font
    
    ATSUFontID fontID;
    
    status = ATSUFindFontFromName(family.c_str(), family.size(),
                                  kFontFullName, kFontNoPlatformCode, 
                                  kFontUnicodePlatform, kFontNoLanguageCode,
                                  &fontID);
								  
	if(status == kATSUInvalidFontErr)
	{
		status = ATSUFindFontFromName("Arial", 5,
									  kFontFullName, kFontNoPlatformCode, 
									  kFontUnicodePlatform, kFontNoLanguageCode,
									  &fontID);
	}
    
    // Create style
    
    status = ATSUCreateStyle (&defaultStyle);
    
    if(status == noErr)
    {
        ATSUAttributeTag  theTags[] = {kATSUSizeTag, kATSURGBAlphaColorTag, kATSUFontTag, kATSUStyleRenderingOptionsTag};
        ByteCount        theSizes[] = {sizeof(Fixed), sizeof(ATSURGBAlphaColor), sizeof(ATSUFontID), sizeof(ATSStyleRenderingOptions)};
        
        Fixed             atsuSize = IntToFixed(this->height);
        ATSURGBAlphaColor color    = {1, 1, 1, 1};
        ATSStyleRenderingOptions o = kATSStyleNoAntiAliasing;
            
        ATSUAttributeValuePtr theValues[] = {&atsuSize, &color, &fontID, &o};
        
        status = ATSUSetAttributes(defaultStyle, 4, 
                                   theTags, 
                                   theSizes, 
                                   theValues);                
        
        if(bold)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDBoldfaceTag};
            ByteCount        sizes[] = {sizeof(Boolean)};

            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
    }
    
#endif

	this->r = copy->r;
	this->g = copy->g;
	this->b = copy->b;
}

PKFont::~PKFont()
{
#ifdef WIN32

	if(this->font != NULL)
	{
		DeleteObject(this->font);
		this->font = NULL;
	}
#endif
    
#ifdef MACOSX
    
    ATSUDisposeStyle(defaultStyle);
    
#endif
    
    this->mutex.wait();

	for(unsigned int i=0; i < this->cache_bitmaps.size(); i++)
	{
		if(this->cache_bitmaps[i] != NULL)
		{
			delete this->cache_bitmaps[i];
		}
	}

	this->cache_map.clear();
	this->cache_bitmaps.clear();

	this->wcache_map.clear();
	this->wcache_widths.clear();

    this->mutex.release();
}

bool PKFont::isValid()
{
#ifdef WIN32
	return (this->font != NULL);
#endif
    
    return true;
}

unsigned int PKFont::getCharHeight()
{
	return this->height;
}

unsigned int PKFont::getCharWidth(unsigned short c)
{
    PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->font != NULL)
	{
		SIZE size;
		memset(&size, 0, sizeof(SIZE));

		HDC dc = GetDC(NULL);
		HFONT old = (HFONT) SelectObject(dc, this->font);

		GetTextExtentPoint32(dc, (LPCWSTR) &c, 1, &size);
		
		SelectObject(dc, old);
		ReleaseDC(NULL, dc);
		return size.cx;
	}

#endif

#ifdef LINUX

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
    cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy(surface);
    
    cairo_text_extents_t extents;
    
    std::wstring wtext = std::wstring(1, c);
    std::string textUTF8 = PKStr::wStringToUTF8string(wtext);
    
    cairo_select_font_face(cr, this->family.c_str(), 
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
                           
    cairo_set_font_size(cr, this->size);
 
    cairo_text_extents(cr, textUTF8.c_str(), &extents);
        
    cairo_destroy(cr);

    return extents.width;

#endif

#ifdef MACOSX
    
	int index = this->getWCacheIndex(c);

    if(index != -1)
    {
        return this->wcache_widths[index];
    }
    else
    {
        OSStatus        status;
        ATSUTextLayout  myTextLayout;
        UniCharCount    length = 1;
        
        status = ATSUCreateTextLayoutWithTextPtr ((UniChar*) &c,
                                                  kATSUFromTextBeginning, // offset from beginning
                                                  kATSUToTextEnd,         // length of text range
                                                  1,                      // length of text buffer
                                                  1,                      // number of style runs
                                                  &length,                // length of the style run
                                                  &defaultStyle, 
                                                  &myTextLayout); 
        
        ItemCount count = 0;
        ATSTrapezoid value;
        
        ATSUGetGlyphBounds(myTextLayout, 0, 0,
                           kATSUFromTextBeginning,
                           1, kATSUseDeviceOrigins,
                           1, &value, &count);
        
        int x1 = FixedToInt(value.lowerLeft.x);
        int x2 = FixedToInt(value.lowerRight.x);
        
        ATSUDisposeTextLayout(myTextLayout);
        
		this->wcache_map.push_back(c);
        this->wcache_widths.push_back(x2 - x1);
        
        return (x2 - x1);
    }
        
#endif
    
	return 0;
}

unsigned int PKFont::getTextWidth(std::wstring text)
{
    PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->font != NULL)
	{
		SIZE l = {0, 0};
		HDC dc = GetDC(NULL);
		HFONT old = (HFONT) SelectObject(dc, this->font);

		GetTextExtentPoint32(dc, text.c_str(), text.size(), &l);
		
		SelectObject(dc, old);
		ReleaseDC(NULL, dc);
		
		return l.cx;
	}

#endif
    
#ifdef LINUX

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
    cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy(surface);
    
    cairo_text_extents_t extents;
    
    std::string textUTF8 = PKStr::wStringToUTF8string(text);
    
    cairo_select_font_face(cr, this->family.c_str(), 
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
                           
    cairo_set_font_size(cr, this->size);
 
    cairo_text_extents(cr, textUTF8.c_str(), &extents);
        
    cairo_destroy(cr);

    return extents.width + 2;

#endif
    
    
#ifdef MACOSX
    
    int total = 0;
    
    for(unsigned int i=0; i < text.length(); i++)
    {        
        total += this->getCharWidth(text[i]);
    }
        
    return total;
    
#endif

	return 0;
}

void PKFont::setColor(unsigned char r, 
					  unsigned char g,
					  unsigned char b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

void PKFont::drawChar(unsigned short  c,
			          void           *dstBuffer, 
			          int    x,
			          int    y,
                      unsigned int bufferW,
                      unsigned int bufferH,
                      unsigned char alpha)
{
    PKCritLock lock(&this->mutex);

    PKImage *src  = NULL;
	unsigned int l = 0;

	if(dstBuffer == NULL)
	{
		return;
	}

	int index = this->getCacheIndex(c);

	if(index != -1)
	{
		src = this->cache_bitmaps[index];
	}
	else
	{
#ifdef WIN32

		// We have to create the cache
		// bitmap for this character...

		HDC  dc = GetDC(NULL);
		HDC fdc = CreateCompatibleDC(dc);

		l = this->getCharWidth(c);
		
		HBITMAP    bmp = CreateCompatibleBitmap(dc, l, 2*this->height);

		// Prep the bitmap and DC

		SelectObject(fdc, (HFONT) this->font);
		SelectObject(fdc, bmp);
		SetTextColor(fdc, RGB(255, 255, 255));
		SetBkMode(fdc, TRANSPARENT);
		
		RECT r;
		r.top    = 0;
		r.left   = 0;
		r.right  = l;
		r.bottom = 2*this->height;

		HBRUSH b = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(fdc, &r, b);

		DeleteObject(b);

		// Draw the text to the DDB

		TextOut(fdc, 0, 0, (LPCWSTR) &c, 1);
		
		// Now move it to a DIB

		PKImage *cache_bmp = new PKImage();

		if(cache_bmp == NULL || 
		  !cache_bmp->create(l, 2*this->height))
		{
			DeleteObject(bmp);
			DeleteDC(fdc);
			ReleaseDC(NULL, dc);

			return;
		}

		char *bits = cache_bmp->getPixels();

		BITMAPINFO dibInfo;
		memset(&dibInfo, 0, sizeof(BITMAPINFO));

		dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
		dibInfo.bmiHeader.biWidth         =  l;
		dibInfo.bmiHeader.biHeight        =  -2*this->height;
		dibInfo.bmiHeader.biBitCount      =  32;
		dibInfo.bmiHeader.biClrUsed       =  0;
		dibInfo.bmiHeader.biClrImportant  =  0;
		dibInfo.bmiHeader.biCompression   =  BI_RGB;
		dibInfo.bmiHeader.biPlanes        =  1;
		dibInfo.bmiHeader.biSizeImage     =  l*2*this->height*4;
		dibInfo.bmiHeader.biXPelsPerMeter =  0;
		dibInfo.bmiHeader.biYPelsPerMeter =  0;

		GetDIBits(fdc, bmp, 0, 2*this->height, bits, &dibInfo, 0);

		DeleteObject(bmp);
		DeleteDC(fdc);
		ReleaseDC(NULL, dc);

		// Now convert the grey values to
		// alpha values andd colorize...

		unsigned char *s = (unsigned char *) bits;

		for(unsigned int j=0; j < 2*this->height; j++)
		{
			for(unsigned int i=0; i < l; i++)
			{
				int grey  = (255 - *s);
				int alpha = 255 - grey;
 
				*(s + 0) = this->b;
				*(s + 1) = this->g;
				*(s + 2) = this->r;
				*(s + 3) = alpha;

				s += 4;
			}
		}

		src = cache_bmp;

		this->cache_map.push_back(c);
		this->cache_bitmaps.push_back(src);

#endif

#ifdef LINUX

    uint32_t width  = this->getCharWidth(c);
    uint32_t stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
    
    unsigned char *data = new unsigned char[stride*this->size];

    if(data == NULL)
    {
        return;
    }        

    memset(data, 0, stride*this->size);

    cairo_surface_t *surface = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_ARGB32, width, this->size, stride);
    cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy(surface);
        
    // Now draw text
    
    std::wstring wtext = std::wstring(1, c);
    std::string textUTF8 = PKStr::wStringToUTF8string(wtext);
    
    cairo_select_font_face(cr, this->family.c_str(), 
                           (this->italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL),
                           (this->bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL));
                           
	cairo_set_source_rgba(cr, 
	                      (double) this->r/255.0, 
	                      (double) this->g/255.0, 
	                      (double) this->b/255.0,
	                      (double) 1.0);

    cairo_set_font_size(cr, this->size);

    cairo_move_to(cr, 0, this->size);
    cairo_show_text(cr, textUTF8.c_str());
    
    // We have the text in the data buffer, now copy to our buffer
        
	PKBitmapOps::alphaBlitFunc((unsigned char *) data, 
							   (unsigned char *) dstBuffer + y*bufferW*4 + x*4, 
							   width, this->size, stride/4, bufferW, alpha);

    // All done
        
    cairo_destroy(cr);
    delete [] data;

#endif
        
#ifdef MACOSX
      
        // Create cache bitmap
        
        l = this->getCharWidth(c);
        int h = 2*this->height;
        
		PKImage *cache_bmp = new PKImage();
        
		if(cache_bmp == NULL || 
           !cache_bmp->create(l, h))
		{
			return;
		}
        
		char *bits = cache_bmp->getPixels();
        
        // black out the pixels
        memset(bits, 0, l*h*4);
        
        CGContextRef    context;
        CGColorSpaceRef colorspace;

        colorspace = CGColorSpaceCreateDeviceRGB();
        
        context = CGBitmapContextCreate(bits, l, h, 8, 4*l, 
                                        colorspace, 
                                        kCGImageAlphaNoneSkipFirst);
        
        if(context != NULL)
        {
            OSStatus        status;
            ATSUTextLayout  myTextLayout;
            UniCharCount    length = 1;
            
            status = ATSUCreateTextLayoutWithTextPtr ((UniChar*) &c,
                                                      kATSUFromTextBeginning, // offset from beginning
                                                      kATSUToTextEnd,         // length of text range
                                                      1,                      // length of text buffer
                                                      1,                      // number of style runs
                                                      &length,                // length of the style run
                                                      &defaultStyle, 
                                                      &myTextLayout); 
            
            ATSUAttributeTag  ltheTags[] =  {kATSUCGContextTag};
            ByteCount   ltheSizes[] = {sizeof(CGContextRef)};
            
            ATSUAttributeValuePtr ltheValues[] = {&context};
            
            ATSUSetLayoutControls(myTextLayout, 1, 
                                  ltheTags, 
                                  ltheSizes, 
                                  ltheValues);
            
            ATSUDrawText (myTextLayout, 
                          kATSUFromTextBeginning, 
                          kATSUToTextEnd, 
                          IntToFixed(0), IntToFixed(this->height));
            
            ATSUDisposeTextLayout(myTextLayout);
            
            CGContextRelease(context);
        }
        
        CGColorSpaceRelease(colorspace);
        
		// Now convert the grey values to
		// alpha values and colorize...
        
		unsigned char *s = (unsigned char *) bits;
        
		for(int j=0; j < 2*this->height; j++)
		{
			for(int i=0; i < l; i++)
			{
				int grey  = *s;
                                
				*(s + 0) = grey; // use initial level as alpha value
                                 // so that the black bkg is not drawn
                
				*(s + 1) = grey*this->r/255;
				*(s + 2) = grey*this->g/255;
				*(s + 3) = grey*this->b/255;
                
				s += 4;
			}
		}
        
		src = cache_bmp;
        
		this->cache_map.push_back(c);
        this->cache_bitmaps.push_back(src);
        
#endif
	}	

	if(src != NULL)
	{
        PKBitmapOps::alphaBlit(dstBuffer, alpha, x, y, bufferW, bufferH, src); 
	}
}

void PKFont::drawChar(unsigned short  c,
					  PKImage        *dst, 
					  int    x,
					  int    y,
                      unsigned char alpha)
{
	if(dst == NULL)
	{
		return;
	}

    void *buffer = (void *) dst->getPixels();

    unsigned int w = dst->getWidth();
    unsigned int h = dst->getHeight();

    return this->drawChar(c, buffer, x, y, w, h, alpha);
}

int PKFont::drawText(std::wstring    text,
					 PKImage        *dst, 
					 int    x,
					 int    y,
                     unsigned char alpha)
{
    PKCritLock lock(&this->mutex);

    if(dst == NULL)
    {
        return 0;
    }        

    return this->drawText(text, 
                          dst->getPixels(), 
                          x, y, 
                          dst->getWidth(), 
                          dst->getHeight(), 
                          alpha);
}

int PKFont::drawText(std::wstring    text,
			         void           *dstBuffer, 
			         int    x,
			         int    y,
                     unsigned int bufferW,
                     unsigned int bufferH,
                     unsigned char alpha)
{
    PKCritLock lock(&this->mutex);

#ifdef LINUX

    uint32_t width  = this->getTextWidth(text);
    uint32_t stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
    
    unsigned char *data = new unsigned char[stride*this->size];

    if(data == NULL)
    {
        return 0;
    }        

    memset(data, 0, stride*this->size);

    cairo_surface_t *surface = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_ARGB32, width, this->size, stride);
    cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy(surface);
        
    // Now draw text
    
    std::string textUTF8 = PKStr::wStringToUTF8string(text);
    
    cairo_select_font_face(cr, this->family.c_str(), 
                           (this->italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL),
                           (this->bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL));
                           
	cairo_set_source_rgba(cr, 
	                      (double) this->r/255.0, 
	                      (double) this->g/255.0, 
	                      (double) this->b/255.0,
	                      (double) 1.0);

    cairo_set_font_size(cr, this->size);

    cairo_move_to(cr, 0, this->size);
    cairo_show_text(cr, textUTF8.c_str());
    
    // We have the text in the data buffer, now copy to our buffer
        
	PKBitmapOps::alphaBlitFunc((unsigned char *) data, 
							   (unsigned char *) dstBuffer + y*bufferW*4 + x*4, 
							   width, this->size, stride/4, bufferW, alpha);

    // All done
        
    cairo_destroy(cr);
    delete [] data;

    return width;

#endif


    int off = x;

	for(unsigned int i=0; i < text.size(); i++)
	{
		this->drawChar(text[i], dstBuffer, off, y, bufferW, bufferH, alpha);
		off += this->getCharWidth(text[i]);
	}

	return (off - x);
}

int PKFont::getCacheIndex(unsigned short c)
{
    PKCritLock lock(&this->mutex);

    for(unsigned int i=0; i < this->cache_map.size(); i++)
	{
		if(this->cache_map[i] == c)
		{
			return i;
		}
	}

	return -1;
}

int PKFont::getWCacheIndex(unsigned short c)
{
    PKCritLock lock(&this->mutex);

    for(unsigned int i=0; i < this->wcache_map.size(); i++)
	{
		if(this->wcache_map[i] == c)
		{
			return i;
		}
	}

	return -1;
}
