//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BUTTON_H
#define PK_BUTTON_H

#include "PKControl.h"

//
// PKButton
//

class PKButton : public PKControl
{
public:

	static std::string CAPTION_WSTRING_PROPERTY;
    static std::string DEFAULT_BOOL_PROPERTY;
	static std::string OVERRIDE_MIN_SIZE_BOOL_PROPERTY;

public:
	PKButton();
protected:
	~PKButton();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);

	void setCaption(std::wstring text);

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

#ifdef LINUX

    static void gtk_pkbutton_clicked(GtkButton *button,
                                     gpointer   user_data);
                              
    void linuxButtonClicked();    

#endif
};

#endif // PK_BUTTON_H
