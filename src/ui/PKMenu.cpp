//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKMenu.h"
#include "PKMenuItem.h"
#include "PKSeparator.h"
#include "PKStr.h"
#include "PKObjectLoader.h"
#include "PKUIFactory.h"
#include "PKWindow.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKOS.h"
#include "PKClipboard.h"

#ifdef LINUX
GtkAccelGroup *PKMenu::accelGroup = NULL;
#endif

//
// Properties
//

std::string PKMenu::MAC_ABOUT_MENUID_STRING_PROPERTY = "macAboutMenuId";
std::string PKMenu::MAC_ABOUT_TEXT_WSTRING_PROPERTY  = "macAboutText";
std::string PKMenu::MAC_PREFS_MENUID_STRING_PROPERTY = "macPrefsMenuId";
std::string PKMenu::MAC_QUIT_MENUID_STRING_PROPERTY  = "macQuitMenuId";


//
// Static
//

int PKMenu::lastId = 10000;

//
// PKMenu
//

PKMenu::PKMenu()
{
	PKOBJECT_ADD_STRING_PROPERTY(macAboutMenuId, "");
	PKOBJECT_ADD_WSTRING_PROPERTY(macAboutText, L"NOT_SET");
	PKOBJECT_ADD_STRING_PROPERTY(macPrefsMenuId, "");
	PKOBJECT_ADD_STRING_PROPERTY(macQuitMenuId, "");
	
    this->window   = NULL;
	this->settings = NULL;

#ifdef WIN32
    this->handle = NULL;
#endif

#ifdef MACOSX
    this->handle = NULL;
#endif

#ifdef LINUX
    this->handle   = NULL;
    this->topLevel = false;
    this->widget   = NULL;
    
    if(PKMenu::accelGroup == NULL)
    {
        PKMenu::accelGroup = gtk_accel_group_new();
    }
    
#endif
}

PKMenu::~PKMenu()
{
#ifdef WIN32

    if(this->handle != NULL)
    {
        DestroyMenu(this->handle);
    }

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKObject *child = this->getChildAt(i);

		if(child)
		{
			child->release();
		}
	}

#endif

#ifdef MACOSX

	if(this->handle != NULL)
	{
		CFRelease(this->handle);
	}
	
#endif
}

void PKMenu::setWindow(PKWindow *w)
{
    this->window = w;
    
	for(unsigned int i=0; i < this->submenus.size(); i++)
	{
        this->submenus[i]->setWindow(w);
    }
}

void PKMenu::setSettings(PKSettings *settings)
{
	this->settings = settings;
}

// recent files
void PKMenu::updateRecentFilesMenu()
{
	if(this->settings == NULL)
	{
		return;
	}

	// Find the recent files submenu

	PKMenu *recentFilesSubmenu = this->findSubmenu("MENU_ID_RECENT_FILES");

	if(recentFilesSubmenu)
	{
		// Clear the menu
		recentFilesSubmenu->clear();

		// See how many items we have
		uint32_t count = this->settings->getNumberOfRecentFiles();

		if(count == 0)
		{
			// Create the empty menu
			recentFilesSubmenu->insertItem("ID_RECENT_FILES_NONE", PK_i18n(L"No recent files yet...")); 
			recentFilesSubmenu->setItemEnabled("ID_RECENT_FILES_NONE", false);
		}
		else
		{
			// Create the items and the clear item

			for(uint32_t i=0; i<count; i++)
			{
				std::wstring fileName = this->settings->getRecentFileAt(i);
				recentFilesSubmenu->insertItem("ID_RECENT_FILES_"+PKStr::uint32ToString(i+1), 
											   PKStr::uint32ToWstring(i+1) + L" - " + fileName);
			}

			recentFilesSubmenu->insertSeparator();
			recentFilesSubmenu->insertItem("ID_RECENT_FILES_CLEAR", PK_i18n(L"Clear this Menu")); 
		}
	}
}

void PKMenu::clearRecentFiles()
{
	if(this->settings)
	{
		this->settings->clearRecentFiles();
		this->updateRecentFilesMenu();
	}
}


#ifdef LINUX
void PKMenu::setTopLevel(bool top)
{
    this->topLevel = top;
}
#endif

PKMenuHandle PKMenu::getMenuHandle()
{
#ifdef WIN32
    return this->handle;
#endif
	
#ifdef MACOSX
	return this->handle;
#endif

#ifdef LINUX
	return this->handle;
#endif
}

