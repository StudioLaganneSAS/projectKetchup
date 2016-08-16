//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SCROLL_VIEW_H
#define PK_SCROLL_VIEW_H

#include "PKCustomControl.h"

//
// PKScrollView
//

#ifdef MACOSX
#include "PKPanelControl.h"
#include "PKHLayout.h"
#endif

class PKScrollView : public PKControl
{
public:

    // PROPERTIES

    static std::string BACKGROUND_COLOR_PROPERTY;
    static std::string VSCROLL_BOOL_PROPERTY;
    static std::string HSCROLL_BOOL_PROPERTY;
    static std::string MACMARGIN_INT32_PROPERTY;

public:
	PKScrollView();

protected:
	virtual ~PKScrollView();

public:
    
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

    PKWindowHandle getWindowHandle();

#ifdef WIN32
    HBRUSH getBackgroundBrush();
    bool   drawsItsBackground();
#endif
	
	void updateScrollbars();
	void scrollToTop();

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

    virtual int32_t getAbsoluteX();
    virtual int32_t getAbsoluteY();

    virtual uint32_t getViewportWidth();
    virtual uint32_t getViewportHeight();

private:

#ifdef WIN32
    HBRUSH bkg;
#endif
	
#ifdef MACOSX
	PKPanelControl *panel;
	PKHLayout      *layout;
#endif

#ifdef WIN32
public:

	static LRESULT CALLBACK CustomWinProc(HWND hwnd,
							 		      UINT uMsg,
								 	      WPARAM wParam,
									      LPARAM lParam);
#endif
};

#endif // PK_SCROLL_VIEW_H
