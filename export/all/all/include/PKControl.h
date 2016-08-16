//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKCONTROL_H
#define PKCONTROL_H

#include "PKView.h"
#include "PKSafeEvents.h"
#include "PKCritSec.h"

#ifdef WIN32
#include <windows.h>
#include <commctrl.h>
#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

#ifdef LINUX
#include <gtk/gtk.h>
#endif

class PKDialog;

typedef enum {

	PK_LAYOUT_MIN,
	PK_LAYOUT_FIXED,
	PK_LAYOUT_MAX,

} PKLayoutPolicy;

//
// PKControl
//

class PKControl : public PKView, public IPKSafeEventHandler
{
public:

	static std::string ID_STRING_PROPERTY;
	static std::string FRAME_RECT_PROPERTY;
	static std::string HIDDEN_BOOL_PROPERTY;
	static std::string ENABLED_BOOL_PROPERTY;
	static std::string LAYOUTX_INT32_PROPERTY;
	static std::string LAYOUTY_INT32_PROPERTY;
    static std::string TOOLTIP_WSTRING_PROPERTY;

public:
	PKControl();

protected:
	virtual ~PKControl();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

    virtual PKDialog *getDialog();

	virtual PKWindowHandle getWindowHandle();

#ifdef WIN32
    virtual HBRUSH getBackgroundBrush();
    virtual bool   drawsItsBackground();
    virtual bool   parentDrawsItsBackground();

    virtual void fillDeviceWithBackground(PKDrawingDevice device,
                                          int x, int y, int w, int h);

#endif
	
#ifdef MACOSX
	virtual ControlRef getControlRef();
#endif

    void setLayoutPolicy(PKLayoutPolicy layoutx,
						 PKLayoutPolicy layouty);

	void setControlId(std::string id);
	std::string getControlId();

	PKLayoutPolicy getLayoutPolicyX();
	PKLayoutPolicy getLayoutPolicyY();

    bool isShowing();

	virtual void moveBy(int32_t x, 
					    int32_t y);

    virtual void move(int32_t x, 
					  int32_t y);

	virtual void resize(uint32_t w, 
						uint32_t h);

    virtual void relayout();

	virtual void setHidden(bool hidden);
	virtual bool getHidden();

	virtual void setHiddenRecursive(bool hidden);

    virtual void show();
	virtual void hide();

	virtual void enable();
	virtual void disable();

    virtual void invalidate(bool recurse = true);
	virtual void invalidateRect(PKRect client);
    virtual void invalidateParentControl();

    virtual void updateUIStrings();

    virtual void doCut();
    virtual void doCopy();
    virtual void doPaste();
    virtual void doSelectAll();

    virtual bool hasSelection();

    // Override this for focus notifications
    virtual void focusAcquired(PKDirection direction);
    virtual void focusLost();

    virtual bool hasFocus();

	// Override this if you have multiple focus parts
	virtual bool moveFocusToNextControlPart();
	virtual bool moveFocusToPreviousControlPart();
	
    // Tooltips
    virtual void clearTooltips();
    virtual void addTooltip(PKRect rect, std::wstring text);

	// Children management
	virtual void deleteAllChildControls();

    // Gets x and y in parent coordinates
    virtual int32_t getAbsoluteX();
    virtual int32_t getAbsoluteY();

	int32_t  getX();
	int32_t  getY();
	uint32_t getW();
	uint32_t getH();

	virtual uint32_t getMinimumWidth()  = 0;
	virtual uint32_t getMinimumHeight() = 0;

	virtual PKControl *findChildControlAt(int32_t x, 
									      int32_t y);
	
	virtual PKView *findObjectById(std::string id);

#ifdef LINUX
	virtual PKView *findObjectByWidget(GtkWidget *w);
#endif

#ifdef WIN32
	virtual PKView *findObjectByWindow(HWND w);

    virtual bool relayWIN32Event(HWND   parent,
                                 UINT   msg, 
							     WPARAM wParam, 
							     LPARAM lParam);
    bool dontTrack;
#endif

#ifdef MACOSX
	virtual PKView *findObjectByControlRef(ControlRef cRef);
#endif
	
	virtual void setNoWidget(bool noWidget);

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

public:

	static PKControl *getControlWithId(int32_t id);

protected:

    PKDialog *dialog;
	int32_t controlId;
    bool showing;

    int32_t getInternalControlId();

	static int32_t nextControlId;
	static std::vector<PKControl *> controls;

public:
    PKDirection focusDirection;

#ifdef WIN32
public:
	HWND hwnd;
    HWND tooltip;
    std::vector<HWND> tooltips;
    LONG_PTR oldWindowLongPtr;
    WNDPROC  oldWndProc;

	static LRESULT CALLBACK PKControlWindowProc(HWND hwnd,
							 		            UINT uMsg,
								 	            WPARAM wParam,
									            LPARAM lParam);

#endif

#ifdef LINUX
protected:
    GtkWidget *widget;

    static gboolean gtk_focus_in_event(GtkWidget     *widget,
                                       GdkEventFocus *event,
                                       gpointer       user_data);

    static gboolean gtk_focus_out_event(GtkWidget     *widget,
                                        GdkEventFocus *event,
                                        gpointer       user_data);
    
    static void gtk_size_allocation_event(GtkWidget     *widget,
                                          GtkAllocation *allocation,
                                          gpointer       user_data);
    
#endif
	bool noWidget;
    bool focused;

#ifdef MACOSX
public:
	ControlPartCode _focusedPart;
	bool haveFocus;	
	
	EventHandlerRef handler;
	
	static pascal OSStatus PKControlEventHandler(EventHandlerCallRef  nextHandler,
												 EventRef             inEvent, 
												 void                *userData);
	
protected:
	ControlRef controlRef;
	ControlID  macId;
		
	std::vector<PKRect>       tooltipRects;
	std::vector<CFStringRef>  tooltipTexts;
	HMControlContentUPP fHelpContentProviderUPP;
	
	static pascal OSStatus HelpContentProvider(ControlRef inControl, 
											   Point inGlobalMouse, 
											   HMContentRequest inRequest, 
											   HMContentProvidedType *outContentProvided, 
											   HMHelpContentPtr ioHelpContent);
#endif

	// From IPKSafeEventHandler
	
	virtual void processSafeEvent(std::string  eventName,
								  void        *payload);
	
	
};

#endif // PKCONTROL_H