void PKMenu::build()
{
#ifdef LINUX

    // Create the menu

    if(this->handle == NULL)
    {
        if(this->topLevel)
        {
            this->handle = gtk_menu_bar_new();
        }
        else
        {
            this->handle = gtk_menu_new();
            
            gtk_menu_set_accel_group(GTK_MENU(this->handle), 
                                     this->accelGroup);
        }
    }
	else
	{
		return;
	}
	
	this->widget = this->handle;
	gtk_widget_show(this->widget);

    // Add children

    uint32_t count = this->getChildrenCount();

    for(uint32_t i=0; i < count; i++)
    {
        PKObject *child = this->getChildAt(i);

        PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);

        if(item)
        {
            // Add menu item

            PKVariant *t = item->get(PKMenuItem::TEXT_WSTRING_PROPERTY);
            std::wstring text = PKVALUE_WSTRING(t);

            PKVariant *s = item->get(PKMenuItem::SHORTCUT_STRING_PROPERTY);
            std::string accel = PKVALUE_STRING(s);

            PKVariant *e = item->get(PKMenuItem::ENABLED_BOOL_PROPERTY);
            bool enabled = PKVALUE_BOOL(e);

            PKVariant *c = item->get(PKMenuItem::CHECKED_BOOL_PROPERTY);
            bool checked = PKVALUE_BOOL(c);

            PKVariant *nc = item->get(PKMenuItem::NEEDS_CHECKBOX_BOOL_PROPERTY);
            bool needs_check = PKVALUE_BOOL(nc);

            PKVariant *i = item->get(PKMenuItem::ICON_WSTRING_PROPERTY);
            std::wstring icon = PKVALUE_WSTRING(i);

            // Check if the item has a submenu

            GtkWidget *submenuHandle = NULL;

            if(item->getChildrenCount() == 1)
            {
                PKMenu *submenu = dynamic_cast<PKMenu*>(item->getChildAt(0));

                if(submenu)
                {
                    // TODO: connect to "activate" to catch 
                    // the "edit" menu showing up 
                
                    this->submenus.push_back(submenu);
                    
                    submenu->setWindow(this->window);
                    submenu->build();
                    submenuHandle = submenu->getMenuHandle();
                }
            }

            PKMenu::lastId++;

            // Create & add the item
            
	        std::string textUTF8 = PKStr::wStringToUTF8string(text);
	
	        GtkWidget *gitem = NULL;
	        
	        textUTF8 = PKStr::replaceString("&", "_", textUTF8);
	        
            if(icon != L"")
            {
    	        gitem = gtk_image_menu_item_new_with_mnemonic(textUTF8.c_str());

    	        // Load the image

		        PKImage *bitmap = NULL;
	
		        PKImageURIType type;
		        std::wstring   image;
	
		        PKImage::processURI(icon, &type, &image);
	
		        if(type == PKIMAGE_URI_RESOURCE)
		        {
			        bitmap = PKImageLoader::loadBitmapFromResource(image);
		        }
	
		        if(type == PKIMAGE_URI_FILENAME)
		        {
			        bitmap = PKImageLoader::loadBitmapFromFilename(image);
		        }
	
		        if(bitmap != NULL)
		        {
		            // Create an image widget

                    GtkWidget *image = gtk_image_new();
                    
	                // Invert R & B
	                uint32_t width  = bitmap->getWidth();
	                uint32_t height = bitmap->getWidth();
	                uint8_t *pdata  = (uint8_t *) bitmap->getPixels();
	
	                for(unsigned int j=0; j < height; j++)
	                {
		                for(unsigned int i=0; i < width; i++)
		                {
		                    uint8_t temp = pdata[0];
		                     
			                pdata[0] = pdata[2];
     		                pdata[2] = temp;

			                pdata +=4;
		                }
	                }

                    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*) bitmap->getPixels(),
                                                            GDK_COLORSPACE_RGB,
                                                            TRUE, 8, width, height, 
                                                            width*4, 
                                                            NULL, NULL);

                    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
                    gtk_widget_show(image);
	            
		            // Assign to item
		            gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(gitem), image);
		            gtk_image_menu_item_set_always_show_image(GTK_IMAGE_MENU_ITEM(gitem), TRUE);
                }
            }
            else
            {
                if(!needs_check)
	            {
        	        gitem = gtk_menu_item_new_with_mnemonic(textUTF8.c_str());
	            }
                else
	            {
        	        gitem = gtk_check_menu_item_new_with_mnemonic(textUTF8.c_str());
	            }
            }
                        
            if(gitem != NULL)
	        {
	            item->setId(PKMenu::lastId);
	            item->setMenu(this);
	            
	            this->items.push_back(gitem);
	            this->ids.push_back(PKMenu::lastId);

                // Add submenu if any
	
	            if(submenuHandle != NULL)
    	        {
            	    gtk_menu_item_set_submenu(GTK_MENU_ITEM(gitem), 
            	                              GTK_WIDGET(submenuHandle));
    	        }

                if(accel != "")
    	        {
    	            GdkModifierType mod;
    	            guint key;
    	            
    	            PKUIParseAccelerator(accel, &mod, &key);
     	        
                    gtk_widget_add_accelerator(gitem, "activate", 
                                               PKMenu::accelGroup,
                                               key, mod, GTK_ACCEL_VISIBLE);
    	        }
	
                gtk_menu_shell_append(GTK_MENU_SHELL(this->handle), gitem);

                if(needs_check)
    	        {
                    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gitem), checked);
    	        }

                g_signal_connect(G_OBJECT(gitem), "activate",
                                 G_CALLBACK (PKMenu::gtk_menu_item_activated), this);      

                gtk_widget_set_state(gitem, (enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE));
            	gtk_widget_show(gitem);
	        }
        }
        else
        {
            PKSeparator *sep = dynamic_cast<PKSeparator*>(child);

            if(sep)
            {
                // Add the separator

	            GtkWidget *gitem = gtk_separator_menu_item_new();

                if(gitem != NULL)
	            {
                    gtk_menu_shell_append(GTK_MENU_SHELL(this->handle), gitem);
                	gtk_widget_show(gitem);
	            }
            }
        }
    }

#endif

