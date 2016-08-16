//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKLAYOUT_H
#define PKLAYOUT_H

#include "PKControl.h"

//
// PKLayoutType
//

typedef enum {

	PK_LAYOUT_HORIZONTAL = 0,
	PK_LAYOUT_VERTICAL   = 1,

} PKLayoutType;

//
// PKLayout
//

class PKLayout : public PKControl
{
public:

	static std::string MARGIN_INT32_PROPERTY;
	static std::string SPACING_INT32_PROPERTY;
	static std::string TYPE_INT32_PROPERTY;

public:

	PKLayout();
protected:
	~PKLayout();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

	virtual PKWindowHandle getWindowHandle();

	void setLayoutType(PKLayoutType type);
	PKLayoutType getLayoutType();

	void setMargin(uint32_t margin);
	void setSpacing(uint32_t spacing);

	virtual uint32_t getMinimumWidth();
	virtual uint32_t getMinimumHeight();

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

#endif // PKLAYOUT_H
