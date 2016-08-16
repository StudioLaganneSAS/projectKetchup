//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKApp.h"

#ifdef MACOSX

// Declare our event handler

static PKMenu    *pkMenu      = NULL;
static MenuRef   rootMenu     = NULL;
static PKWindow *pkMainWindow = NULL;

static EventHandlerRef mHandler = NULL;
static OSStatus MacMenuCmdHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon);

#endif // MACOSX

#ifdef WIN32

HACCEL    pkGlobalAccelTable  = NULL;
PKWindow *pkMainWindow        = NULL;
HWND      pkActiveWindow      = NULL;

#endif // WIN32

PKApp::PKApp() 
{
    // Required Global Inits 

    PKBitmapOps::initialize();
	PKSafeEvents::initialize();

#ifdef WIN32
    InitCommonControls();
    INITCOMMONCONTROLSEX init;
    init.dwSize = sizeof(INITCOMMONCONTROLSEX);
    init.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&init);
#endif
};
    
PKApp::~PKApp() 
{
	PKSafeEvents::deinitialize();
};

// Utilities

void PKApp::quit()
{
#ifdef WIN32
	PostQuitMessage(0);
#endif
#ifdef LINUX
	gtk_main_quit();
#endif
#ifdef MACOSX
	if(mHandler)
	{
		RemoveEventHandler(mHandler);
	}
	
	QuitApplicationEventLoop();
#endif
}

void PKApp::installRootMenu(PKWindow *window, PKMenu *menu)
{
	if(window == NULL)
	{
		return;
	}
	
	if(menu == NULL)
	{
		return;
	}
	
#ifdef MACOSX
	
	// Record the root menu pointer & window
	
	pkMenu = menu;
	pkMainWindow = window;
	
	window->setMenu(menu);
	
	// Install the menu in the MenuBar
	
	rootMenu = menu->getMenuHandle();
	
	SetRootMenu(rootMenu);
	
	// See if we need to activate certain
	// standard menu items
	
	MenuRef       imenu;
	MenuItemIndex index;
	
	if(GetIndMenuItemWithCommandID(NULL, kHICommandPreferences, 
								   1, &imenu, &index) == noErr)
	{
		// Preferences
		
		PKVariant  *p   = menu->get(PKMenu::MAC_PREFS_MENUID_STRING_PROPERTY);
		std::string pid = PKVALUE_STRING(p);
		
		if(pid != "")
		{
			EnableMenuItem(imenu, index);
		}			
		
		// About
		
		PKVariant  *a   = menu->get(PKMenu::MAC_ABOUT_MENUID_STRING_PROPERTY);
		std::string aid = PKVALUE_STRING(a);
		
		PKVariant   *t   = menu->get(PKMenu::MAC_ABOUT_TEXT_WSTRING_PROPERTY);
		std::wstring txt = PKVALUE_WSTRING(t);
		
		if(aid != "")
		{
			CFStringRef cfTxt = PKStr::wStringToCFString(txt);
			InsertMenuItemTextWithCFString(imenu, cfTxt, 0, 0, kHICommandAbout);
			InsertMenuItemTextWithCFString(imenu, 0, 1, kMenuItemAttrSeparator, 0);
			CFRelease(cfTxt);
		}			
	}
	
	// Install our event handler
	
	EventTypeSpec kCmdEvents[ ]= { {kEventClassCommand, kEventProcessCommand},
		{kEventClassMenu, kEventMenuOpening} };
	
	// Try InstallEventHandler( GetEventMonitorTarget(), also
	
	InstallApplicationEventHandler(MacMenuCmdHandler, 
								   GetEventTypeCount(kCmdEvents), 
								   kCmdEvents, 0, &mHandler);
	
#endif
	
#ifdef WIN32
    
	// Record the main Window pointer
	
	pkMainWindow = window;
	
	// Set the menu to the window
	
	window->setMenu(menu);
	menu->setWindow(window); 
	
	// Create the accelerator table
	
	std::vector<ACCEL *> accelerators;
	menu->getAccelerators(&accelerators);
	
	uint32_t count = accelerators.size();
	
	ACCEL *accelTable = (ACCEL*) malloc(sizeof(ACCEL)*count);
	
	for(uint32_t i = 0; i < count; i++)
	{
		memcpy(&accelTable[i], accelerators[i], sizeof(ACCEL));
	}
	
	pkGlobalAccelTable = CreateAcceleratorTable(accelTable, count);
	
	free(accelTable);
	
	for(uint32_t i = 0; i < count; i++)
	{
		delete accelerators[i];
	}
	
#endif
	
#ifdef LINUX
	
	gtk_window_add_accel_group(GTK_WINDOW(window->getWindowHandle()), 
							   menu->getGtkAccelGroup());
	
	window->setMenu(menu);       
	menu->setWindow(window); 
	
#endif
}