#ifdef WIN32
    
    // Create the menu

    if(this->handle == NULL)
    {
        this->handle = CreateMenu();
    }
	else
	{
		return;
	}

    // Add children

    uint32_t count = this->getChildrenCount();

    for(uint32_t i=0; i < count; i++)
    {
        PKObject *child = this->getChildAt(i);

        PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);

        if(item)
        {
            // Add menu item

            PKVariant *t = item->get(PKMenuItem::TEXT_WSTRING_PROPERTY);
            std::wstring text = PKVALUE_WSTRING(t);

            PKVariant *s = item->get(PKMenuItem::SHORTCUT_STRING_PROPERTY);
            std::string accel = PKVALUE_STRING(s);

            PKVariant *e = item->get(PKMenuItem::ENABLED_BOOL_PROPERTY);
            bool enabled = PKVALUE_BOOL(e);

            PKVariant *c = item->get(PKMenuItem::CHECKED_BOOL_PROPERTY);
            bool checked = PKVALUE_BOOL(c);

            PKVariant *r = item->get(PKMenuItem::RIGHT_BOOL_PROPERTY);
            bool right = PKVALUE_BOOL(r);

            PKVariant *i = item->get(PKMenuItem::ICON_WSTRING_PROPERTY);
            std::wstring icon = PKVALUE_WSTRING(i);

            MENUITEMINFO info;
	        memset(&info, 0, sizeof(MENUITEMINFO));

	        std::wstring title = text;

	        if(accel != "")
	        {
		        title += L"\t";
                title += PKStr::stringToWString(accel);
	        }

            // Check if the item has a submenu

            HMENU submenuHandle = NULL;

            if(item->getChildrenCount() == 1)
            {
                PKMenu *submenu = dynamic_cast<PKMenu*>(item->getChildAt(0));

                if(submenu)
                {
                    submenu->build();
                    submenuHandle = submenu->getMenuHandle();
                    this->submenus.push_back(submenu);
                }
            }

            PKMenu::lastId++;

            info.cbSize     = sizeof(MENUITEMINFO);
	        info.fMask      = MIIM_DATA | MIIM_ID | MIIM_STRING;
	        info.wID        = PKMenu::lastId;
	        info.dwItemData = (ULONG_PTR) (LPVOID) item;
	        info.fType      = MFT_STRING;
	        info.dwTypeData = (TCHAR *) title.c_str();
	        info.cch        = title.size();

            if(icon != L"")
            {
				PKImage *bitmap = NULL;
			
				PKImageURIType type;
				std::wstring   image;
			
				PKImage::processURI(icon, &type, &image);
			
				if(type == PKIMAGE_URI_RESOURCE)
				{
					bitmap = PKImageLoader::loadBitmapFromResource(image);
				}
			
				if(type == PKIMAGE_URI_FILENAME)
				{
					bitmap = PKImageLoader::loadBitmapFromFilename(image);
				}
			
				if(bitmap != NULL)
				{
					info.fMask |= MIIM_BITMAP;

                    if(PKOS::isVistaOrAbove())
                    {
                        // We just need an ARGB bitmap

                        int32_t w = bitmap->getWidth();
                        int32_t h = bitmap->getHeight();

	                    HBITMAP bmp  = NULL;
                        void   *bits = NULL;

	                    BITMAPINFO dibInfo;
	                    memset(&dibInfo, 0, sizeof(BITMAPINFO));

	                    dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
	                    dibInfo.bmiHeader.biWidth         =  w;
                        dibInfo.bmiHeader.biHeight        =  -h;
	                    dibInfo.bmiHeader.biBitCount      =  32;
	                    dibInfo.bmiHeader.biClrUsed       =  0;
	                    dibInfo.bmiHeader.biClrImportant  =  0;
	                    dibInfo.bmiHeader.biCompression   =  BI_RGB;
	                    dibInfo.bmiHeader.biPlanes        =  1;
	                    dibInfo.bmiHeader.biSizeImage     =  w*h*4;
	                    dibInfo.bmiHeader.biXPelsPerMeter =  0;
	                    dibInfo.bmiHeader.biYPelsPerMeter =  0;

	                    bmp = CreateDIBSection(NULL, &dibInfo, DIB_RGB_COLORS, &bits, NULL, 0);

                        if(bmp == NULL)
                        {
							delete bitmap;
                            continue;
                        }

						// Alpha premultiply

						uint8_t *pdata = (uint8_t *) bitmap->getPixels();

						for(unsigned int j=0; j <  bitmap->getHeight(); j++)
						{
							for(unsigned int i=0; i < bitmap->getWidth(); i++)
							{
								if(pdata[3] != 255)
								{
									pdata[0] = (uint8_t) ((int)pdata[0]*(int)pdata[3]/255);
									pdata[1] = (uint8_t) ((int)pdata[1]*(int)pdata[3]/255);
									pdata[2] = (uint8_t) ((int)pdata[2]*(int)pdata[3]/255);
								}
							
								pdata +=4;
							}
						}

                        // Copy pixels
                        memcpy(bits, bitmap->getPixels(), w*h*4);
						delete bitmap;

                        // Assign
                        info.hbmpItem = bmp;
                    }
                    else
                    {
                        // 2000, XP
                        info.hbmpItem = HBMMENU_CALLBACK;
                        item->setIcon(bitmap);

                        int32_t w = bitmap->getWidth();
                        int32_t h = bitmap->getHeight();

                        // Create DIB bitmap

                        BITMAPV5HEADER bi;
                        void *lpBits = NULL;
                        HBITMAP hBitmap = NULL;

                        ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
                        
                        bi.bV5Size        = sizeof(BITMAPV5HEADER);
                        bi.bV5Width       = w;
                        bi.bV5Height      = -h;
                        bi.bV5Planes      = 1;
                        bi.bV5BitCount    = 32;
                        bi.bV5Compression = BI_BITFIELDS;

                        bi.bV5RedMask   =  0x00FF0000;
                        bi.bV5GreenMask =  0x0000FF00;
                        bi.bV5BlueMask  =  0x000000FF;
                        bi.bV5AlphaMask =  0xFF000000; 

                        HDC hDC = GetDC(NULL);

                        hBitmap = CreateDIBSection(hDC, 
                                                   (BITMAPINFO *)&bi, 
                                                   DIB_RGB_COLORS, 
                                                   (void **)&lpBits, 
                                                   NULL, (DWORD)0);

                        ReleaseDC(NULL, hDC);
                            
                        if(hBitmap != NULL && 
                           lpBits  != NULL)
                        {
                            memcpy(lpBits, bitmap->getPixels(), w*h*4);
                        }

                        // Create Mask

                        HBITMAP hMonoBitmap = CreateBitmap(w, h, 1, 1, NULL);

                        ICONINFO iinfo;
                        memset(&iinfo, 0, sizeof(ICONINFO));

                        iinfo.fIcon    = TRUE;
                        iinfo.hbmColor = hBitmap;
                        iinfo.hbmMask  = hMonoBitmap;

                        HICON icon = CreateIconIndirect(&iinfo);
                        item->setHIcon(icon);

                        DeleteObject(hBitmap);          
                        DeleteObject(hMonoBitmap); 

                    }
                }
            }

            if(right)
            {
			    info.fMask = MIIM_DATA | MIIM_ID | MIIM_TYPE;
	            info.fType |= MFT_RIGHTJUSTIFY;
            }

            if(submenuHandle)
            {
                info.fMask    |= MIIM_SUBMENU;
                info.hSubMenu  = submenuHandle;
            }

	        InsertMenuItem(this->handle, -1, FALSE, &info);
            item->setId(PKMenu::lastId);
            item->setMenu(this);

            EnableMenuItem(this->handle, PKMenu::lastId, enabled ? MF_ENABLED : (MF_DISABLED | MF_GRAYED));
            CheckMenuItem(this->handle, PKMenu::lastId, checked ? MF_CHECKED : MF_UNCHECKED);
        }
        else
        {
            PKSeparator *sep = dynamic_cast<PKSeparator*>(child);

            if(sep)
            {
	            MENUITEMINFO info;
	            memset(&info, 0, sizeof(MENUITEMINFO));

	            info.cbSize     = sizeof(MENUITEMINFO);
	            info.fMask      = MIIM_TYPE;
	            info.fType      = MFT_SEPARATOR;

                InsertMenuItem(this->handle, -1, FALSE, &info);
            }
        }
    }

