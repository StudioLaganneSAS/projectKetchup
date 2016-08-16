//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_COMBOBOX_H
#define PK_COMBOBOX_H

#include "PKControl.h"

#ifdef MACOSX
#include "PKPopupMenu.h"
#endif

// 
// PKComboBox
//

class PKComboBox : public PKControl
{
public:

    static std::string ITEMS_WSTRINGLIST_PROPERTY;
	static std::string CURRENTITEM_INT32_PROPERTY;

public:
	PKComboBox();
protected:
	~PKComboBox();

public:
    void build();

	void clearItems();
	void addItem(std::wstring text);

	// Pass -1 to remove selection
	void selectItem(int index);

	// Returns -1 if no selection
	int getSelectedItem();
	
	// Return L"" if no selecte
	std::wstring getSelectedText();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);
	
private:

#ifdef MACOSX
	PKPopupMenu *menu;
#endif

#ifdef LINUX
    uint32_t numItems;
    bool recurse;
    gulong handler_id;
    
    static void gtk_pkcombobox_changed(GtkComboBox *widget,
                                       gpointer     user_data);
                                       
    void linuxComboBoxChanged();
                                           
#endif
};

#endif // PK_COMBOBOX_H
