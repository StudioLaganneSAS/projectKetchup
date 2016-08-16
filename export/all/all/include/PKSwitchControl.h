//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SWITCHCONTROL_H
#define PK_SWITCHCONTROL_H

#include "PKControl.h"
#include "PKLayout.h"
#include "PKCritSec.h"

class PKDialog;

//
// PKSwitchControl
//

class PKSwitchControl : public PKControl
{
public:
	PKSwitchControl();
protected:
	~PKSwitchControl();

public:

    void build();
    void initialize(PKDialog *dialog);
    void destroy();

    virtual PKWindowHandle getWindowHandle();

    void clearTabs();
	void addTab(PKLayout *object);

	void setCurrentTab(unsigned int index);

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

public:
	unsigned int currentTab;

#ifdef WIN32
    virtual bool relayWIN32Event(HWND   parent,
                                 UINT   msg, 
							     WPARAM wParam, 
							     LPARAM lParam);
#endif
};

#endif // PK_SWITCHCONTROL_H