#endif

#ifdef MACOSX
	
    // Create the menu
	
    if(this->handle == NULL)
    {
		PKMenu::lastId++;
		CreateNewMenu(PKMenu::lastId, 0, &this->handle);
    }
	else
	{
		return;
	}
	
	// Add items
	
    uint32_t count = this->getChildrenCount();
	
    for(uint32_t i=0; i < count; i++)
    {
        PKObject *child = this->getChildAt(i);
		  
        PKMenuItem *mitem = dynamic_cast<PKMenuItem*>(child);
		
        if(mitem)
        {
            // Add menu item
			
            PKVariant *t = mitem->get(PKMenuItem::TEXT_WSTRING_PROPERTY);
            std::wstring text = PKVALUE_WSTRING(t);
			
            PKVariant *s = mitem->get(PKMenuItem::SHORTCUT_STRING_PROPERTY);
            std::string accel = PKVALUE_STRING(s);

            PKVariant *e = mitem->get(PKMenuItem::ENABLED_BOOL_PROPERTY);
            bool enabled = PKVALUE_BOOL(e);
			
            PKVariant *c = mitem->get(PKMenuItem::CHECKED_BOOL_PROPERTY);
            bool checked = PKVALUE_BOOL(c);

			CFStringRef cfText = PKStr::wStringToCFString(text);

			if(mitem->getChildrenCount() == 1)
			{
				// It has a submenu
				
				PKMenu *submenu = dynamic_cast<PKMenu*>(mitem->getChildAt(0));

				if(submenu)
				{
					submenu->build();
					
					MenuRef submenuHandle = submenu->getMenuHandle();

					// Set title
					
					SetMenuTitleWithCFString(submenuHandle, cfText);
					
					// Add an empty item
					
					PKMenu::lastId++;

					MenuItemIndex item;
					AppendMenuItemTextWithCFString(this->handle, cfText,  0, 0, &item);
                    mitem->setId((int32_t)item);
                    mitem->setMenu(this);
					
					uint32_t data = (uint32_t)(void *) mitem;

					SetMenuItemProperty(this->handle, item, 
										kPKMenuData, kPKItemHandle, 
										sizeof(data), &data);
					
					if(enabled) EnableMenuItem(this->handle, item);
					else DisableMenuItem(this->handle, item);
					
					if(checked) SetItemMark(this->handle, item, 0x12);
					else SetItemMark(this->handle, item, 0x00);
					
					if(accel != "")
					{
						UInt8  mod;
						UInt16 key;
						
						PKUIParseAccelerator(accel, &mod, &key);
						
						SetMenuItemCommandKey(this->handle, item, false, key);
						SetMenuItemModifiers(this->handle, item, mod);
					}
					
					// Add the submenu
					
					SetMenuItemHierarchicalMenu(this->handle, item, submenuHandle);
					this->submenus.push_back(submenu);
				}
			}
			else
			{
				PKMenu::lastId++;
				
				MenuItemIndex item;
				AppendMenuItemTextWithCFString(this->handle, cfText,  0, 0, &item);
                mitem->setId((int32_t)item);
                mitem->setMenu(this);

				if(enabled) EnableMenuItem(this->handle, item);
				else DisableMenuItem(this->handle, item);

				if(checked) SetItemMark(this->handle, item, 0x12);
				else SetItemMark(this->handle, item, 0x00);

				if(accel != "")
				{
					UInt8  mod;
					UInt16 key;
					
					PKUIParseAccelerator(accel, &mod, &key);
					
					SetMenuItemCommandKey(this->handle, item, false, key);
					SetMenuItemModifiers(this->handle, item, mod);
				}

				uint32_t data = (uint32_t) (void *) mitem;
				
				SetMenuItemProperty(this->handle, item, 
									kPKMenuData, kPKItemHandle, 
									sizeof(data), &data);
			}
			

			CFRelease(cfText);
		}
        else
        {
            PKSeparator *sep = dynamic_cast<PKSeparator*>(child);
			
            if(sep)
            {
				MenuItemIndex item;
				AppendMenuItemTextWithCFString(this->handle, CFSTR("-"),  0, 0, &item);
			}
		}
	}
	