void PKApp::updateUIStrings()
{
#ifdef MACOSX
	if(pkMenu)
	{
        // Update strings first
		pkMenu->updateUIStrings();
        
        // Now update special items

        MenuRef       imenu;
        MenuItemIndex index;
        
        if(GetIndMenuItemWithCommandID(NULL, kHICommandAbout, 
                                       1, &imenu, &index) == noErr)
        {
            PKVariant   *a   = pkMenu->get(PKMenu::MAC_ABOUT_TEXT_WSTRING_PROPERTY);
            std::wstring txt = PKVALUE_WSTRING(a);
            
            CFStringRef cfTxt = PKStr::wStringToCFString(txt);
            SetMenuItemTextWithCFString(imenu, index, cfTxt);
            CFRelease(cfTxt);
        }        
        
	}
#endif
}
	
//
// Main PKApp
//

static PKApp *app = NULL;

#ifdef MACOSX

//
// Mac Menu event handler
//

static OSStatus MacMenuCmdHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
{
    UInt32   eclass;
    
    eclass = GetEventClass(inEvent);

    if(eclass == kEventClassMenu) 
    {
		switch(GetEventKind(inEvent))
		{
			case kEventMenuOpening:
			{
				// Menu is openning, detect the edit menu
				
				MenuRef menu = NULL;
				
				GetEventParameter(inEvent,
								  kEventParamDirectObject,
								  typeMenuRef,NULL,
								  sizeof(MenuRef),NULL,
								  &menu);
				
				if(menu == NULL)
				{
					return eventNotHandledErr;
				}
				
				if(pkMainWindow != NULL && pkMenu != NULL)
				{
					// Run the callback for the menu items of this menu
					
					PKMenu *sub_menu = pkMenu->findSubmenuByHandle((void*) menu);
					
					if(sub_menu)
					{
						uint32_t count = sub_menu->getChildrenCount();
						
						for(uint32_t i=0; i < count; i++)
						{
							PKObject *child = sub_menu->getChildAt(i);
							
							PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);
							
							if(item)
							{
								std::string id = PKVALUE_STRING(item->get(PKMenuItem::ID_STRING_PROPERTY));
								PKDialog *dialog = dynamic_cast<PKDialog *> (pkMainWindow);
								
								if(dialog != NULL)
								{
									dialog->menuItemAboutToBeShown(sub_menu, item, id);
								}
							}
						}
					}
					
					// Handle cut/copy/paste & undo/redo
					
					PKMenu *editMenu = pkMenu->findSubmenu("MENU_ID_EDIT");
					
					if(editMenu && (editMenu->getMenuHandle() == menu))
					{
						// let all edit controls know that
						// they have to commit their changes
						// (for undo/redo)
						
						PKDialog *dialog = dynamic_cast<PKDialog *> (pkMainWindow);
						
						if(dialog != NULL)
						{
							dialog->commitEditControlChanges();
						}
						
						// cut/copy/paste
						
						editMenu->setItemEnabled("MENU_ID_CUT",  false);
						editMenu->setItemEnabled("MENU_ID_COPY", false);
						
						PKClipboard clip;
						editMenu->setItemEnabled("MENU_ID_PASTE", clip.containsText());
						
						ControlRef outControl = NULL;
						GetKeyboardFocus(GetWindowFromPort(pkMainWindow->getWindowHandle()), &outControl);	
						
						if(outControl)
						{
							PKControl *rootControl = dynamic_cast<PKControl*>(pkMainWindow->getChildAt(0));
							
							if(rootControl)
							{
								PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByControlRef(outControl));
								
								if(focusControl)
								{
									if(focusControl->hasSelection())
									{
										editMenu->setItemEnabled("MENU_ID_CUT",  true);
										editMenu->setItemEnabled("MENU_ID_COPY", true);
									}
								}
							}
						}
						
						// undo/redo
						
						if(PKUndoManager::getInstance()->canUndo())
						{
							std::wstring wtext = PK_i18n(L"Undo \"%1\"");
							wtext = PKStr::replaceString(L"%1", PKUndoManager::getInstance()->getUndoCommandDisplayName(), wtext);
							
							editMenu->updateItemText("MENU_ID_UNDO", wtext);
							editMenu->setItemEnabled("MENU_ID_UNDO", true);
						}
						else
						{
							editMenu->updateItemText("MENU_ID_UNDO", PK_i18n(L"Undo"));
							editMenu->setItemEnabled("MENU_ID_UNDO", false);
						}
						
						if(PKUndoManager::getInstance()->canRedo())
						{
							std::wstring wtext = PK_i18n(L"Redo \"%1\"");
							wtext = PKStr::replaceString(L"%1", PKUndoManager::getInstance()->getRedoCommandDisplayName(), wtext);
							
							editMenu->updateItemText("MENU_ID_REDO", wtext);
							editMenu->setItemEnabled("MENU_ID_REDO", true);
						}
						else
						{
							editMenu->updateItemText("MENU_ID_REDO", PK_i18n(L"Redo"));
							editMenu->setItemEnabled("MENU_ID_REDO", false);
						}						

						return noErr;
					}
				}	
				
				return eventNotHandledErr;
			}
			break;
				
		}
	}
	
    
    if(eclass == kEventClassCommand) 
    {
		switch(GetEventKind(inEvent))
		{
			case kEventProcessCommand:
			{
				HICommand     hiCommand;
				MenuID        menuID;
				MenuItemIndex menuItem;

				GetEventParameter(inEvent,
								  kEventParamDirectObject,
								  typeHICommand,NULL,
								  sizeof(HICommand),NULL,
								  &hiCommand);
				
				// Check commandID for About, Prefs, Quit
				
				if(hiCommand.commandID == kHICommandAbout)
				{
					if(pkMainWindow != NULL && pkMenu != NULL)
					{
						PKVariant   *p = pkMenu->get(PKMenu::MAC_ABOUT_MENUID_STRING_PROPERTY);
						std::string id = PKVALUE_STRING(p);
						
						pkMainWindow->menuItemClicked(pkMenu, NULL, id);
					}

					return noErr;
				}
				else if(hiCommand.commandID == kHICommandPreferences)
				{
					if(pkMainWindow != NULL && pkMenu != NULL)
					{
						PKVariant   *p = pkMenu->get(PKMenu::MAC_PREFS_MENUID_STRING_PROPERTY);
						std::string id = PKVALUE_STRING(p);
						
						pkMainWindow->menuItemClicked(pkMenu, NULL, id);
					}

					return noErr;
				}
				else if(hiCommand.commandID == kHICommandQuit)
				{
					if(pkMainWindow != NULL && pkMenu != NULL)
					{
						PKVariant   *p = pkMenu->get(PKMenu::MAC_QUIT_MENUID_STRING_PROPERTY);
						std::string id = PKVALUE_STRING(p);

						pkMainWindow->menuItemClicked(pkMenu, NULL, id);
					}
					
					return noErr;
				}
				else
				{
					menuID = GetMenuID(hiCommand.menu.menuRef);
					
					menuItem = hiCommand.menu.menuItemIndex;
					
					uint32_t  item;
					ByteCount outSize;
					
					OSStatus result = GetMenuItemProperty(hiCommand.menu.menuRef,
														  menuItem,
														  kPKMenuData,
														  kPKItemHandle,
														  sizeof(item),
														  &outSize,
														  &item);
					
					if(result == noErr)
					{
						PKMenuItem *mitem = (PKMenuItem *) item;
						
						PKVariant *i = mitem->get(PKMenuItem::ID_STRING_PROPERTY);
						std::string id = PKVALUE_STRING(i);
						
						if(pkMainWindow != NULL)
						{
							PKVariant   *i = mitem->get(PKMenuItem::ID_STRING_PROPERTY);
							std::string id = PKVALUE_STRING(i);
							
							if(id== "ID_RECENT_FILES_CLEAR")
							{
								if(pkMenu)
								{
									pkMenu->clearRecentFiles();
								}
							}
							else 
							{
								pkMainWindow->menuItemClicked(mitem->getMenu(), mitem, id);
							}
							
						}
					}			
					else
					{
						return eventNotHandledErr;
					}
				}				
				
				return noErr;
			}
			break;
		}
	}		
