//
//
//

#ifndef PK_VECTOR_CONTROL_ROOT_H
#define PK_VECTOR_CONTROL_ROOT_H

#include "PKCustomControl.h"
#include "PKVectorControl.h"
#include "PKVectorAnimation.h"

#include "PKThread.h"
#include "PKCritSec.h"

class PKVectorEngine;

//
// PKVectorControlRoot
//

class PKVectorControlRoot : public PKCustomControl, public IPKRunnable
{
public:

	//
	// Properties
	//

	static std::string DOUBLE_BUFFER_BOOL_PROPERTY;
	static std::string BACK_COLOR_COLOR_PROPERTY;
	static std::string ALPHA_DOUBLE_PROPERTY;

public:
	PKVectorControlRoot();

protected:
	~PKVectorControlRoot();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

    virtual void commitChanges();

    // Animations

	void queueVectorControlAnimation(PKVectorAnimation *anim);

    void cancelAllAnimationsForChild(PKVectorControl *ctrl);
	void clearInputState();

    // Putting a child on top of the Z-order, pass NULL to clear
    void setTopLevelControl(PKVectorControl *child);

	// From PKControl

	void move(int32_t x, 
			  int32_t y);

	void resize(uint32_t x, 
				uint32_t y);

    void relayout();

    void show();
	void hide();

	void enable();
	void disable();

	virtual uint32_t getMinimumWidth();
	virtual uint32_t getMinimumHeight();

    virtual void invalidate(bool recurse = true);
	virtual void invalidateRect(PKRect client);

	virtual void drawIntoVectorEngine(PKVectorEngine *engine);
	
	// From IPKRunnable

    void runThread(PKThread *thread);

	// From PKControl
	
    virtual void doCut();
    virtual void doCopy();
    virtual void doPaste();
    virtual void doSelectAll();

    virtual bool hasSelection();

    virtual void focusAcquired(PKDirection direction);
    virtual void focusLost();
	
	virtual bool moveFocusToNextControlPart();
	virtual bool moveFocusToPreviousControlPart();

	// From PKCustomControl

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

	virtual bool keyDown(unsigned int modifier,
						 PKKey        keyCode,
						 unsigned int virtualCode);

	virtual bool keyUp(unsigned int modifier,
					   PKKey        keyCode,
					   unsigned int virtualCode);

    virtual void characterInput(std::wstring wstr);

public:

	PKVectorEngine *currentEngine;

protected:

	virtual void drawVectorInternal(PKVectorEngine *engine);

#ifdef WIN32
    void drawWin32(PKDrawingDevice device, 
				   PKRect          rect);
#endif
	
#ifdef LINUX

    void drawLinux(GtkWidget   *widget,
                   GdkDrawable *drawable,
                   GdkGC       *gc,
                   PKRect       rect);    
                   
    static void gtk_size_allocation_callback(GtkWidget     *widget,
                                             GtkAllocation *allocation,
                                             gpointer       user_data);
                   
#endif

#ifdef MACOSX
    void drawMacOSX(RgnHandle	 inLimitRgn,
					CGContextRef inContext);
#endif

public:
	PKCustomControl *currentControl;
	PKCustomControl *downControl;
    PKCustomControl *focusControl;
    PKCustomControl *nextFocusControl;
    PKCustomControl *lastFocusControl;
	bool focusSet;
	bool skipFocus;
	bool nextOne;
    bool downHit;
	
    PKVectorControl *topLevelControl;

	// For animations

	PKCritSec animMutex;
	PKThread  animThread;
	std::vector<PKVectorAnimation *> anims;
	PKDrawingDevice currentDevice;
	PKDrawingDevice currentBuffer;

#ifdef WIN32
	HDC mdc;
	HBITMAP bmp;
    bool invalidated;
#endif
};

#endif // PK_VECTOR_CONTROL_ROOT_H