#endif
}

//
// Helper
//

PKMenu *PKMenu::loadMenuFromXML(std::string menuXML)
{
	PKUIFactory factory;
	PKObjectLoader loader;

	loader.addObjectFactory(&factory);
	
	PKObject *rootMenu = loader.loadFromXML(menuXML);

    if(rootMenu)
    {
        PKMenu *menu = dynamic_cast<PKMenu*>(rootMenu);

        if(menu)
        {
#ifdef LINUX
            menu->setTopLevel(true);
#endif
			menu->build();
            return menu;
        }
    }

    return NULL;
}

//
// From PKControl
//

uint32_t PKMenu::getMinimumWidth()
{
    return 0;
}

uint32_t PKMenu::getMinimumHeight()
{
    return 0;
}

//
// Menu management
//

void PKMenu::insertItem(std::string id,
						std::wstring text,
						std::wstring accel)
{
	PKMenuItem *newItem = new PKMenuItem();
	PKMenu::lastId++;

	newItem->setId(PKMenu::lastId);
	newItem->setMenu(this);

	PKVariantString idv(id);
	newItem->set(PKMenuItem::ID_STRING_PROPERTY, &idv);

	PKVariantWString tv(text);
	newItem->set(PKMenuItem::TEXT_WSTRING_PROPERTY, &tv);

	PKVariantWString av(accel);
	newItem->set(PKMenuItem::SHORTCUT_STRING_PROPERTY, &av);

	this->addChild(newItem);

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
	info.wID        = PKMenu::lastId++;
	info.dwItemData = (ULONG_PTR) newItem;
	info.fType      = MFT_STRING;
	info.dwTypeData = (TCHAR *) title.c_str();
	info.cch        = title.size();

	InsertMenuItem(this->handle, -1, FALSE, &info);

#endif
    
#ifdef LINUX

    if(this->handle == NULL)
	{
    	return;
	}
	
	std::string textUTF8 = PKStr::wStringToUTF8string(text);
	
	GtkWidget *gitem = NULL;
    gitem = gtk_menu_item_new_with_label(textUTF8.c_str());
	
    if(gitem != NULL)
	{
        this->items.push_back(gitem);
        this->ids.push_back(PKMenu::lastId);

        if(accel != L"")
        {
			std::string a = PKStr::wStringToString(accel);

            GdkModifierType mod;
            guint key;
            
            PKUIParseAccelerator(a, &mod, &key);
        
            gtk_widget_add_accelerator(gitem, "activate", 
                                       PKMenu::accelGroup,
                                       key, mod, GTK_ACCEL_VISIBLE);
        }

        gtk_menu_shell_append(GTK_MENU_SHELL(this->handle), gitem);

        g_signal_connect(G_OBJECT(gitem), "activate",
                         G_CALLBACK (PKMenu::gtk_menu_item_activated), this);      

    	gtk_widget_show(gitem);
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
		AppendMenuItemTextWithCFString(this->handle, cfText,  0, 0, &index);
		newItem->setId((int32_t)index);
		newItem->setMenu(this);
		
		if(accel != L"")
		{
			std::string a = PKStr::wStringToString(accel);
			
			UInt8  mod;
			UInt16 key;
			
			PKUIParseAccelerator(a, &mod, &key);
			
			SetMenuItemCommandKey(this->handle, index, false, key);
			SetMenuItemModifiers(this->handle, index, mod);
		}
		
		uint32_t data = (uint32_t) (void *) newItem;
		
		SetMenuItemProperty(this->handle, index, 
							kPKMenuData, kPKItemHandle, 
							sizeof(data), &data);
        
        delete [] ptr;
        CFRelease(cfText);
    }
    
#endif	
}

void PKMenu::insertSeparator()
{
#ifdef WIN32

	MENUITEMINFO info;
	memset(&info, 0, sizeof(MENUITEMINFO));

	info.cbSize     = sizeof(MENUITEMINFO);
	info.fMask      = MIIM_TYPE;
	info.fType      = MFT_SEPARATOR;

	InsertMenuItem(this->handle, -1, FALSE, &info);


#endif
    
#ifdef LINUX

    if(this->handle == NULL)
	{
    	return;
	}
		
	GtkWidget *item = gtk_separator_menu_item_new();

    if(item != NULL)
	{
    	gtk_widget_show(item);
        gtk_menu_shell_append(GTK_MENU_SHELL(this->handle), item);
	}

#endif
    
#ifdef MACOSX
    
    AppendMenuItemTextWithCFString(this->handle,
                                   NULL,
                                   kMenuItemAttrSeparator,
                                   0,
                                   NULL);
    
#endif	
}