/*	
	if(pkMainWindow == NULL)
	{
		return eventNotHandledErr;
	}
	
	// See if the current focus 
	// is a custom control, if so
	// we stop the event chain here
	
	ControlRef outControl = NULL;
	CGrafPtr   window = pkMainWindow->getWindowHandle();
	GetKeyboardFocus(GetWindowFromPort(window), &outControl);	
	
	if(outControl)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(pkMainWindow->getChildAt(0));
		
        if(rootControl)
        {
            PKCustomControl *focusControl = dynamic_cast<PKCustomControl*>(rootControl->findObjectByControlRef(outControl));
			
            if(focusControl)
            {
				return noErr;
            }
        }
	}
*/	
	return eventNotHandledErr;
}

#endif

//
// WIN32 WinMain
//

#ifdef WIN32

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
    MSG msg;
    BOOL bRet; 

    std::string  cmdLine  = lpCmdLine;
    std::wstring wCmdLine = PKStr::stringToWString(cmdLine);

	app = new PKApp();
	
    int32_t result = app->appStartupDelegate(wCmdLine);

    if(result != 0)
    {
        // Don't start
        return result;
    }

    // Now start the message loop

   // Start the message loop. 
 
    while((bRet = GetMessage(&msg, NULL, 0, 0 )) != 0)
    { 
        if(bRet == -1)
        {
            // handle the error and possibly exit ?
        }
        else
        {
			if(pkGlobalAccelTable != NULL && 
			   pkMainWindow       != NULL)
            {
				if (!TranslateAccelerator( 
					pkMainWindow->getWindowHandle(), // handle to receiving window 
					pkGlobalAccelTable,              // handle to active accelerator table 
					&msg))                           // message data 
				{
                    // We need (?) to call both 
                    // in order for TAB to work AND
                    // to receive WM_CHAR ??

                    if(pkActiveWindow == NULL ||
                      !IsDialogMessage(pkActiveWindow, &msg))
                    {
    		    		TranslateMessage(&msg); 
	    		    	DispatchMessage(&msg); 
                    }
				}
			}
            else
            {
	    		TranslateMessage(&msg); 
		    	DispatchMessage(&msg); 
            }
        }
    } 

	PKUndoManager::cleanup();
     
    result = app->appShutdownDelegate();
	delete app;
	
    return result;
}

