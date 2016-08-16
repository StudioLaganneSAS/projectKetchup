//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_LINE_CONTROL_H
#define PK_LINE_CONTROL_H

#include "PKCustomControl.h"

//
// PKPanelControl
//

class PKLineControl : public PKCustomControl
{
public:

    // PROPERTIES

    static std::string COLOR_COLOR_PROPERTY;
    static std::string HORIZONTAL_BOOL_PROPERTY;

public:
	PKLineControl();

protected:
	virtual ~PKLineControl();

public:
    
    virtual void build();

#ifdef WIN32
    virtual void drawWin32(PKDrawingDevice device, 
						   PKRect          rect);
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

    virtual uint32_t getMinimumWidth();
	virtual uint32_t getMinimumHeight();

private:

};

#endif // PK_LINE_CONTROL_H
