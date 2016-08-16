//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_DB_CONTROL_H
#define PK_DB_CONTROL_H

#include "PKCustomControl.h"

class PKImage;

//
// PKDoubleBufferedControl
//

class PKDoubleBufferedControl : public PKCustomControl
{
public:

    static std::string ALPHA_DOUBLE_PROPERTY; // [0-1]

public:
	PKDoubleBufferedControl();

protected:
	virtual ~PKDoubleBufferedControl();

public:
    virtual void build();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

    // Subclasses implement this
    virtual void draw32bpp(uint8_t *buffer);

	void resize(uint32_t w, 
				uint32_t h);

    void premultiplyAlpha(PKImage *bitmap);

protected:

#ifdef WIN32
    void drawWin32(PKDrawingDevice device, 
				   PKRect          rect);
#endif
	
#ifdef LINUX
    void drawLinux(GtkWidget   *widget,
                   GdkDrawable *drawable,
                   GdkGC       *gc,
                   PKRect       rect);    
#endif

#ifdef MACOSX
    void drawMacOSX(RgnHandle	 inLimitRgn,
					CGContextRef inContext);
#endif

private:

#ifdef WIN32

    HDC     dcBrush;
    HBITMAP bmpBrush;
    HBITMAP bmp;

#endif

#ifdef LINUX

    GdkPixbuf *pix;

#endif

    uint8_t *buffer;
};

#endif // PK_DB_CONTROL_H
