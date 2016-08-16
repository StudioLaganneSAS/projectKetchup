//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCustomControl.h"
#include "PKCustomContainer.h"
#include "PKImage.h"

//
// Defines
//

#ifndef PK_ICON_TAB_CONTROL_H
#define PK_ICON_TAB_CONTROL_H

//
// PKIconTabControl
//

#ifdef LINUX
class PKIconTabControl : public PKCustomContainer
#else
class PKIconTabControl : public PKCustomControl
#endif
{
public:

    //
    // Properties
    // 

    static std::string ICONS_WSTRINGLIST_PROPERTY;
    static std::string CURRENT_TAB_UINT32_PROPERTY;
    static std::string TOOLTIPS_WSTRINGLIST_PROPERTY;

public:
    PKIconTabControl();

protected:
    ~PKIconTabControl();

public:
    void build();
    void initialize(PKDialog *dialog);
    void destroy();

    PKWindowHandle getWindowHandle();

    void switchToTab(uint32_t tab);

#ifdef WIN32
    void drawWin32(PKDrawingDevice device, 
				   PKRect          rect);
#endif
	
#ifdef MACOSX
    virtual void drawMacOSX(RgnHandle	 inLimitRgn,
							CGContextRef inContext);	
#endif

#ifdef LINUX
    virtual void drawLinux(GtkWidget   *widget,
                           GdkDrawable *drawable,
                           GdkGC       *gc,
                           PKRect       rect);     
#endif

    // From PKCustomControl

	virtual void mouseDown(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod);

	virtual void mouseMove(int x,
						   int y,
						   unsigned int mod);

    // From PKControl

    virtual void move(int32_t x, 
					  int32_t y);

	virtual void resize(uint32_t w, 
						uint32_t h);

    void relayout();

    void show();
	void hide();

	void enable();
	void disable();

    virtual uint32_t getMinimumWidth();
	virtual uint32_t getMinimumHeight();

    // From PKObject

	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

#ifdef WIN32
    std::vector<HICON> hicons;
#endif

#ifdef MACOSX
    std::vector<PKImage *> bitmaps;
#endif

#ifdef LINUX
    std::vector<GdkPixbuf *> bitmaps;
    GtkWidget *tab;
    GdkCursor *hand;
#endif

    bool built;

public:
    bool hasFocusChild;
    bool focusSet;
};

#endif // PK_ICON_TAB_CONTROL_H
