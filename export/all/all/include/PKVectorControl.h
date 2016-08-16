//
//
//

#ifndef PK_VECTOR_CONTROL_H
#define PK_VECTOR_CONTROL_H

#include "PKCustomControl.h"

class PKVectorEngine;
class PKVectorControlRoot;

//
// PKVectorControl
//

class PKVectorControl : public PKCustomControl
{
public:

	//
	// Properties
	//

	static std::string ALPHA_DOUBLE_PROPERTY;
	static std::string SHARED_VECTOR_ENGINE_BOOL_PROPERTY;
	static std::string DOUBLE_BUFFER_BOOL_PROPERTY;

public:
	PKVectorControl();

protected:
	~PKVectorControl();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();
	
	virtual void setControlRoot(PKVectorControlRoot *r);

	virtual void drawVector(PKVectorEngine *engine,
		                    int32_t x, int32_t y);

	virtual void drawVectorInternal(PKVectorEngine *engine,
									int32_t x, int32_t y);

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

    virtual void invalidate(bool recurse = true);
	
	virtual bool wantsKeyboardFocus();
	
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

protected:
	PKVectorControlRoot *controlRoot;

#ifdef WIN32
	HDC mdc;
	HBITMAP bmp;
#endif
};

#endif // PK_VECTOR_CONTROL_H
