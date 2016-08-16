//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//


#include "PKPopupMenu.h"
#include "PKStr.h"

//
// PKPopupMenu
//

#ifdef MACOSX
int PKPopupMenu::lastId = 0;
#endif

PKPopupMenu::PKPopupMenu()
{
    this->selectionMade = false;

#ifdef WIN32

	this->menu = CreatePopupMenu();

#endif
    
#ifdef LINUX

	this->menu = gtk_menu_new();
	gtk_widget_show(this->menu);
	
    g_signal_connect(G_OBJECT(this->menu), "selection-done",
                     G_CALLBACK (PKPopupMenu::gtk_menu_selection_done), this);      

#endif

#ifdef MACOSX
    
    PKPopupMenu::lastId++;
    
    OSStatus err = CreateNewMenu(PKPopupMenu::lastId, 
                                 0, &this->menu);
    
    if(err != noErr)
    {
        this->menu = NULL;
    }
    
#endif

	this->callback = NULL;
}

PKPopupMenu::~PKPopupMenu()
{
#ifdef WIN32

	if(this->menu != NULL)
	{
		DestroyMenu(this->menu);
		this->menu = NULL;
	}

#endif

#ifdef MACOSX
    
    if(this->menu != NULL)
    {
        DisposeMenu(this->menu);
        this->menu = NULL;
    }
    
#endif
}

void PKPopupMenu::setMenuCallback(PKPopupMenuCallback *callback)
{
	this->callback = callback;
}

void PKPopupMenu::insertItem(unsigned int id,
							 std::wstring text,
							 bool needsCheck,
							 std::wstring accel)
{
#ifdef WIN32

	MENUITEMINFO info;
	memset(&info, 0, sizeof(MENUITEMINFO));

	std::wstring title = text;

	if(accel != L"")
	{
		title += L"\t";
		title += accel;
	}

	info.cbSize     = sizeof(MENUITEMINFO);
	info.fMask      = MIIM_DATA | MIIM_ID | MIIM_TYPE;
	info.wID        = id;
	info.dwItemData = id;
	info.fType      = MFT_STRING;
	info.dwTypeData = (TCHAR *) title.c_str();
	info.cch        = title.size();

	InsertMenuItem(this->menu, -1, FALSE, &info);

#endif
    
#ifdef LINUX

    if(this->menu == NULL)
	{
    	return;
	}
	
	std::string textUTF8 = PKStr::wStringToUTF8string(text);
	
	GtkWidget *item = NULL;
	
	if(needsCheck)
	{
	    item = gtk_check_menu_item_new_with_label(textUTF8.c_str());
	}
    else
	{
	    item = gtk_menu_item_new_with_label(textUTF8.c_str());
	}    	
	
    if(item != NULL)
	{
	    this->items.push_back(item);
	    this->ids.push_back(id);
	    
        g_signal_connect(G_OBJECT(item), "activate",
                         G_CALLBACK (PKPopupMenu::gtk_menu_item_activated), this);      

    	gtk_widget_show(item);
        gtk_menu_shell_append(GTK_MENU_SHELL(this->menu), item);
	}

#endif

#ifdef MACOSX
    
    CFStringRef cfText = NULL;
    
    uint32_t size = 0;
    UniChar *ptr  = NULL;
    
    PKStr::wStringToUniChar(text, &size, &ptr);

    if(ptr != NULL)
    {
        cfText = CFStringCreateWithCharacters(NULL,
                                              ptr, 
                                              size);
        
        MenuItemIndex index;
        
        AppendMenuItemTextWithCFString(this->menu,
                                       cfText,
                                       0,
                                       id,
                                       &index);
        
        SetMenuItemCommandID(this->menu, index, id);

        delete [] ptr;
        CFRelease(cfText);
    }
    
#endif
}

