//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_GRADIENT_PANEL_CONTROL_H
#define PK_GRADIENT_PANEL_CONTROL_H

#include "PKCustomControl.h"
#include "PKCustomContainer.h"

//
// PKGradientPanel
//

#ifdef LINUX
class PKGradientPanel : public PKCustomContainer
#else
class PKGradientPanel : public PKCustomControl
#endif
{
public:

    // PROPERTIES

    static std::string BORDER_COLOR_PROPERTY;
    static std::string BACKGROUND_COLOR1_PROPERTY;
    static std::string BACKGROUND_COLOR2_PROPERTY;
    static std::string BORDER_SIZE_UINT32_PROPERTY;
    static std::string HORIZONTAL_BOOL_PROPERTY;

public:
	PKGradientPanel();

protected:
	virtual ~PKGradientPanel();

public:
    
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

    PKWindowHandle getWindowHandle();

#ifdef WIN32
    virtual void drawWin32(PKDrawingDevice device, 
						   PKRect          rect);

    HBRUSH getBackgroundBrush();
    bool   drawsItsBackground();

#endif
	
#ifdef LINUX
    virtual void drawLinux(GtkWidget   *widget,
                           GdkDrawable *drawable,
                           GdkGC       *gc,
                           PKRect       rect);    
#endif

#ifdef MACOSX
    virtual void drawMacOSX(RgnHandle	 inLimitRgn,
							CGContextRef inContext);
#endif

	void move(int32_t x, 
			  int32_t y);

	void resize(uint32_t x, 
				uint32_t y);

	void show();
	void hide();

	void enable();
	void disable();

    virtual uint32_t getMinimumWidth();
	virtual uint32_t getMinimumHeight();

private:

#ifdef WIN32
    HBRUSH bkg;
#endif

};

#endif // PK_GRADIENT_PANEL_CONTROL_H
