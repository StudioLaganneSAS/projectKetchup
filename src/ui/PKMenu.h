//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_MENU_H
#define PK_MENU_H

#include "PKObject.h"
#include "PKUI.h"
#include "PKControl.h"
#include "PKSettings.h"

//
// Defines
//

#ifdef WIN32
typedef HMENU PKMenuHandle;
#endif

#ifdef MACOSX
typedef MenuRef PKMenuHandle;
#endif

#ifdef LINUX
typedef GtkWidget *PKMenuHandle;
#endif

#ifdef MACOSX
enum {
    kPKMenuData   = 'PKMD',
    kPKItemHandle = 'PKMH',
};
#endif

class PKWindow;

//
// PKMenu
//

class PKMenu : public PKControl
{
public:

	// Properties
	
	static std::string MAC_ABOUT_MENUID_STRING_PROPERTY;
	static std::string MAC_ABOUT_TEXT_WSTRING_PROPERTY;
	static std::string MAC_PREFS_MENUID_STRING_PROPERTY;
	static std::string MAC_QUIT_MENUID_STRING_PROPERTY;

public:

    PKMenu();

protected:
    virtual ~PKMenu();

public:
    void build();

    PKMenuHandle getMenuHandle();
    void setWindow(PKWindow *window);
	void setSettings(PKSettings *settings);

    // Helper

    static PKMenu *loadMenuFromXML(std::string menuXML);

    // Item management

    void setItemEnabled(std::string id, bool enabled);
    void setItemChecked(std::string id, bool checked);

    PKMenu *findSubmenu(std::string id);
    PKMenu *findSubmenuByHandle(void *handle);

	void updateItemText(std::string id);
	void updateItemText(std::string id, std::wstring text);

	// Menu management

	void insertItem(std::string id,
					std::wstring text,
					std::wstring accel = L"");

	void insertSeparator();
	void clear();

	// i18n
	void updateUIStrings();

	// recent files
	void updateRecentFilesMenu();
	void clearRecentFiles();

#ifdef WIN32
    void getAccelerators(std::vector<ACCEL *> *outArray);
    void drawItem(WPARAM wParam, LPARAM lParam);
#endif

#ifdef LINUX
    void setTopLevel(bool top);
    GtkAccelGroup *getGtkAccelGroup();
#endif

    // From PKControl
    
    uint32_t getMinimumWidth();
    uint32_t getMinimumHeight();

protected:

    PKWindow *window;
    std::vector<PKMenu*> submenus;
	PKSettings *settings;

#ifdef WIN32
    HMENU handle;
#endif

#ifdef MACOSX
    MenuRef handle;
#endif

#ifdef LINUX
    GtkWidget *handle;
    bool topLevel;
    std::vector<GtkWidget *> items;
    std::vector<int> ids;
    
    static GtkAccelGroup *accelGroup;
    
    static void gtk_menu_item_activated(GtkMenuItem *menuitem,
                                        gpointer     user_data);
                                        
    void linuxItemActivated(GtkMenuItem *item);
    
#endif

    static int lastId;
};

#endif // PK_MENU_H