void PKPopupMenu::insertSeparator()
{
#ifdef WIN32

	MENUITEMINFO info;
	memset(&info, 0, sizeof(MENUITEMINFO));

	info.cbSize     = sizeof(MENUITEMINFO);
	info.fMask      = MIIM_TYPE;
	info.fType      = MFT_SEPARATOR;

	InsertMenuItem(this->menu, -1, FALSE, &info);


#endif
    
#ifdef LINUX

    if(this->menu == NULL)
	{
    	return;
	}
		
	GtkWidget *item = gtk_separator_menu_item_new();

    if(item != NULL)
	{
    	gtk_widget_show(item);
        gtk_menu_shell_append(GTK_MENU_SHELL(this->menu), item);
	}

#endif
    
#ifdef MACOSX
    
    AppendMenuItemTextWithCFString(this->menu,
                                   NULL,
                                   kMenuItemAttrSeparator,
                                   0,
                                   NULL);
    
#endif
    
}

void PKPopupMenu::insertSubmenu(unsigned int id,
							    std::wstring text,
							    PKPopupMenu   *sub)
{
	if(sub == NULL)
	{
		return;
	}

#ifdef WIN32

	HMENU handle = (HMENU) sub->getHandle();

	MENUITEMINFO info;
	memset(&info, 0, sizeof(MENUITEMINFO));

	info.cbSize     = sizeof(MENUITEMINFO);
	info.fMask      = MIIM_DATA | MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
	info.wID        = id;
	info.dwItemData = id;
	info.fType      = MFT_STRING;
	info.dwTypeData = (TCHAR *) text.c_str();
	info.cch        = text.size();
	info.hSubMenu   = handle;

	InsertMenuItem(this->menu, -1, FALSE, &info);

#endif

#ifdef LINUX

    if(this->menu == NULL)
	{
    	return;
	}
	
	std::string textUTF8 = PKStr::wStringToUTF8string(text);
	
	GtkWidget *item = gtk_menu_item_new_with_label(textUTF8.c_str());

    if(item != NULL)
	{
	    // Add the submenu
	    
	    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), 
	                              GTK_WIDGET((GtkWidget *)sub->getHandle()));
	
	    // Add the item
	
        gtk_menu_shell_append(GTK_MENU_SHELL(this->menu), item);
	}


#endif
    
#ifdef MACOSX
    
    OSStatus err;
    
    MenuRef submenu = (MenuRef) sub->getHandle();
    
    CFStringRef cfText = NULL;

    uint32_t size = 0;
    UniChar *ptr  = NULL;
    
    PKStr::wStringToUniChar(text, &size, &ptr);
    
    if(ptr != NULL)
    {
        MenuItemIndex index;
        
        cfText = CFStringCreateWithCharacters(NULL, ptr, size);
        
        err = AppendMenuItemTextWithCFString(this->menu,
                                             cfText,
                                             0, id,
                                             &index);    
        
        delete [] ptr;
        CFRelease(cfText);
        
        if(err != noErr)
        {
            return;
        }
        
        SetMenuItemHierarchicalMenu(this->menu, index, submenu);
		
		MenuID mid = GetMenuID(submenu);
		int    iid = (int) mid;

		this->submenus[iid] = submenu;
    }
    
#endif
}

void PKPopupMenu::checkItem(unsigned int id,
						    bool checked)
{
#ifdef WIN32

	CheckMenuItem(this->menu, id, 
				  checked ? MF_CHECKED : MF_UNCHECKED);

#endif
    
#ifdef LINUX
    
    for(uint32_t i=0; i<this->ids.size(); i++)
    {
        if(this->ids[i] == id)
        {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(this->items[i]), checked);
            break;
        }                   
    }           

#endif

#ifdef MACOSX
    
    OSStatus      err;
    MenuRef       ref;
    MenuItemIndex index;
    
    err = GetIndMenuItemWithCommandID(this->menu,
                                      id, 1,
                                      &ref, &index);
    
    if(err != noErr)
    {
        return;
    }
    
    CheckMenuItem(ref, index, checked);
    
