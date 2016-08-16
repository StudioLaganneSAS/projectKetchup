//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_TABCONTROL_H
#define PK_TABCONTROL_H

#include "PKControl.h"
#include "PKLayout.h"
#include "PKCritSec.h"

class PKDialog;

//
// PKTabControl
//

class PKTabControl : public PKControl
{
public:

    static std::string TABS_WSTRINGLIST_PROPERTY;

public:
	PKTabControl();
protected:
	~PKTabControl();

public:
    void setParentDialog(PKDialog *parent);

    void build();
    void initialize(PKDialog *dialog);
    void destroy();

    // From PKControl
#ifdef WIN32
    virtual HBRUSH getBackgroundBrush();
#endif

    // From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

    void clearTabs();

	void addTab(PKLayout    *object, 
				std::wstring tabTitle);

	void setCurrentTab(unsigned int index);

    // Gets x and y in parent coordinates
    int32_t getAbsoluteX();
    int32_t getAbsoluteY();

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
	HBRUSH getBkgBrush();
public:

	WNDPROC lpOldProc;

	static LRESULT CALLBACK TabWinProc(HWND hwnd,
							 		   UINT uMsg,
								 	   WPARAM wParam,
									   LPARAM lParam);
#endif

	PKDialog *dialog;

private:

	PKCritSec mutex;

#ifdef WIN32
	HBRUSH backBrush;
#endif

public:
	unsigned int currentTab;
};

#endif // PK_TABCONTROL_H