void PKMenu::clear()
{
#ifdef WIN32

	int count = GetMenuItemCount(this->handle);

	for(int i=0; i < count; i++)
	{
		DeleteMenu(this->handle, 0, MF_BYPOSITION);
	}

#endif

#ifdef LINUX
    // Can't be done    
#endif

#ifdef MACOSX
    
	if(this->handle != NULL)
	{
		UInt16 count = CountMenuItems(this->handle);
		DeleteMenuItems(this->handle, 1, count);
    }
	
#endif
}


//
// Item management
//

void PKMenu::setItemEnabled(std::string id, bool enabled)
{
    if(this->handle == NULL)
    {
        return;
    }

    PKVariant *idValue = PKVariantString_Create(id);

    PKObject *child = this->findChildWithPropertyValue(PKMenuItem::ID_STRING_PROPERTY, 
                                                       idValue);

    if(child)
    {
        PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);

        if(item)
        {
            int32_t id = item->getId();

#ifdef WIN32
            EnableMenuItem(this->handle, (UINT) id, enabled ? MF_ENABLED : (MF_DISABLED | MF_GRAYED));
#endif
#ifdef MACOSX
			if(enabled) EnableMenuItem(this->handle, id);
			else DisableMenuItem(this->handle, id);
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
            // And update the item silently

            PKPropertyBag *props = item->getProperties();
            PKVariant *value = PKVariantBool_Create(enabled);

            props->setNoCallback(PKMenuItem::ENABLED_BOOL_PROPERTY, value);
        }
    }

    delete idValue;
}

void PKMenu::setItemChecked(std::string id, bool checked)
{
    if(this->handle == NULL)
    {
        return;
    }

    PKVariant *idValue = PKVariantString_Create(id);

    PKObject *child = this->findChildWithPropertyValue(PKMenuItem::ID_STRING_PROPERTY, 
                                                       idValue);

    if(child)
    {
        PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);

        if(item)
        {
            int32_t id = item->getId();

#ifdef WIN32
            CheckMenuItem(this->handle, (UINT) id, checked ? MF_CHECKED : MF_UNCHECKED);
#endif
#ifdef MACOSX
			if(checked) SetItemMark(this->handle, id, 0x12);
			else SetItemMark(this->handle, id, 0x00);
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
            // And update the item silently

            PKPropertyBag *props = item->getProperties();
            PKVariant *value = PKVariantBool_Create(checked);

            props->setNoCallback(PKMenuItem::CHECKED_BOOL_PROPERTY, value);
        }
    }

    delete idValue;
}

void PKMenu::updateItemText(std::string id, std::wstring text)
{
    if(this->handle == NULL)
    {
        return;
    }

    PKVariant *idValue = PKVariantString_Create(id);

    PKObject *child = this->findChildWithPropertyValue(PKMenuItem::ID_STRING_PROPERTY, 
                                                       idValue);

    if(child)
    {
        PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);

        if(item)
        {
			PKVariantWString _text(text);
			item->set(PKMenuItem::TEXT_WSTRING_PROPERTY, &_text);
			this->updateItemText(id);
		}
	}
}