#endif
}

void PKPopupMenu::enableItem(unsigned int id,
						     bool enabled)
{
#ifdef WIN32

	EnableMenuItem(this->menu, id, 
				   enabled ? MF_ENABLED : (MF_DISABLED | MF_GRAYED));

#endif

#ifdef LINUX
    
    for(uint32_t i=0; i<this->ids.size(); i++)
    {
        if(this->ids[i] == id)
        {
            gtk_widget_set_sensitive(this->items[i], enabled);
            break;
        }                   
    }           

#endif

#ifdef MACOSX

    OSStatus      err;
    MenuRef       ref;
    MenuItemIndex index;
    
    err = GetIndMenuItemWithCommandID(this->menu,
                                      id, 1,
                                      &ref, &index);
    
    if(err != noErr)
    {
        return;
    }
    
    if(enabled)
    {
        EnableMenuItem(ref, index);
    }
    else
    {
        DisableMenuItem(ref, index);
    }
    
#endif
}

void PKPopupMenu::updateItem(unsigned int id,
						     std::wstring text,
						     std::wstring accel)
{
#ifdef WIN32

	MENUITEMINFO info;
	memset(&info, 0, sizeof(MENUITEMINFO));

	std::wstring title = text;

	if(accel != L"")
	{
		title += L"\t";
		title += accel;
	}

	info.cbSize     = sizeof(MENUITEMINFO);
	info.fMask      = MIIM_TYPE;
	info.fType      = MFT_STRING;
	info.dwTypeData = (TCHAR *) title.c_str();
	info.cch        = title.size();

	SetMenuItemInfo(this->menu, id, FALSE, &info);

#endif

#ifdef LINUX
    
    for(uint32_t i=0; i<this->ids.size(); i++)
    {
        if(this->ids[i] == id)
        {
        	std::string textUTF8 = PKStr::wStringToUTF8string(text);
            gtk_menu_item_set_label(GTK_MENU_ITEM(this->items[i]), textUTF8.c_str());   
            break;
        }                   
    }           

#endif

#ifdef MACOSX
    
    OSStatus      err;
    MenuRef       ref;
    MenuItemIndex index;
    
    err = GetIndMenuItemWithCommandID(this->menu,
                                      id, 1,
                                      &ref, &index);
    
    if(err != noErr)
    {
        return;
    }

    CFStringRef cfText = NULL;
    
    uint32_t size = 0;
    UniChar *ptr  = NULL;
    
    PKStr::wStringToUniChar(text, &size, &ptr);
    
    if(ptr != NULL)
    {
        cfText = CFStringCreateWithCharacters(NULL, ptr, size);
        
        SetMenuItemTextWithCFString(ref, index,
                                    cfText);
        
        delete [] ptr;
        CFRelease(cfText);
    }
    
#endif
}

void PKPopupMenu::removeItem(unsigned int id)
{
#ifdef WIN32

	DeleteMenu(this->menu, id, MF_BYCOMMAND);

#endif

#ifdef LINUX
    // Can't be done    
#endif

#ifdef MACOSX
    
    OSStatus      err;
    MenuRef       ref;
    MenuItemIndex index;
    
    err = GetIndMenuItemWithCommandID(this->menu,
                                      id, 1,
                                      &ref, &index);
    
    if(err != noErr)
    {
        return;
    }

    DeleteMenuItem(ref, index);
    
#endif
}

void PKPopupMenu::clear()
{
#ifdef WIN32

	int count = GetMenuItemCount(this->menu);

	for(int i=0; i < count; i++)
	{
		DeleteMenu(this->menu, 0, MF_BYPOSITION);
	}

#endif

#ifdef LINUX
    // Can't be done    
#endif

#ifdef MACOSX
    
	if(this->menu != NULL)
	{
		UInt16 count = CountMenuItems(this->menu);
		DeleteMenuItems(this->menu, 1, count);
    }
	
#endif
}

