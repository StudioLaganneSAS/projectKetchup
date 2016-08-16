//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_GROUPBOX_H
#define PK_GROUPBOX_H

#include "PKControl.h"

//
// PKGroupBox
//

class PKGroupBox : public PKControl
{
public:

	static std::string CAPTION_WSTRING_PROPERTY;
	static std::string MARGIN_INT32_PROPERTY;
	static std::string TOPMARGIN_INT32_PROPERTY;
	static std::string CHECKBOX_BOOL_PROPERTY;
	static std::string CHECKED_BOOL_PROPERTY;

public:
	PKGroupBox();
protected:
	~PKGroupBox();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

    PKWindowHandle getWindowHandle();

	void setCaption(std::wstring cap);

	void setChecked(bool checked);
	bool getChecked();

	void move(int32_t x, 
			  int32_t y);

	void resize(uint32_t x, 
				uint32_t y);

	void show();
	void hide();

	void enable();
	void disable();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

#ifdef WIN32
public:

	WNDPROC lpOldProc;

	static LRESULT CALLBACK GroupWinProc(HWND hwnd,
							 		     UINT uMsg,
								 	     WPARAM wParam,
									     LPARAM lParam);

    HWND checkbox;
	int  checkw;
	int  checkh;

#endif
};

#endif // DWP_GROUPBOX_H
