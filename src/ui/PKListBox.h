//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_LISTBOX_H
#define PK_LISTBOX_H

#include "PKControl.h"

//
// PKListBox
//

class PKListBox : public PKControl
{
public:

//
// PROPERTIES
//

    static std::string ITEMS_WSTRINGLIST_PROPERTY;
    static std::string ALWAYS_SCROLL_BOOL_PROPERTY;
    static std::string SELECTED_ITEM_INT32_PROPERTY;

public:
	PKListBox();
protected:
	~PKListBox();

public:
    virtual void build();

    void clearItems();
    void addItem(std::wstring item);
	
    void setCurrentSelection(int32_t index);
	int32_t getCurrentSelection();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:
		
#ifdef MACOSX
	std::vector<int> ids;
#endif

#ifdef LINUX
    GtkListStore *store;
    GtkWidget *tree;
    
    static gboolean gtk_pklistbox_selection_changed(GtkTreeSelection *treeselection,
                                                    gpointer     user_data);    
    
    void linuxSelectionChanged();    
#endif

public:
	
#ifdef MACOSX
	static std::vector<CFStringRef> items;
#endif
};

#endif // PK_LISTBOX_H