#endif // WIN32

//
// LINUX event loop
//

#ifdef LINUX

int main(int argc, char **argv)
{
    std::wstring commandLine = L"";

    if(argc > 1)
    {
 	for(int i=1; i<argc; i++)
	{
		std::string str = argv[i];
		commandLine += (PKStr::stringToWString(str) + L" ");
	}
    }

    // GTK init

    gtk_init(&argc, &argv);
	
	app = new PKApp();
	
    // App delegate

    int32_t result = app->appStartupDelegate(commandLine);


    if(result != 0)
    {
        // Don't start
        return result;
    }

    // Gtk+ Event Loop

    gtk_main();

    // Loop is done, cleanup

	PKUndoManager::cleanup();

    result = app->appShutdownDelegate();
	delete app;

    return result;
}


#endif // LINUX

//
// MacOSX event loop
//

#ifdef MACOSX

int main(int argc, char **argv) 
{
    std::wstring commandLine = L"";

    if(argc > 1)
    {
 	    for(int i=1; i<argc; i++)
	    {
		    std::string str = argv[i];
		    commandLine += (PKStr::stringToWString(str) + L" ");
	    }
    }

	app = new PKApp();
	
    int32_t result = app->appStartupDelegate(commandLine);

    if(result != 0)
    {
        // Don't start
        return result;
    }

    RunApplicationEventLoop();

	// Loop is done, cleanup

	PKUndoManager::cleanup();

	result = app->appShutdownDelegate();
	delete app;
	
    return result;
}

#endif // MACOSX