void PKMenu::updateItemText(std::string id)
{
    if(this->handle == NULL)
    {
        return;
    }

    PKVariant *idValue = PKVariantString_Create(id);

    PKObject *child = this->findChildWithPropertyValue(PKMenuItem::ID_STRING_PROPERTY, 
                                                       idValue);

    if(child)
    {
        PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);

        if(item)
        {
            PKVariant *t = item->get(PKMenuItem::TEXT_WSTRING_PROPERTY);
            std::wstring text = PKVALUE_WSTRING(t);

            int32_t id = item->getId();

#ifdef WIN32

            PKVariant *s = item->get(PKMenuItem::SHORTCUT_STRING_PROPERTY);
            std::string accel = PKVALUE_STRING(s);

            PKVariant *r = item->get(PKMenuItem::RIGHT_BOOL_PROPERTY);
            bool right = PKVALUE_BOOL(r);

            PKVariant *i = item->get(PKMenuItem::ICON_WSTRING_PROPERTY);
            std::wstring icon = PKVALUE_WSTRING(i);

            MENUITEMINFO info;
	        memset(&info, 0, sizeof(MENUITEMINFO));

	        std::wstring title = text;

	        if(accel != "")
	        {
		        title += L"\t";
                title += PKStr::stringToWString(accel);
	        }

            // Check if the item has a submenu

            HMENU submenuHandle = NULL;

            if(item->getChildrenCount() == 1)
            {
                PKMenu *submenu = dynamic_cast<PKMenu*>(item->getChildAt(0));

                if(submenu)
                {
                    submenuHandle = submenu->getMenuHandle();
                }
            }

            info.cbSize     = sizeof(MENUITEMINFO);
	        info.fMask      = MIIM_DATA | MIIM_ID | MIIM_BITMAP | MIIM_STRING;
            info.wID        = item->getId();
	        info.dwItemData = (ULONG_PTR) (LPVOID) item;
	        info.fType      = MFT_STRING;
	        info.dwTypeData = (TCHAR *) title.c_str();
	        info.cch        = title.size();

            if(icon != L"")
            {
				PKImage *bitmap = NULL;
			
				PKImageURIType type;
				std::wstring   image;
			
				PKImage::processURI(icon, &type, &image);
			
				if(type == PKIMAGE_URI_RESOURCE)
				{
					bitmap = PKImageLoader::loadBitmapFromResource(image);
				}
			
				if(type == PKIMAGE_URI_FILENAME)
				{
					bitmap = PKImageLoader::loadBitmapFromFilename(image);
				}
			
				if(bitmap != NULL)
				{
                    if(PKOS::isVistaOrAbove())
                    {
                        // We just need an ARGB bitmap

                        int32_t w = bitmap->getWidth();
                        int32_t h = bitmap->getHeight();

	                    HBITMAP bmp  = NULL;
                        void   *bits = NULL;

	                    BITMAPINFO dibInfo;
	                    memset(&dibInfo, 0, sizeof(BITMAPINFO));

	                    dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
	                    dibInfo.bmiHeader.biWidth         =  w;
                        dibInfo.bmiHeader.biHeight        =  -h;
	                    dibInfo.bmiHeader.biBitCount      =  32;
	                    dibInfo.bmiHeader.biClrUsed       =  0;
	                    dibInfo.bmiHeader.biClrImportant  =  0;
	                    dibInfo.bmiHeader.biCompression   =  BI_RGB;
	                    dibInfo.bmiHeader.biPlanes        =  1;
	                    dibInfo.bmiHeader.biSizeImage     =  w*h*4;
	                    dibInfo.bmiHeader.biXPelsPerMeter =  0;
	                    dibInfo.bmiHeader.biYPelsPerMeter =  0;

	                    bmp = CreateDIBSection(NULL, &dibInfo, DIB_RGB_COLORS, &bits, NULL, 0);

                        if(bmp == NULL)
                        {
							delete bitmap;
                            return;
                        }

						// Alpha premultiply

						uint8_t *pdata = (uint8_t *) bitmap->getPixels();

						for(unsigned int j=0; j <  bitmap->getHeight(); j++)
						{
							for(unsigned int i=0; i < bitmap->getWidth(); i++)
							{
								if(pdata[3] != 255)
								{
									pdata[0] = (uint8_t) ((int)pdata[0]*(int)pdata[3]/255);
									pdata[1] = (uint8_t) ((int)pdata[1]*(int)pdata[3]/255);
									pdata[2] = (uint8_t) ((int)pdata[2]*(int)pdata[3]/255);
								}
							
								pdata +=4;
							}
						}

                        // Copy pixels
                        memcpy(bits, bitmap->getPixels(), w*h*4);
						delete bitmap;

                        // Assign
                        info.hbmpItem = bmp;
                    }
                    else
                    {
                        // 2000, XP
                        // Nothing to do, item already has its icon
						info.hbmpItem = HBMMENU_CALLBACK;
                    }
                }
            }

            if(right)
            {
			    info.fMask = MIIM_DATA | MIIM_ID | MIIM_TYPE;
	            info.fType |= MFT_RIGHTJUSTIFY;
            }

            if(submenuHandle)
            {
                info.fMask    |= MIIM_SUBMENU;
                info.hSubMenu  = submenuHandle;
            }

            SetMenuItemInfo(this->handle, id, FALSE, &info);
            
#endif

#ifdef LINUX

            for(uint32_t i=0; i<this->ids.size(); i++)
            {
                if(this->ids[i] == id)
                {
                	std::string textUTF8 = PKStr::wStringToUTF8string(text);
    		        textUTF8 = PKStr::replaceString("&", "_", textUTF8);
                    gtk_menu_item_set_label(GTK_MENU_ITEM(this->items[i]), textUTF8.c_str());   
                    break;
                }                   
            }           

#endif

#ifdef MACOSX

			CFStringRef cfText = NULL;
			
			uint32_t size = 0;
			UniChar *ptr  = NULL;
			
			PKStr::wStringToUniChar(text, &size, &ptr);
			
			if(ptr != NULL)
			{
				cfText = CFStringCreateWithCharacters(NULL, ptr, size);
			
				if(item->getChildrenCount() == 1)
				{
					// It has a submenu, hence it is a "menu" item
				
					PKMenu *submenu = dynamic_cast<PKMenu*>(item->getChildAt(0));
					
					if(submenu)
					{
						MenuRef submenuHandle = submenu->getMenuHandle();
						
						// Set title
						
						SetMenuTitleWithCFString(submenuHandle, cfText);
					}						
				}
				else
				{
					SetMenuItemTextWithCFString(this->handle, id,
												cfText);
				}
				
				delete [] ptr;
				CFRelease(cfText);
			}
			
#endif
        }
    }

    delete idValue;
}

//
// i18n
//

void PKMenuChildrenWalkI18N(PKObject *child, void *context)
{
    if(child != NULL)
    {
        PKPropertyBag *props = child->getProperties();
		
        if(props != NULL)
        {
            uint32_t count = props->getNumberOfProperties();
			
            for(uint32_t i=0; i < count; i++)
            {
                PKProperty *p = props->getPropertyAt(i);
				
                if(p != NULL)
                {
                    PKVariant *value = p->getValue();
					
                    if(value->_type == PK_VARIANT_WSTRING)
                    {
                        PKVariantWStringI18N *i18n = dynamic_cast<PKVariantWStringI18N *>(value);
						
                        if(i18n)
                        {
                            PKVariantWStringI18N *newVal = new PKVariantWStringI18N(i18n->_original);
                            newVal->_val = PK_i18n(i18n->_original);
                            p->setValue(newVal);
                            delete newVal;
                        }
                    }
                }
            }
        }
    }
}

void PKMenu::updateUIStrings()
{
	this->walkChildren(PKMenuChildrenWalkI18N, NULL);
}