unsigned int PKPopupMenu::popup(PKWindowHandle w)
{
    this->selectionMade = false;

#ifdef WIN32

	POINT pt;
	GetCursorPos(&pt);

	unsigned int result = TrackPopupMenu(this->menu, TPM_RETURNCMD, 
										 pt.x, pt.y, 
										 0, w, NULL);

	if(result == 0)
	{
	    if(this->callback != NULL)
	    {
            this->callback->menuCancelled();
	    }

        return 0;
	}

	if(this->callback != NULL)
	{
		this->callback->menuCallback(result);
	}

	return result;

#endif

#ifdef LINUX

    if(this->menu == NULL)
	{
    	return 0;
	}
    
    gtk_menu_popup(GTK_MENU(this->menu), NULL, NULL, NULL, NULL, 0,
                   gtk_get_current_event_time());

#endif

#ifdef MACOSX
    
    Point p;
	GetGlobalMouse(&p);
	    
    UInt32        type  = 0;
    SInt16        id    = 0;
    MenuItemIndex index = 0;
	
    GrafPtr oldPort;
	Rect    clip;

    if(w != NULL)
    {
        GetPort(&oldPort);
        SetPort((GrafPtr) w);
		GetPortBounds((GrafPtr) w, &clip);
		SetOrigin(0,0);        
    }

	OSStatus err = ContextualMenuSelect(this->menu, p, 
                         false, kCMHelpItemRemoveHelp,
                         "\p", NULL,
                         &type, &id, &index);
    
    if(w != NULL)
    {
		SetOrigin(clip.left, clip.top);
        SetPort((GrafPtr)oldPort);        
    }

    if(type == kCMMenuItemSelected)
    {
		if(id == GetMenuID(this->menu))
		{
			MenuCommand command;
			GetMenuItemCommandID(this->menu, index, &command);
        
			if(this->callback != NULL)
			{
				this->callback->menuCallback(command);
			}
		}
		else
		{
			// Most likely a submenu

			MenuRef submenu = this->submenus[(int)id];
						
			if(submenu != NULL)
			{
				MenuCommand command;
				GetMenuItemCommandID(submenu, index, &command);
        
				if(this->callback != NULL)
				{
					this->callback->menuCallback(command);
				}
			}
		}
    }    
    else
    {
		if(this->callback != NULL)
		{
			this->callback->menuCancelled();
		}
    }

    return id;

#endif
    
    return 0;
}

#ifdef LINUX

void PKPopupMenu::gtk_menu_item_activated(GtkMenuItem *menuitem,
                                          gpointer     user_data)
{
    PKPopupMenu *menu = (PKPopupMenu *) user_data;
    
    if(menu == NULL)
    {
        return;
    }          
    
    menu->linuxItemActivated(menuitem);                               
}                                     

void PKPopupMenu::linuxItemActivated(GtkMenuItem *item)
{
    if(item == NULL)
    {
        return;
    }          
    
    this->selectionMade = true;

    for(unsigned int i=0; i<this->items.size(); i++)
    {
        if(this->items[i] == (GtkWidget *) item)
        {
            int id = this->ids[i];

            if(this->callback != NULL)
            {
                this->callback->menuCallback(id);       
            }                  

            break;
        }                  
    }                
}              

void PKPopupMenu::gtk_menu_selection_done(GtkMenuShell *menushell,
                                          gpointer      user_data)
{
    PKPopupMenu *menu = (PKPopupMenu *) user_data;
    
    if(menu == NULL)
    {
        return;
    }          

    menu->linuxSelectionDone();                                             
}                             

void PKPopupMenu::linuxSelectionDone()
{
    if(!this->selectionMade)
    {
        if(this->callback != NULL)
        {
            this->callback->menuCancelled();
        }
    }
}

#endif

void *PKPopupMenu::getHandle()
{
	return (void *) this->menu;
}
