//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKVectorControl.h"
#include "PKCustomContainer.h"
#include "PKImage.h"

//
// Defines
//

#ifndef PK_ARROW_TAB_CONTROL_H
#define PK_ARROW_TAB_CONTROL_H

//
// PKIconTabControl
//

#ifdef LINUX
class PKArrowTabControl : public PKCustomContainer
#else
class PKArrowTabControl : public PKVectorControl
#endif
{
public:

    //
    // Properties
    // 

    static std::string BKG_IMAGE_WSTRING_PROPERTY;
    static std::string ARROW_IMAGE_WSTRING_PROPERTY;
    static std::string ICONS_WSTRINGLIST_PROPERTY;
    static std::string CURRENT_TAB_UINT32_PROPERTY;
    static std::string TOOLTIPS_WSTRINGLIST_PROPERTY;

public:
    PKArrowTabControl();

protected:
    ~PKArrowTabControl();

public:
    void build();
    void initialize(PKDialog *dialog);
    void destroy();

    PKWindowHandle getWindowHandle();

    void switchToTab(uint32_t tab);

	virtual void drawVector(PKVectorEngine *engine,
		                    int32_t x, int32_t y);

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

    std::vector<PKImage *> icons;

#ifdef LINUX
    std::vector<GdkPixbuf *> bitmaps;
    GtkWidget *tab;
    GdkCursor *hand;
#endif

	PKImage *bkg;
	PKImage *arrow;
	
	int bar_height;
	
    bool built;

public:
    bool hasFocusChild;
    bool focusSet;
};

#endif // PK_ARROW_TAB_CONTROL_H
