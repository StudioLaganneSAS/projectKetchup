//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_POPUP_MENU_H
#define PK_POPUP_MENU_H

#include "PKUI.h"
#include "PKObject.h"

#ifdef MACOSX
#include <map>
#endif

//
// PKPopupMenuCallback
//

class PKPopupMenuCallback
{
public:
    
    virtual ~PKPopupMenuCallback() {}

	virtual void menuCallback(unsigned int id) = 0;
	virtual void menuCancelled() = 0;
};

//
// PKPopupMenu
//

class PKPopupMenu : public PKObject
{
public:

	PKPopupMenu();

	void setMenuCallback(PKPopupMenuCallback *callback);

	//
	// Item management
	//

	void insertItem(unsigned int id,
					std::wstring text,
					bool needsCheck = false,
				    std::wstring accel = L"");

	void insertSeparator();

	void insertSubmenu(unsigned int id,
					   std::wstring text,
					   PKPopupMenu *sub);

	void checkItem(unsigned int id,
				   bool checked);

	void enableItem(unsigned int id,
				    bool enabled);

	void updateItem(unsigned int id,
				    std::wstring text,
					std::wstring accel = L"");

	void removeItem(unsigned int id);

	void clear();

	//
	// show() the menu
	//

	unsigned int popup(PKWindowHandle w);

public:

	//
	// Platform dependent
	//

	void *getHandle();

private:

#ifdef WIN32
	HMENU menu;
#endif

#ifdef LINUX
    GtkWidget *menu;
    std::vector<GtkWidget*> items;
    std::vector<unsigned int> ids;
    
    static void gtk_menu_item_activated(GtkMenuItem *menuitem,
                                        gpointer     user_data);
                                        
    static void gtk_menu_selection_done(GtkMenuShell *menushell,
                                        gpointer      user_data);
                                        
    void linuxItemActivated(GtkMenuItem *item);
    void linuxSelectionDone();
#endif

#ifdef MACOSX
	MenuRef    menu;
    static int lastId;
	
	std::map <int, MenuRef> submenus;
#endif
    
	PKPopupMenuCallback *callback;
	bool selectionMade;

protected:

    virtual ~PKPopupMenu();
};

#endif // PK_POPUP_MENU_H
