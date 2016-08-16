//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CHECKBOX_H
#define PK_CHECKBOX_H

#include "PKControl.h"

//
// PKCheckBox
//

class PKCheckBox : public PKControl
{
public:
	PKCheckBox();
protected:
	~PKCheckBox();

public:
    void build();

	bool getChecked();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);
	
public:

	static std::string TEXT_WSTRING_PROPERTY;
	static std::string CHECKED_BOOL_PROPERTY;
    static std::string TRANSPARENT_BOOL_PROPERTY;

private:

	void setText(std::wstring text);
	void setChecked(bool checked);

#ifdef LINUX
    static void gtk_pkcheckbox_toggled(GtkToggleButton *togglebutton,
                                       gpointer         user_data);
                                
    void linuxCheckAction();     

#endif 

};

#endif // PK_CHECKBOX_H