PKMenu *PKMenu::findSubmenu(std::string id)
{
    for(uint32_t i=0; i<this->submenus.size(); i++)
    {
        PKMenu *found = this->submenus[i]->findSubmenu(id);

        if(found)
        {
            return found;
        }

        PKVariant  *idv = this->submenus[i]->get(PKControl::ID_STRING_PROPERTY);
        std::string sid = PKVALUE_STRING(idv);

        if(sid == id)
        {
            return this->submenus[i];
        }
    }

    return NULL;
}

PKMenu *PKMenu::findSubmenuByHandle(void *h)
{
	if((void *) this->handle == h)
	{
		return this;
	}

    for(uint32_t i=0; i<this->submenus.size(); i++)
    {
        PKMenu *found = this->submenus[i]->findSubmenuByHandle(h);

        if(found)
        {
            return found;
        }
	}

	return NULL;
}

//
// Linux signal
//

#ifdef LINUX

GtkAccelGroup *PKMenu::getGtkAccelGroup()
{
    return PKMenu::accelGroup;
}          

void PKMenu::gtk_menu_item_activated(GtkMenuItem *menuitem,
                                     gpointer     user_data)
{
    PKMenu *menu = (PKMenu *) user_data;
    
    if(menu == NULL)
    {
        return;
    }          
    
    menu->linuxItemActivated(menuitem);                               
}                                     

void PKMenu::linuxItemActivated(GtkMenuItem *item)
{
    if(item == NULL)
    {
        return;
    }          

    for(unsigned int i=0; i<this->items.size(); i++)
    {
        if(this->items[i] == (GtkWidget *) item)
        {
            int id = this->ids[i];

            for(unsigned int n=0; n<this->getChildrenCount(); n++)
            {
                PKMenuItem *item = dynamic_cast<PKMenuItem *>(this->getChildAt(n));                
       
                if(item)
                {   
                    if((item->getId() == id) && (this->window != NULL))
                    {   
                        // Check for the EDIT menu
    
                        PKVariant *idv = item->get(PKMenuItem::ID_STRING_PROPERTY);
                        std::string idstr = PKVALUE_STRING(idv);

                        if(idstr == "MENU_ID_EDIT") // Special value
                        {
                            // We need to enable or disable items
                            // on the fly based on the clipboard and
                            // current focus control data

                            PKClipboard clip;

                            PKMenu *editMenu = dynamic_cast<PKMenu*>(item->getChildAt(0));

                            if(editMenu)
                            {
                                editMenu->setItemEnabled("MENU_ID_CUT",  false);
                                editMenu->setItemEnabled("MENU_ID_COPY", false);

                                editMenu->setItemEnabled("MENU_ID_PASTE", clip.containsText());

                                PKWindow *window = editMenu->window;

                                if(window)
                                {
                                    GtkWidget *widget = window->getWindowHandle();

                                    if(widget)
                                    {
                                        GtkWidget *focused = gtk_window_get_focus(GTK_WINDOW(widget));
        
                                        if(focused)
                                        {
                                    		PKControl *rootControl = dynamic_cast<PKControl*>(window->getChildAt(0));

                                            if(rootControl)
                                            {
                                                PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWidget(focused));

                                                if(focusControl)
                                                {
                                                    editMenu->setItemEnabled("MENU_ID_CUT",  focusControl->hasSelection());
                                                    editMenu->setItemEnabled("MENU_ID_COPY", focusControl->hasSelection());
                                                }
                                            }                
                                        }        
                                    }                                    
                                }                                
                            }                            
                        }                   
                    
                        // Callback in all cases

                        this->window->menuItemClicked(this, item, idstr);
                        return;
                    }                  
                }                  
            }                  
        
            break;
        }                  
    }                
}                                                           
                             
#endif

//
// Accelerators
//

#ifdef WIN32

void PKMenu::drawItem(WPARAM wParam, LPARAM lParam)
{
    DRAWITEMSTRUCT *draw_struct = (DRAWITEMSTRUCT *) lParam;

    if(draw_struct)
    {
        UINT menuID = draw_struct->itemID;

        PKMenuItem *item = (PKMenuItem*)(void*)draw_struct->itemData;

        if(item)
        {
            HICON icon = item->getHIcon();

            if(icon)
            {
                DrawIconEx(draw_struct->hDC, 4, 2, icon,
                           16, 16, 0, NULL, DI_NORMAL);
            }
        }
    }
}

void PKMenu::getAccelerators(std::vector<ACCEL *> *outArray)
{
    uint32_t count = this->getChildrenCount();

    for(uint32_t i = 0; i < count; i++)
    {
        PKMenuItem *item = dynamic_cast<PKMenuItem*>(this->getChildAt(i));

        if(item)
        {
            PKVariant  *s = item->get(PKMenuItem::SHORTCUT_STRING_PROPERTY);
            std::string shortcut = PKVALUE_STRING(s);

            if(shortcut != "")
            {
                ACCEL *accel = new ACCEL;

                accel->cmd   = item->getId();

                BYTE modifier;
                WORD key;

                PKUIParseAccelerator(shortcut, &modifier, &key);

                accel->fVirt = modifier;
                accel->key   = key;

                outArray->push_back(accel);
            }

            if(item->getChildrenCount() == 1)
            {
                PKMenu *submenu = dynamic_cast<PKMenu*>(item->getChildAt(0));

                if(submenu)
                {
                    submenu->getAccelerators(outArray);
                }
            }
        }
    }
}

#endif
