//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CUSTOM_CONTROL_H
#define PK_CUSTOM_CONTROL_H

#include "PKControl.h"

#ifdef MACOSX
#include "PKMacCustomView.h"
#endif

//
// PKCustomControl
//

class PKCustomControl : public PKControl
{
public:

    //
    // PROPERTIES
    //

	static std::string MINIMUMWIDTH_INT32_PROPERTY;
	static std::string MINIMUMHEIGHT_INT32_PROPERTY;
	static std::string WANTFOCUS_BOOL_PROPERTY;

public:
	PKCustomControl();

protected:
	virtual ~PKCustomControl();

public:
    virtual void build();

#ifdef WIN32
    virtual void drawWin32(PKDrawingDevice device, 
						   PKRect          rect);
#endif

#ifdef LINUX
    
    virtual void setDesiredWidgetSize(uint32_t w, uint32_t h);

    virtual void drawLinux(GtkWidget   *widget,
                           GdkDrawable *drawable,
                           GdkGC       *gc,
                           PKRect       rect);    
#endif
	
#ifdef MACOSX
    virtual void drawMacOSX(RgnHandle	 inLimitRgn,
							CGContextRef inContext);
	
	virtual int getScrollableWidth();
	virtual int getScrollableHeight();
	
	virtual int getScrollLineWidth();
	virtual int getScrollLineHeight();
	
	virtual void setScrollOrigin(int x, int y);	

	virtual int getScrollOriginX();
	virtual int getScrollOriginY();	
#endif

	virtual uint32_t getMinimumWidth();
	virtual uint32_t getMinimumHeight();

    //
    // Control Events, override if you need them
    //
    
	//
	// Mouse events
	//
	// Sent by the window when the mouse
	// hovers, or is down/up as well as for
	// double clicks and mouse wheel
	//

	virtual void mouseDown(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod);

	virtual void mouseUp(PKButtonType button,
						 int x,
						 int y,
						 unsigned int mod);

	virtual void mouseDblClick(PKButtonType button,
							   int x,
							   int y,
							   unsigned int mod);

	virtual void mouseMove(int x,
						   int y,
						   unsigned int mod);

	virtual void mouseLeave();

	virtual void mouseWheel(PKButtonType button,
							int x,
							int y,
							unsigned int mod);

	//
	// keyDown() / keyUp()
	//
	// Sent when the window has
	// the focus and a key is 
	// pressed/released.
	//

    // Return true if handled

	virtual bool keyDown(unsigned int modifier,
						 PKKey        keyCode,
						 unsigned int virtualCode);

	virtual bool keyUp(unsigned int modifier,
					   PKKey        keyCode,
					   unsigned int virtualCode);

	virtual void characterInput(std::wstring wstr);

	//
	// Focus handling
	//
	
	virtual bool moveFocusToNextControlPart();
	virtual bool moveFocusToPreviousControlPart();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

#ifdef WIN32
public:

	static LRESULT CALLBACK CustomWinProc(HWND hwnd,
							 		      UINT uMsg,
								 	      WPARAM wParam,
									      LPARAM lParam);

	void getMouseParams(WPARAM wParam, 
					    LPARAM lParam,
					    int *x,
					    int *y,
					    unsigned int *mod);

#endif

#ifdef LINUX

    GtkWidget *draw_area;

    static gboolean gtk_expose_event_callback(GtkWidget      *widget, 
                                              GdkEventExpose *event, 
                                              gpointer        data);
                                              
    static gboolean gtk_leave_notify_event_callback(GtkWidget        *widget,
                                                     GdkEventCrossing *event,
                                                     gpointer          user_data);

    static gboolean gtk_motion_notify_event_callback(GtkWidget      *widget,
                                                     GdkEventMotion *event,
                                                     gpointer        user_data);                                                                                                   

    static gboolean gtk_button_press_event_callback(GtkWidget      *widget,
                                                    GdkEventButton *event,
                                                    gpointer        user_data);
                                                        
    static gboolean gtk_button_release_event_callback(GtkWidget      *widget,
                                                      GdkEventButton *event,
                                                      gpointer        user_data);

    static gboolean gtk_key_press_event_callback(GtkWidget   *widget,
                                                 GdkEventKey *event,
                                                 gpointer     user_data);  

    static gboolean gtk_key_release_event_callback(GtkWidget   *widget,
                                                   GdkEventKey *event,
                                                   gpointer     user_data);  

    static gboolean gtk_focus_event_callback(GtkWidget       *widget,
                                             GtkDirectionType direction,
                                             gpointer         user_data);

    static gboolean gtk_keynav_failed_event_callback(GtkWidget       *widget,
                                                     GtkDirectionType direction,
                                                     gpointer         user_data);
                                                      
#endif
	
#ifdef MACOSX

public:

    virtual void drawMacOSXInternal(RgnHandle	 inLimitRgn,
							        CGContextRef inContext);
	
	int sx;
	int sy;
	
#endif
};

#endif // PK_CUSTOM_CONTROL_H
