//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_FIXED_LAYOUT_H
#define PK_FIXED_LAYOUT_H

#include "PKLayout.h"

//
// PKFixedLayout
//

#ifdef LINUX
class PKFixedLayout : public PKControl
#else
class PKFixedLayout : public PKLayout
#endif
{
public:

	PKFixedLayout();
protected:
	~PKFixedLayout();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

	PKWindowHandle getWindowHandle();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

    void move(int32_t x, 
			  int32_t y);

    void resize(uint32_t w, 
				uint32_t h);

	void show();
	void hide();

	void enable();
	void disable();

#ifdef WIN32
    virtual bool relayWIN32Event(HWND   parent,
                                 UINT   msg, 
							     WPARAM wParam, 
							     LPARAM lParam);
#endif

private:

};

#endif // PK_FIXED_LAYOUT_H
