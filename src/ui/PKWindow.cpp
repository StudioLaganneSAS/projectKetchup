//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKWindow.h"

// Utils

#include "PKCursorTools.h"
#include "PKStr.h"
#include "PKTime.h"
#include "PKBlitter.h"
#include "PKMenuItem.h"
#include "PKScreenTool.h"
#include "PKScrollView.h"
#include "PKVLayout.h"

#define CUSTOM_MESSAGE_DRAW32BPPBITMAP 0

// Local Stuff

#ifdef WIN32

#pragma warning(disable : 4800)

#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wparam) ((short)HIWORD (wparam))
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#define LWA_COLORKEY  0x00000001
#define LWA_ALPHA     0x00000002

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 1
#endif

#define ULW_ALPHA 0x00000002
#endif

#define WM_HIDECURSOR (WM_USER+1)
#define WM_PK_CUSTOM  (WM_USER+2)

// WIN32 Window Procedure

LRESULT CALLBACK PKWindow::PKWindowProc(HWND hwnd,
					 					UINT uMsg,
										WPARAM wParam,
										LPARAM lParam)
{
	PKWindow *window = NULL;
	window = (PKWindow *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if(window == NULL)
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

	PKCritLock lock(&window->eventMutex);

	if((uMsg >= WM_PK_CUSTOM) &&
	   (uMsg <= WM_PK_CUSTOM + 100))
	{
		window->receiveCustomMessage(uMsg - WM_PK_CUSTOM);
	
		return 0;
	}

	switch(uMsg)
	{
	case WM_CREATE:
		{
		}
		break;

	// Drag & Drop

	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP) wParam;

			if(hDrop)
			{
				std::vector<std::wstring> files;

				// Get the number of files
				UINT fileNum = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

				for(uint32_t i=0; i<fileNum; i++)
				{
					// Get the required buffer size
					UINT size = DragQueryFile(hDrop, i, NULL, 0);

					// Allocate a buffer for the string
					wchar_t *buffer = new wchar_t[size+1];

					if(buffer != NULL)
					{
						memset(buffer, 0, size+1);
						DragQueryFile(hDrop, 0, buffer, size+1);

						std::wstring file = buffer;
						files.push_back(file);
					}
				}

				window->filesDropped(files);

				DragFinish(hDrop);
			}
		}
		break;

	// Misc

	case WM_COMMAND:
		{
			uint32_t msg = HIWORD(wParam);
			uint32_t id  = LOWORD(wParam);

            if(msg == 0 || msg == 1) // MENU ITEM or ACCEL
            {
                if(window->menu)
                {
                    MENUITEMINFO info;
                    memset(&info, 0, sizeof(info));
                    info.cbSize = sizeof(info);
                    info.fMask  = MIIM_ID | MIIM_DATA;

                    HMENU handle = window->menu->getMenuHandle();

                    if(GetMenuItemInfo(handle, id, FALSE, &info))
                    {
                        PKMenuItem *item = (PKMenuItem *) info.dwItemData;

                        if(item)
                        {
                            PKVariant *i = item->get(PKMenuItem::ID_STRING_PROPERTY);
                            std::string id = PKVALUE_STRING(i);

							if(id == "ID_RECENT_FILES_CLEAR")
							{
								window->menu->clearRecentFiles();
							}
							else
							{
	                            window->menuItemClicked(item->getMenu(), item, id);
							}
                        }
                    }
                }
            }

            return 0;
        }
        break;

    // Windowing

	case WM_MOVE:
		{
			window->windowMoved();
		}
		break;

	case WM_SIZE:
		{
			window->windowResized();

            if(wParam == SIZE_MINIMIZED)
            {
                window->windowMinimized();
            }

            if(wParam == SIZE_MAXIMIZED)
            {
                window->windowMaximized();
            }
        }
		break;

	case WM_SIZING:
		{
			PKRect rect;
			LPRECT  dragRect = (LPRECT) lParam;

			rect.x =  dragRect->left;
			rect.y =  dragRect->top;
			rect.w = (dragRect->right  - dragRect->left);
			rect.h = (dragRect->bottom - dragRect->top);

			PKDirection dir;

			switch(wParam)
			{
			case WMSZ_LEFT:
				{
					dir = PK_DIR_LEFT;
				}
				break;

			case WMSZ_RIGHT:
				{
					dir = PK_DIR_RIGHT;
				}
				break;

			case WMSZ_TOP:
				{
					dir = PK_DIR_TOP;
				}
				break;

			case WMSZ_BOTTOM:
				{
					dir = PK_DIR_BOTTOM;
				}
				break;

			case WMSZ_TOPLEFT:
				{
					dir = PK_DIR_TOPLEFT;
				}
				break;

			case WMSZ_TOPRIGHT:
				{
					dir = PK_DIR_TOPRIGHT;
				}
				break;

			case WMSZ_BOTTOMRIGHT:
				{
					dir = PK_DIR_BOTTOMRIGHT;
				}
				break;

			case WMSZ_BOTTOMLEFT:
			default:
				{
					dir = PK_DIR_BOTTOMLEFT;
				}
				break;
			}

			window->windowResizing(dir, &rect);

			dragRect->left   = rect.x;
			dragRect->top    = rect.y;
			dragRect->right  = rect.x + rect.w;
			dragRect->bottom = rect.y + rect.h;

			return 0;
		}
		break;

	case WM_GETMINMAXINFO:
		{
			MINMAXINFO *info = (MINMAXINFO *) lParam;

			window->windowGetMinMax((uint32_t *) &info->ptMinTrackSize.x,
									(uint32_t *) &info->ptMinTrackSize.y,
									(uint32_t *) &info->ptMaxTrackSize.x,
									(uint32_t *) &info->ptMaxTrackSize.y);

			return 0;
		}
		break;

	case WM_ACTIVATE:
		{
			window->windowActivated(wParam != WA_INACTIVE);
		}
		break;

	case WM_DISPLAYCHANGE:
		{
			window->displayChange();
		}
		break;

	case WM_CLOSE:
		{
			window->windowClosed();
			return 0;
		}
		break;

	case WM_SHOWWINDOW:
		{
			if(wParam == TRUE)
			{
				// Draw the window first
				// So it doesn't flicker 
				// when it is shown next

				PAINTSTRUCT lp;
				BeginPaint(hwnd, &lp);

				PKRect rect;

				rect.x =  lp.rcPaint.left;
				rect.y =  lp.rcPaint.top;
				rect.w = (lp.rcPaint.right - lp.rcPaint.left);
				rect.h = (lp.rcPaint.bottom - lp.rcPaint.top);

				window->draw(lp.hdc, rect);
				
				EndPaint(hwnd, &lp);

				window->windowShowing();
			}
			else
			{
				window->windowHiding();
			}
		}
		break;

	// Drawing

	case WM_PAINT:
		{
			PAINTSTRUCT lp;
			BeginPaint(hwnd, &lp);

			PKRect rect;

			rect.x =  lp.rcPaint.left;
			rect.y =  lp.rcPaint.top;
			rect.w = (lp.rcPaint.right - lp.rcPaint.left);
			rect.h = (lp.rcPaint.bottom - lp.rcPaint.top);

			window->draw(lp.hdc, rect);
			
			EndPaint(hwnd, &lp);

			return 0;
		}
		break;

	// Keyboard

	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			if(wParam == VK_SHIFT   ||
			   wParam == VK_CONTROL)
			{
				return 0;
			}

			unsigned int modifier  = PK_MOD_NONE;
			PKKey key = PK_KEY_UNKNOWN;
			unsigned int vcode = 0;

			PKUIComputeKeyState(wParam, &modifier, &key, &vcode);

			if(uMsg == WM_KEYDOWN)
			{
				window->keyDown(modifier, key, vcode);
			}

			if(uMsg == WM_KEYUP)
			{
				window->keyUp(modifier, key, vcode);
			}

			return 0;
		}
		break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		{
			if(wParam == VK_SHIFT   ||
			   wParam == VK_CONTROL ||
			   wParam == VK_MENU)
			{
				return 0;
			}

			unsigned int modifier  = PK_MOD_NONE;
			PKKey key = PK_KEY_UNKNOWN;
			unsigned int vcode = 0;

			PKUIComputeKeyState(wParam, &modifier, &key, &vcode);
			modifier |= PK_MOD_ALT;

			if(uMsg == WM_SYSKEYDOWN)
			{
				window->keyDown(modifier, key, vcode);
			}

			if(uMsg == WM_SYSKEYUP)
			{
				window->keyUp(modifier, key, vcode);
			}

			return 0;
		}
		break;

	// Mouse

	case WM_LBUTTONDOWN:
		{
			int x;
			int y;
			unsigned int mod;

			SetCapture(hwnd);

			window->getMouseParams(wParam, lParam, &x, &y, &mod);
			window->mouseDown(PK_BUTTON_LEFT, x, y, mod);
		}
		break;

	case WM_RBUTTONDOWN:
		{
			int x;
			int y;
			unsigned int mod;

			window->getMouseParams(wParam, lParam, &x, &y, &mod);
			window->mouseDown(PK_BUTTON_RIGHT, x, y, mod);
		}
		break;

	case WM_LBUTTONUP:
		{
			int x;
			int y;
			unsigned int mod;

			ReleaseCapture();

            window->getMouseParams(wParam, lParam, &x, &y, &mod);
			window->mouseUp(PK_BUTTON_LEFT, x, y, mod);
    	}
		break;

	case WM_RBUTTONUP:
		{
			int x;
			int y;
			unsigned int mod;

			window->getMouseParams(wParam, lParam, &x, &y, &mod);
			window->mouseUp(PK_BUTTON_RIGHT, x, y, mod);
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			int x;
			int y;
			unsigned int mod;

			window->getMouseParams(wParam, lParam, &x, &y, &mod);
			window->mouseDblClick(PK_BUTTON_LEFT, x, y, mod);
		}
		break;

	case WM_RBUTTONDBLCLK:
		{
			int x;
			int y;
			unsigned int mod;

			window->getMouseParams(wParam, lParam, &x, &y, &mod);
			window->mouseDblClick(PK_BUTTON_RIGHT, x, y, mod);
		}
		break;

	case WM_MOUSEMOVE:
		{
			int x;
			int y;
			unsigned int mod;

			window->getMouseParams(wParam, lParam, &x, &y, &mod);
			window->mouseMove(x, y, mod);

			TRACKMOUSEEVENT tme;

			tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;

			_TrackMouseEvent(&tme);
		}
		break;

	case WM_MOUSELEAVE:
		{
			window->mouseLeave();
		}
		break;

	case WM_MOUSEWHEEL:
		{
			int x;
			int y;
			unsigned int mod;

			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			window->getMouseParams(wParam, lParam, &x, &y, &mod);	

			if(zDelta > 0)
			{
				window->mouseWheel(PK_BUTTON_WHEEL_UP, x, y, mod);
			}
			else
			{
				window->mouseWheel(PK_BUTTON_WHEEL_DOWN, x, y, mod);
			}
		}
		break;

	case WM_HIDECURSOR:
		{
			PKCursorTools::hideCursor();
		}
		break;

	default:
		break;
	}

	// Default return
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void PKWindow::getMouseParams(WPARAM wParam, 
							   LPARAM lParam,
							   int *x,
							   int *y,
							   unsigned int *mod)
{
	if(x   == NULL || 
	   y   == NULL || 
	   mod == NULL)
	{
		return;
	}
    
	unsigned int m = PK_MOD_NONE;
    
	if((wParam & MK_CONTROL) == MK_CONTROL)
	{
		m |= PK_MOD_CONTROL;
	}
    
	if((wParam & MK_SHIFT) == MK_SHIFT)
	{
		m |= PK_MOD_SHIFT;
	}
    
	*mod = m;
    
	*x = LOWORD(lParam); 
	*y = HIWORD(lParam); 

	if(*x >= 32768)
	{
		*x = -1;
	}

	if(*y >= 32768)
	{
		*y = -1;
	}
}

#endif // WIN32

// MACOSX window procedure

#ifdef MACOSX

enum {
  
  kEventClassDWP        = FOUR_CHAR_CODE('DIVX'),
  kEventDWPCustom		= 1,
  kEventDWPMouse		= 2,
  
};

pascal OSStatus PKWindow::PKWindowEventHandler(EventHandlerCallRef  nextHandler,
                                      EventRef             inEvent, 
                                      void                *userData)
{
    PKWindow *window = (PKWindow *) userData;
        
	if(window == NULL)
	{
		return noErr;
	}
	
    UInt32 eventClass = GetEventClass(inEvent);
    UInt32 eventKind  = GetEventKind(inEvent);

    switch(eventClass)
    {
		// Custom Event
		
		case kEventClassDWP:
		{
			if(eventKind == kEventDWPCustom)
			{
				int32_t id  = 0;
				void *param = NULL;
				
				GetEventParameter(inEvent, kEventParamUserData, typeVoidPtr,
								  nil, sizeof(param), nil, &param);
				
				id = (int32_t) param;				
			
				window->receiveCustomMessage(id);
			}
			else if(eventKind == kEventDWPMouse)
			{
				Point point;
				
				GetEventParameter(inEvent, kEventParamMouseLocation, typeQDPoint,
								  nil, sizeof(point), nil, &point);
			
				window->mouseMove(point.h, point.v, 0);
			}
			
			return noErr;
		}		
		break;
		
		// Drawing
	
        case kEventClassWindow:
        {
            if(eventKind == kEventWindowDrawContent)
            {
                WindowRef mwindow;
                
                GetEventParameter(inEvent, kEventParamDirectObject, 
								  typeWindowRef, NULL,
                                  sizeof(mwindow), NULL, &mwindow);      
                
                CGrafPtr graphPort = GetWindowPort(mwindow);
				
				PKRect rect;
				rect.x = 0;
				rect.y = 0;
				rect.w = window->getClientWidth();
				rect.h = window->getClientHeight();
			
				window->draw(graphPort, rect);
				
				return noErr;
            }
			else if(eventKind == kEventWindowShown)
			{
				window->windowShowing();
			}
            else if(eventKind == kEventWindowClose)
            {
				window->windowClosed();
            }
			else if(eventKind == kEventWindowCollapsed)
			{
				window->windowMinimized();
			}
			else if(eventKind == kEventWindowZoomed)
			{
				window->windowResized();
				window->windowMaximized();
			}
			else if(eventKind == kEventWindowResizeCompleted)
			{
				window->windowResized();
			}
			else if(eventKind == kEventWindowBoundsChanged)
			{
				UInt32 attributes;
				GetEventParameter(inEvent, kEventParamAttributes, typeUInt32, NULL,
								  sizeof(UInt32), NULL, &attributes);

				// Only respond if the window is being resized:

				if(attributes & kWindowBoundsChangeOriginChanged)
				{				
					window->windowMoved();
				}
			}
			else if(eventKind == kEventWindowBoundsChanging)
			{
				PKRect rect;
				Rect bounds;

				UInt32 attributes;
				GetEventParameter(inEvent, kEventParamAttributes, typeUInt32, NULL,
								  sizeof(UInt32), NULL, &attributes);

				// Only respond if the window is being resized:

				if(attributes & kWindowBoundsChangeSizeChanged)
				{				
					GetEventParameter(inEvent, kEventParamCurrentBounds, 
									  typeQDRectangle, NULL, 
									  sizeof(Rect), NULL, &bounds);
									
					rect.x = bounds.left;
					rect.y = bounds.top;
					rect.w = (bounds.right - bounds.left);
					rect.h = (bounds.bottom - bounds.top);
			
					window->windowResizing(PK_DIR_BOTTOMRIGHT, &rect);

					bounds.left   = rect.x;
					bounds.top    = rect.y;
					bounds.right  = rect.x + rect.w;
					bounds.bottom = rect.y + rect.h; 
				
					SetEventParameter(inEvent, kEventParamCurrentBounds,
									  typeQDRectangle, sizeof(Rect), &bounds);					
									
					return noErr;
				}
				return eventNotHandledErr;
			}
			else
			{
				return eventNotHandledErr;
			}
        }
        break;
		
		// Keyboard
		
        case kEventClassKeyboard:
        {
            UInt32 keyCode;
            unsigned char charCode;
            UInt32 modifiers;
                
            //  Extract the key code parameter (kEventParamKeyCode).
            
            GetEventParameter(inEvent, kEventParamKeyCode, 
                              typeUInt32, nil, sizeof (keyCode),
                              nil, &keyCode );
                
            //  Extract the character code parameter (kEventParamKeyMacCharCodes).
                
            GetEventParameter(inEvent, kEventParamKeyMacCharCodes, typeChar, nil,
                              sizeof( charCode ), nil, &charCode );
                
            //  Extract the modifiers parameter (kEventParamKeyModifiers).
                
            GetEventParameter(inEvent, kEventParamKeyModifiers, typeUInt32, nil,
                              sizeof( modifiers ), nil, &modifiers );     

			unsigned int modifier  = PK_MOD_NONE;
			PKKey key = PK_KEY_UNKNOWN;
			unsigned int vcode = 0;
			
			EventRecord rec;
			
			rec.message   = charCode | (keyCode << 8);
			rec.modifiers = modifiers;
			
			PKUIComputeKeyState(&rec, &modifier, &key, &vcode);
            
			if(eventKind == kEventRawKeyDown)
            {                
                // Take action
				window->keyDown(modifier, key, vcode);
            }            

			if(eventKind == kEventRawKeyUp)
            {                
                // Take action
				window->keyUp(modifier, key, vcode);
            }            

			return eventNotHandledErr;
        }
        break;
		
		// Mouse
		
        case kEventClassMouse:
        {
            EventMouseButton button;
            Point            point;
			UInt32			 modifiers;
            
            GetEventParameter(inEvent, kEventParamMouseButton, typeMouseButton,
                              nil, sizeof(button), nil, &button);
            
            GetEventParameter(inEvent, kEventParamMouseLocation, typeQDPoint,
                              nil, sizeof(point), nil, &point);
            
			GetEventParameter(inEvent, kEventParamKeyModifiers, typeUInt32,
							  nil, sizeof(modifiers), nil, &modifiers);
							  
			int mod = PK_MOD_NONE;

			if(modifiers & cmdKey)
			{
				mod |= PK_MOD_COMMAND;
			}

			if((modifiers & optionKey) ||
			   (modifiers & rightOptionKey))
			{
				mod |= PK_MOD_OPTION;
			}

			if((modifiers & shiftKey) ||
			   (modifiers & rightShiftKey))
			{
				mod |= PK_MOD_SHIFT;
			}
			
			if((modifiers & (1 << controlKeyBit)) ||
			   (modifiers & (1 << rightControlKeyBit)))
			{
				mod |= PK_MOD_CONTROL;
			}
			
            if(eventKind == kEventMouseDown)
            {
				// Check where the mouse click was
				
				Rect bounds;
				GetWindowBounds(window->macwindow, kWindowContentRgn, &bounds);
				
				Point pt;
				pt.h = point.h;
				pt.v = point.v;
				
				CGrafPtr oldPort;
				Rect     clip;
    
				GetPort(&oldPort);    
				SetPort(window->window);
    
				GetPortBounds(window->window, &clip);
				SetOrigin(0,0);

				GlobalToLocal(&pt);
    
				SetOrigin(clip.left, clip.top);
				SetPort(oldPort);
				
				// Check if it is in the size
				// box area, we need to not handle it
				
				Rect sizeBounds;
				GetWindowBounds(window->macwindow, kWindowGrowRgn, &sizeBounds);
				
				if(point.h >= sizeBounds.left && point.h <= sizeBounds.right &&
				   point.v >= sizeBounds.top && point.v <= sizeBounds.bottom)
				{
					return eventNotHandledErr;
				}

				// If it's in the client area
				// we handle it
				
				if(point.h >= bounds.left && point.h <= bounds.right &&
				   point.v >= bounds.top && point.v <= bounds.bottom)
				{
					if(button == kEventMouseButtonSecondary ||
					  (button == kEventMouseButtonPrimary && (mod & PK_MOD_CONTROL)))
					{
						window->mouseDown(PK_BUTTON_RIGHT, pt.h, pt.v, mod);
					}
					else if(button == kEventMouseButtonPrimary)
					{
						window->mouseDown(PK_BUTTON_LEFT, pt.h, pt.v, mod);
					}

					if(!window->hasChildren)
					{
						return noErr;
					}
				}
				
				return eventNotHandledErr;
			}

            if(eventKind == kEventMouseUp)
            {
				// Check where the mouse click was
				
				Rect bounds;
				GetWindowBounds(window->macwindow, kWindowContentRgn, &bounds);
				
				Point pt;
				pt.h = point.h;
				pt.v = point.v;
				
				CGrafPtr oldPort;
				Rect     clip;
    
				GetPort(&oldPort);    
				SetPort(window->window);
    
				GetPortBounds(window->window, &clip);
				SetOrigin(0,0);

				GlobalToLocal(&pt);
    
				SetOrigin(clip.left, clip.top);
				SetPort(oldPort);
				
				// Check if it is in the size
				// box area, we need to not handle it
				
				Rect sizeBounds;
				GetWindowBounds(window->macwindow, kWindowGrowRgn, &sizeBounds);
				
				if(point.h >= sizeBounds.left && point.h <= sizeBounds.right &&
				   point.v >= sizeBounds.top && point.v <= sizeBounds.bottom)
				{
					return eventNotHandledErr;
				}

				// If it's in the client area
				// we handle it
				
				if(point.h >= bounds.left && point.h <= bounds.right &&
				   point.v >= bounds.top && point.v <= bounds.bottom)
				{
					if(button == kEventMouseButtonSecondary ||
					  (button == kEventMouseButtonPrimary && (mod & PK_MOD_CONTROL)))
					{
						window->mouseUp(PK_BUTTON_RIGHT, point.h, point.v, mod);
					}
					else if(button == kEventMouseButtonPrimary)
					{
						// Here we need to detect double clicks

						bool dblClick = false;
                    
						int64_t time = PKTime::getTime();
						int64_t base = PKTime::getTimeScale();

						if(window->lastClickTime != -1 && base != 0)
						{
							int64_t diff = (time - window->lastClickTime);
							float   dval = (float) diff / (float) base;
                        
							if(dval <= 0.4)
							{
								dblClick = true;
							}
						}

						// Send mouse up first

						window->mouseUp(PK_BUTTON_LEFT, pt.h, pt.v, mod);
                    
						if(dblClick)
						{
							// Then send double click
							window->mouseDblClick(PK_BUTTON_LEFT, pt.h, pt.v, mod);
						}
						else
						{
							window->lastClickTime = time;
						}
					}
					
					if(!window->hasChildren)
					{
						return noErr;
					}
				}
			
				return eventNotHandledErr;
			}
		}
		break;		
		
		default:
		{
            return eventNotHandledErr;
		}
		break;
		
	}
	
	return noErr;
}


pascal OSErr PKWindow::PKWindowDragTrackingHandler(DragTrackingMessage trackingMessage,
												   WindowRef           windowRef,
												   void               *handlerRefCon,
												   DragRef             dragRef)
{
	if(trackingMessage == kDragTrackingLeaveWindow ||
	   trackingMessage == kDragTrackingLeaveHandler)
	{
		SetThemeCursor(kThemeArrowCursor);	
	}
	else
	{
		SetThemeCursor(kThemeCopyArrowCursor);	
	}
	
	return noErr;
}

pascal OSErr PKWindow::PKWindowDragReceiveHandler(WindowRef windowRef, 
											      void     *handlerRefCon, 
											      DragRef   dragRef)
{
	UInt16 numberOfDragItems, index;
	ItemReference itemReference;
	
    PKWindow *window = (PKWindow *) handlerRefCon;
	
	if(window == NULL)
	{
		return noErr;
	}

	std::vector<std::wstring> files;
	
	CountDragItems(dragRef,&numberOfDragItems);
	
	for(index=1; index <= numberOfDragItems; index++) 
	{
		GetDragItemReferenceNumber(dragRef, index, &itemReference);	
	
		UInt16 numFlavors;
		CountDragItemFlavors(dragRef, itemReference, &numFlavors);
		
		for(uint32_t n=1; n<=numFlavors; n++)
		{
			FlavorType type;
			GetFlavorType(dragRef, itemReference, n, &type);

			if(type == 'hfs ')
			{
				HFSFlavor hfs;
				Size dataSize = sizeof(hfs);
				
				if(GetFlavorData(dragRef, itemReference, type, (void *) &hfs, &dataSize, 0) == noErr)
				{
					// Convert to a wstring file path
					FSSpec fSpec = hfs.fileSpec;
					FSRef  ref;
					
					FSpMakeFSRef(&fSpec, &ref);
					
					// Now get the path
					
					OSErr err = noErr;
					std::wstring filePath;
					
					while(err == noErr && 
						  noErr == FSGetCatalogInfo(&ref, 
													kFSCatInfoNone, 
													NULL,NULL,NULL,NULL))
					{ 
						HFSUniStr255  outName; 
						FSCatalogInfo catalogInfo;
						
						err = FSGetCatalogInfo(&ref, 
											   kFSCatInfoNone, 
											   NULL, 
											   &outName, 
											   NULL,NULL); 
						
						CFStringRef  cfname = CFStringCreateWithCharacters(kCFAllocatorDefault, outName.unicode, outName.length);
						std::wstring wfname = PKStr::cfStringToWString(cfname);
						CFRelease(cfname);
						
						err = FSGetCatalogInfo(&ref, 
											   kFSCatInfoNodeID, 
											   &catalogInfo, 
											   NULL,NULL, 
											   &ref); 
						
						if((err == noErr) && 
						   (fsRtDirID == catalogInfo.nodeID)) 
						{ 
							memset(&ref, 0, sizeof(FSRef)); 
						} 
						else
						{
							if(filePath == L"")
							{
								filePath = wfname;
							}
							else
							{								
								filePath = wfname + L"/" + filePath;
							}
						}
					}
					
					filePath = L"/" + filePath;
					
					// Add it to our vector
 					files.push_back(filePath);
				}
			}
		}
		
	}
	
	if(files.size())
	{
		window->filesDropped(files);
	}
	
	return noErr;
}


#endif // MACOSX

// PKWindow

std::string PKWindow::FRAME_RECT_PROPERTY            = "frame";
std::string PKWindow::CAPTION_WSTRING_PROPERTY       = "caption";
std::string PKWindow::SHOWING_BOOL_PROPERTY          = "showing";
std::string PKWindow::ICON_WSTRING_PROPERTY          = "icon";
std::string PKWindow::BACKGROUNDCOLOR_COLOR_PROPERTY = "backgroundColor";

PKWindow::PKWindow(std::wstring   id,
		   PKWindowStyle  style,
		   PKWindowHandle parent,
                   std::wstring   icon)
#ifdef MACOSX
: mouseThread(this)
#endif
{
    this->menu = NULL;
	this->parentWindow = parent;

#ifdef WIN32
	this->window  = NULL;
#endif

#ifdef LINUX
	this->window  = NULL;
#endif
    
    PKColor c  = PKColorCreate(0, 0, 0, 0);
    PKRect  fr = PKRectCreate(0, 0, 640, 480);

	PKOBJECT_ADD_RECT_PROPERTY(frame, fr);
	PKOBJECT_ADD_WSTRING_PROPERTY(caption, L"NOT_SET");
	PKOBJECT_ADD_BOOL_PROPERTY(showing, false);
	PKOBJECT_ADD_WSTRING_PROPERTY(icon, icon);
    PKOBJECT_ADD_COLOR_PROPERTY(backgroundColor, c);

	this->id	  = id;
	this->style   = style;
	
	this->mouseHidden = false;
	this->hasChildren = false;
	
#ifdef LINUX

    this->hasMenu = false;
    this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);    

    if(this->window != NULL)
    {
        // Setup the window
        // DO not setup size constraints here yet

        if(this->style & PK_WINDOW_STYLE_BORDERLESS)
	    {            
             gtk_window_set_decorated(GTK_WINDOW(this->window), FALSE);
        }

        if(style & PK_WINDOW_STYLE_NO_TASKBAR)
        {
            gtk_window_set_skip_taskbar_hint(GTK_WINDOW(this->window), TRUE);
        }

    	if(style & PK_WINDOW_STYLE_TOPMOST ||
    	   style & PK_WINDOW_STYLE_OVER_TOPMOST)
    	{
            gtk_window_set_keep_above(GTK_WINDOW(this->window), TRUE);
    	}

        // Connect signals

    	g_signal_connect (G_OBJECT(this->window), "destroy",
	        	          G_CALLBACK(PKWindow::gtk_destroy), this);

    	g_signal_connect (G_OBJECT(this->window), "expose-event",
	        	          G_CALLBACK(PKWindow::gtk_expose), this);	    	        	          
    }

#endif

#ifdef WIN32

    this->hasMenu = false;
    this->hUser32 = NULL;

	this->setLayeredWindowAttributes = NULL;
	this->updateLayeredWindow = NULL;

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	WNDCLASSEX wclass;

	wclass.cbSize	     = sizeof(WNDCLASSEX);
	wclass.style	     = CS_DBLCLKS;
	wclass.lpfnWndProc   = PKWindow::PKWindowProc;
	wclass.cbClsExtra    = 0;
	wclass.cbWndExtra    = 0;
	wclass.hInstance     = hInst;

	if(this->style & PK_WINDOW_STYLE_NO_TASKBAR)
	{
		wclass.hIcon = NULL;
	}
	else
	{
        PKVariant *ic = this->get(PKWindow::ICON_WSTRING_PROPERTY);
        std::wstring icon = PKVALUE_WSTRING(ic);

        wclass.hIcon = LoadIcon(hInst, icon.c_str());
	}

	wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	if(this->style & PK_WINDOW_STYLE_NO_BKGBRUSH)
	{
		wclass.hbrBackground = NULL;
	}
	else
	{
        PKVariant *bkg = this->get(PKWindow::BACKGROUNDCOLOR_COLOR_PROPERTY);
        PKColor    clr = PKVALUE_COLOR(bkg);

        if(clr.a != 0)
        {
            wclass.hbrBackground = CreateSolidBrush(RGB(clr.r, clr.g, clr.b));
        }
        else
        {
    		wclass.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
        }
	}

	wclass.lpszMenuName  = NULL;
	wclass.hIconSm       = NULL;
	
	// Append the adress of the window proc
	// to the class name so that we are sure
	// we are registering a class with the 
	// right (and valid) function adress

	std::wstring className = id + L"_" + PKStr::uint32ToWstring((uint32_t)PKWindow::PKWindowProc);

	wclass.lpszClassName = className.c_str(); 

	DWORD result = RegisterClassEx(&wclass);

	this->wstyle = 0;

	if(this->style & PK_WINDOW_STYLE_BORDERLESS)
	{
		this->wstyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
	}
	else
	{
		this->wstyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}

	if(this->style & PK_WINDOW_STYLE_NO_MAX)
	{
		this->wstyle &= ~WS_MAXIMIZEBOX;
	}

	if(this->style & PK_WINDOW_STYLE_NO_MIN)
	{
		this->wstyle &= ~WS_MINIMIZEBOX;
	}

    if(this->style & PK_WINDOW_STYLE_NO_RESIZE)
	{
		this->wstyle &= ~WS_THICKFRAME;
	}

	RECT client;

	client.left   = 0;
	client.top    = 0;
	client.right  = 640;
	client.bottom = 480;

    AdjustWindowRect(&client, this->wstyle, this->hasMenu);

	this->exStyle = 0;

	if(this->style & PK_WINDOW_STYLE_NO_TASKBAR)
	{
		this->style   = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
		this->exStyle |= WS_EX_TOOLWINDOW;
	}

    if(this->style & PK_WINDOW_STYLE_DB_BUFFER)
    {
		this->exStyle |= WS_EX_COMPOSITED;
    }

	std::wstring caption;
	PKVariant *cp = this->get(CAPTION_WSTRING_PROPERTY);
	caption = PKVALUE_WSTRING(cp);

	this->window = CreateWindowEx(this->exStyle,
								  className.c_str(),
								  caption.c_str(),
								  this->wstyle, // Style Flags
								  client.left, client.top,
								  client.right - client.left,
								  client.bottom - client.top,
								  parent, NULL, hInst, this);

	if(this->window == NULL)
	{
		return;
	}

	// Setup file drag & drop

	if(this->style & PK_WINDOW_STYLE_ACCEPT_DROP)
	{
		DragAcceptFiles(this->window, TRUE);
	}

	// Setup the other styles

	this->id = className;

	this->hUser32 = LoadLibrary(L"user32.dll");

    this->setLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes) 
		GetProcAddress(hUser32, "SetLayeredWindowAttributes");

	this->updateLayeredWindow = (lpfnUpdateLayeredWindow)
		GetProcAddress(hUser32, "UpdateLayeredWindow");

	this->hUxTheme    = LoadLibrary(L"UxTheme.dll");
	this->isAppThemed = NULL;

	if(this->hUxTheme != NULL)
	{
		this->isAppThemed = (lpfnIsAppThemed)
			GetProcAddress(this->hUxTheme, "IsAppThemed");
	}

    if(this->style & PK_WINDOW_STYLE_LAYERED)
	{
		if(this->setLayeredWindowAttributes)
		{
			SetWindowLong(this->window, GWL_EXSTYLE, 
						  GetWindowLong(this->window, GWL_EXSTYLE) | WS_EX_LAYERED);

			this->setLayeredWindowAttributes(this->window, 
						                     0, 
											 255, // alpha is 255 on startup
											 LWA_ALPHA);
		}
	}

	SetWindowLongPtr(this->window, GWLP_USERDATA, (LONG)(LPVOID) this);

	if(style & PK_WINDOW_STYLE_TOPMOST ||
	   style & PK_WINDOW_STYLE_OVER_TOPMOST)
	{
		SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

    this->menu = NULL;

#endif

#ifdef MACOSX

    this->macwindow = NULL;
    this->window    = NULL;
	this->wndGroup  = NULL;
    
    this->lastClickTime = -1;
    
    this->lastMouseX = -1;
    this->lastMouseY = -1;
        
    Rect wndRect;
	
	wndRect.left   = 0;
	wndRect.right  = 640;
	wndRect.top    = 0;
	wndRect.bottom = 480;

	WindowClass wclass = kDocumentWindowClass;
	
	if(style & PK_WINDOW_STYLE_BORDERLESS)
	{
		wclass = kPlainWindowClass;
	}
	
	if(style & PK_WINDOW_STYLE_OVERLAY)
	{
		wclass = kOverlayWindowClass;
	}

	if(style & PK_WINDOW_STYLE_SHEET)
	{
		wclass = kSheetWindowClass;
	}
	
    // Create Window

    WindowAttributes attrs;

	if(style & PK_WINDOW_STYLE_BORDERLESS)
	{
		attrs = kWindowStandardHandlerAttribute;
	}
	else
	{
		attrs = kWindowCloseBoxAttribute        | 
				kWindowCollapseBoxAttribute     | 
				kWindowStandardHandlerAttribute |
				kWindowResizableAttribute       |
				kWindowFullZoomAttribute;
    }

	if(!(style & PK_WINDOW_STYLE_NO_BKGBRUSH))
	{
		attrs |= kWindowCompositingAttribute;
	}
	
	if(style & PK_WINDOW_STYLE_NO_SHADOW)
	{
		attrs |= kWindowNoShadowAttribute;
	}
	
	if(style & PK_WINDOW_STYLE_NO_ACTIVATE)
	{
		attrs |= (kWindowNoActivatesAttribute | kWindowDoesNotCycleAttribute);
	}

	if(this->style & PK_WINDOW_STYLE_NO_MAX)
	{
		attrs &= ~kWindowFullZoomAttribute;
	}

	if(this->style & PK_WINDOW_STYLE_NO_MIN)
	{
		attrs &= ~kWindowCollapseBoxAttribute;
	}

    if(this->style & PK_WINDOW_STYLE_NO_RESIZE)
	{
		attrs &= ~kWindowResizableAttribute;
	}

	if(style & PK_WINDOW_STYLE_SHEET)
	{
		attrs = kWindowStandardHandlerAttribute | kWindowCompositingAttribute;
	}
	
    CreateNewWindow(wclass, 
                    attrs, 
                    &wndRect, &this->macwindow);
            
	if(this->macwindow == NULL)
	{
		return;
	}
			
    SetWindowProperty(this->macwindow, 
					 'dwpw', 'dwpw', 
					  sizeof(this), this);
    
	if(!(style & PK_WINDOW_STYLE_NO_BKGBRUSH))
	{
		SetThemeWindowBackground(this->macwindow,
								 kThemeBrushDialogBackgroundActive,
								 true);
	}
	
	std::wstring caption;
	PKVariant *cp = this->get(CAPTION_WSTRING_PROPERTY);
	caption = PKVALUE_WSTRING(cp);

	CFStringRef captionStr = PKStr::wStringToCFString(caption);
	
	SetWindowTitleWithCFString(this->macwindow,
							   captionStr);
	CFRelease(captionStr);
	
    this->window = GetWindowPort(this->macwindow);
        
    // Install Window Event Handler
    
    EventTypeSpec    eventTypes[15];
    EventHandlerUPP  handlerUPP;     // Pointer to event handler routine
    
    eventTypes[0].eventClass = kEventClassWindow;
    eventTypes[0].eventKind  = kEventWindowClose;

    eventTypes[1].eventClass = kEventClassKeyboard;
    eventTypes[1].eventKind  = kEventRawKeyDown;
    
    eventTypes[2].eventClass = kEventClassKeyboard;
    eventTypes[2].eventKind  = kEventRawKeyUp;

    eventTypes[3].eventClass = kEventClassMouse;
    eventTypes[3].eventKind  = kEventMouseUp;

    eventTypes[4].eventClass = kEventClassMouse;
    eventTypes[4].eventKind  = kEventMouseDown;
    
    eventTypes[5].eventClass = kEventClassWindow;
    eventTypes[5].eventKind  = kEventWindowDrawContent;

    eventTypes[6].eventClass = kEventClassWindow;
    eventTypes[6].eventKind  = kEventWindowClose;
	
    eventTypes[7].eventClass = kEventClassWindow;
	eventTypes[7].eventKind  = kEventWindowResizeCompleted;
	
    eventTypes[8].eventClass = kEventClassWindow;
	eventTypes[8].eventKind  = kEventWindowBoundsChanged;
	
	eventTypes[9].eventClass = kEventClassWindow;
	eventTypes[9].eventKind  = kEventWindowBoundsChanging;
	
	eventTypes[10].eventClass = kEventClassDWP;
	eventTypes[10].eventKind  = kEventDWPCustom;

	eventTypes[11].eventClass = kEventClassDWP;
	eventTypes[11].eventKind  = kEventDWPMouse;

	eventTypes[12].eventClass = kEventClassWindow;
	eventTypes[12].eventKind  = kEventWindowShown;
	
	eventTypes[13].eventClass = kEventClassWindow;
	eventTypes[13].eventKind  = kEventWindowCollapsed;

	eventTypes[14].eventClass = kEventClassWindow;
	eventTypes[14].eventKind  = kEventWindowZoomed;
    
	handlerUPP = NewEventHandlerUPP(PKWindow::PKWindowEventHandler);   
    
    InstallWindowEventHandler(this->macwindow, handlerUPP,
                              15, eventTypes, this, &this->handler);
	
	InstallStandardEventHandler(GetWindowEventTarget(this->macwindow));

	// Install Drag & Drop if needed
	
	this->dragTrackingHandler = NULL;
	this->dragReceiveHandler  = NULL;

	if(style & PK_WINDOW_STYLE_ACCEPT_DROP)
	{
		DragTrackingHandlerUPP trackingHandlerUPP;
		trackingHandlerUPP = NewDragTrackingHandlerUPP((DragTrackingHandlerProcPtr) PKWindow::PKWindowDragTrackingHandler);
		
		InstallTrackingHandler(trackingHandlerUPP, this->macwindow, (void *) this);

		DragReceiveHandlerUPP receiveHandlerUPP;
		receiveHandlerUPP = NewDragReceiveHandlerUPP((DragReceiveHandlerProcPtr) PKWindow::PKWindowDragReceiveHandler);
		
		InstallReceiveHandler(receiveHandlerUPP, this->macwindow, (void *) this);
	}
	
	
	if(style & PK_WINDOW_STYLE_NO_ACTIVATE)
	{
		SetWindowActivationScope(this->macwindow, kWindowActivationScopeNone);
	}
	
	if(style & PK_WINDOW_STYLE_TOPMOST)
	{
		// Create a window group and place
		// this window in it, above the menu
		// bar and dock
		
		OSStatus err = CreateWindowGroup(kWindowGroupAttrSharedActivation, 
										&this->wndGroup);
		
		if(err == noErr)
		{
			SetWindowGroupLevel(this->wndGroup, kCGMainMenuWindowLevel+1);
			SendWindowGroupBehind(this->wndGroup, GetWindowGroupOfClass(kFloatingWindowClass));
			SetWindowGroup(this->macwindow, this->wndGroup);
		}
	}

	if(style & PK_WINDOW_STYLE_OVER_TOPMOST)
	{
		// Create a window group and place
		// this window in it, above the menu
		// bar and dock + 1
		
		OSStatus err = CreateWindowGroup(kWindowGroupAttrSharedActivation, 
										&this->wndGroup);
		
		if(err == noErr)
		{
			SetWindowGroupLevel(this->wndGroup, kCGMainMenuWindowLevel+2);
			SendWindowGroupBehind(this->wndGroup, GetWindowGroupOfClass(kFloatingWindowClass));
			SetWindowGroup(this->macwindow, this->wndGroup);
		}
	}
	
#endif
}

PKWindow::~PKWindow()
{
#ifndef LINUX
	this->hide();
#endif

	this->eventMutex.wait();

#ifdef WIN32

	if(this->window != NULL)
	{
		DestroyWindow(this->window);
	}

	if(this->hUser32 != NULL)
	{
		FreeLibrary(this->hUser32);
	}

	if(this->hUxTheme != NULL)
	{
		FreeLibrary(this->hUxTheme);
	}

#endif

#ifdef MACOSX

	RemoveEventHandler(this->handler);
	
	if(this->dragTrackingHandler)
	{
		RemoveEventHandler(this->dragTrackingHandler);
		this->dragTrackingHandler = NULL;
	}
	
	if(this->dragReceiveHandler)
	{
		RemoveEventHandler(this->dragReceiveHandler);
		this->dragReceiveHandler = NULL;
	}

    if(this->macwindow != NULL)
    {
        DisposeWindow(this->macwindow);        
    }    
	
	if(this->wndGroup != NULL)
	{
		ReleaseWindowGroup(this->wndGroup);
		this->wndGroup = NULL;
	}

#endif

	this->eventMutex.release();
}

PKWindowHandle PKWindow::getWindowHandle()
{
	return this->window;
}

#ifdef WIN32
HBRUSH PKWindow::getBackgroundBrush()
{

    return (HBRUSH) GetClassLongPtr(this->window, GCLP_HBRBACKGROUND);
}
#endif

PKMenu *PKWindow::getMenu()
{
	return this->menu;
}

void PKWindow::setMenu(PKMenu *menu)
{
    this->hasMenu = false;

#ifdef MACOSX
	this->menu = menu;
#endif
	
#ifdef WIN32
    SetMenu(this->window, menu->getMenuHandle());

    this->menu = NULL;

    if(menu)
    {
        this->hasMenu = true;
        this->menu = menu;
    }
#endif

#ifdef LINUX

    this->menu = menu;
    this->hasMenu = true;

    if(this->getChildrenCount() == 0)
    {
        return;
    }        
    
    PKVLayout *root = dynamic_cast<PKVLayout*>(this->getChildAt(0));
    
    if(root)
    {
        GtkWidget *glayout = root->getWindowHandle();
        
        if(glayout)
  	    {
            // Window was already built, let's insert the menu
            root->prependChild(this->menu);
            this->menu->build();
            gtk_box_pack_start(GTK_BOX(glayout), this->menu->getMenuHandle(), FALSE, FALSE, 0);
            gtk_box_reorder_child(GTK_BOX(glayout), this->menu->getMenuHandle(), 0);
	    }        	    
	    else
  	    {
            // Window was NOT built yet, menu will be
            // inserted automatically when the window is built
	    }        	    
    }        	    

#endif
}

bool PKWindow::isUsingXPThemes()
{
#ifdef WIN32
	if(this->isAppThemed != NULL)
	{
		return (bool) this->isAppThemed();
	}
#endif

	return false;
}

void PKWindow::getGlobalMouse(int32_t *x,
							  int32_t *y)
{
	if(x == NULL || y == NULL)
	{
		return;
	}

#ifdef WIN32
	POINT pt;
	GetCursorPos(&pt);

	*x = pt.x;
	*y = pt.y;
#endif

#ifdef LINUX

    GdkDisplay *display = gdk_display_get_default();
    GdkScreen  *screen  = NULL;

    gint xx = 0;
    gint yy = 0;

    gdk_display_get_pointer(display, &screen, &xx, &yy, NULL);
    
	*x = xx;
	*y = yy;

#endif

#ifdef MACOSX
	Point p;
	GetGlobalMouse(&p);

	*x = p.h;
	*y = p.v;
#endif
}

void PKWindow::getClientMouse(int32_t *x,
							  int32_t *y)
{
	if(x == NULL || y == NULL)
	{
		return;
	}

#ifdef WIN32
	POINT pt;
	GetCursorPos(&pt);
    ScreenToClient(this->window, &pt);

	*x = pt.x;
	*y = pt.y;
#endif

#ifdef LINUX


    gint xx = 0;
    gint yy = 0;

    GdkModifierType mask;

    gdk_window_get_pointer(GDK_WINDOW(this->window->window),
                                              &xx, &yy, &mask);    
	*x = xx;
	*y = yy;

#endif

#ifdef MACOSX

		Point p;
		GetGlobalMouse(&p);

		CGrafPtr oldPort;
		Rect     clip;
    
		GetPort(&oldPort);    
		SetPort(this->window);
    
		GetPortBounds(this->window, &clip);
		SetOrigin(0,0);

		GlobalToLocal(&p);
    
		SetOrigin(clip.left, clip.top);
		SetPort(oldPort);

	*x = p.h;
	*y = p.v;
#endif
}

// PKObject

void PKWindow::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	// This is where we watch
	// for changes in the frame rect,
	// the caption and the showing state

	if(prop->getName() == SHOWING_BOOL_PROPERTY)
	{
		PKVariant *s = this->get(SHOWING_BOOL_PROPERTY);
		bool showing = PKVALUE_BOOL(s);

#ifdef WIN32

		if(this->window != NULL)
		{
			if(showing)
			{
				ShowWindow(this->window, SW_SHOW);
			}
			else
			{
				ShowWindow(this->window, SW_HIDE);
			}
		}

#endif

#ifdef LINUX

		if(this->window != NULL)
		{
			if(showing)
			{
				gtk_widget_show(this->window);
			}
			else
			{
				gtk_widget_hide(this->window);
			}
		}

#endif

#ifdef MACOSX

		if(this->macwindow != NULL)
		{
			if(showing)
			{
				this->lastClickTime = -1;
			
				this->lastMouseX = -1;
				this->lastMouseY = -1;

				if(this->style & PK_WINDOW_STYLE_SHEET)
				{
					ShowSheetWindow(this->macwindow, GetWindowFromPort(this->parentWindow));
				}
				else 
				{
					ShowWindow(this->macwindow);					
				}
				
				this->mouseThread.start();
				
				BringToFront(this->macwindow);
			}
			else
			{
				this->mouseThread.stop();

				if(this->style & PK_WINDOW_STYLE_SHEET)
				{
					HideSheetWindow(this->macwindow);
				}
				else 
				{
					HideWindow(this->macwindow);					
				}
			}
		}

#endif
	}

    if(prop->getName() == PKWindow::BACKGROUNDCOLOR_COLOR_PROPERTY)
    {
        PKVariant *bkg = prop->getValue();
        PKColor    clr = PKVALUE_COLOR(bkg);

        // TODO: Implement for MAC & LINUX

#ifdef WIN32
        if(clr.a != 0)
        {
            HBRUSH br = CreateSolidBrush(RGB(clr.r, clr.g, clr.b));
            SetClassLongPtr(this->window, GCLP_HBRBACKGROUND, (LONG) br);
            
        }
        else
        {
            SetClassLongPtr(this->window, GCLP_HBRBACKGROUND, (LONG)(HBRUSH)(1 + COLOR_BTNFACE));
        }
#endif

#ifdef LINUX
    
        if(this->window != NULL)
    	{        
            GdkColor color;
            color.red   = ((uint32_t)clr.r)*65535/255;
            color.green = ((uint32_t)clr.g)*65535/255;
            color.blue  = ((uint32_t)clr.b)*65535/255;
            color.pixel = 0;

            gtk_widget_modify_bg(this->window, GTK_STATE_NORMAL, &color);
            gtk_widget_modify_bg(this->window, GTK_STATE_ACTIVE, &color);
            gtk_widget_modify_bg(this->window, GTK_STATE_PRELIGHT, &color);
            gtk_widget_modify_bg(this->window, GTK_STATE_INSENSITIVE, &color);
            gtk_widget_modify_bg(this->window, GTK_STATE_SELECTED, &color);
        }

#endif

    }

	if(prop->getName() == CAPTION_WSTRING_PROPERTY)
	{
		PKVariantWString *c = (PKVariantWString *) prop->getValue();
		std::wstring caption = PKVALUE_WSTRING(c);
		
#ifdef WIN32
		SendMessage(this->window, WM_SETTEXT, 0, (LPARAM) caption.c_str());
#endif

#ifdef LINUX
        if(this->window != NULL)
        {
            std::string captionUTF8 = PKStr::wStringToUTF8string(caption);
            gtk_window_set_title(GTK_WINDOW(this->window), captionUTF8.c_str());
        }    
#endif

#ifdef MACOSX
		CFStringRef title = PKStr::wStringToCFString(caption);
		SetWindowTitleWithCFString(this->macwindow, title);
		CFRelease(title);
#endif
	}

	if(prop->getName() == FRAME_RECT_PROPERTY)
	{
		PKVariantRect *f = (PKVariantRect *) prop->getValue();
		PKRect frame = PKVALUE_RECT(f);

#ifdef WIN32

		if(this->window != NULL)
		{
			SetWindowPos(this->window, NULL, frame.x, frame.y, frame.w, frame.h, SWP_NOZORDER); 
		}
#endif

#ifdef LINUX

        if(this->window != NULL)
        {
            gtk_widget_set_uposition(this->window, frame.x, frame.y);
            gtk_widget_set_usize(this->window, frame.w, frame.h);
        }

#endif

#ifdef MACOSX

		if(this->window != NULL)
		{
			Rect wndRect;

			wndRect.left   = frame.x;
			wndRect.top    = frame.y;
			wndRect.right  = frame.x + frame.w;
			wndRect.bottom = frame.y + frame.h;

			SetWindowBounds(this->macwindow,
							kWindowStructureRgn,
							&wndRect);	
		}
#endif

	}

	// Call superclass
	PKView::selfPropertyChanged(prop, oldValue);
}

// Move / Resize

void PKWindow::moveWindowTo(int x, int y)
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);

	frame.x = x;
	frame.y = y;

	PKVariant *newFrame = PKVariantRect_Create(frame);
	this->set(FRAME_RECT_PROPERTY, newFrame);
	delete newFrame;
}

void PKWindow::moveClientTo(int x, int y)
{
#ifdef WIN32

	if(this->window != NULL)
	{
		RECT rc;

		rc.left   = x;
		rc.top    = y;
		rc.right  = x + 200;
		rc.bottom = x + 100;
        AdjustWindowRect(&rc, this->wstyle, this->hasMenu);
		
		PKVariant *f = this->get(FRAME_RECT_PROPERTY);
		PKRect frame = PKVALUE_RECT(f);

		frame.x = rc.left;
		frame.y = rc.top;

		PKVariant *newFrame = PKVariantRect_Create(frame);
		this->set(FRAME_RECT_PROPERTY, newFrame);
		delete newFrame;
	}

#endif

#ifdef MACOSX

	if(this->macwindow != NULL)
	{
		Rect clientRect;
		GetWindowBounds(this->macwindow, kWindowContentRgn, &clientRect);
		
		Rect windowRect;
		GetWindowBounds(this->macwindow, kWindowStructureRgn, &windowRect);

		// compute offsets between the structure
		// rectangle and the client rectangle
		
		int diffLeft   = clientRect.left   - windowRect.left;		
		int diffTop    = clientRect.top    - windowRect.top;		
		
		PKVariant *f = this->get(FRAME_RECT_PROPERTY);
		PKRect frame = PKVALUE_RECT(f);

		frame.x = x - diffLeft;
		frame.y = y - diffTop;

		PKVariant *newFrame = PKVariantRect_Create(frame);
		this->set(FRAME_RECT_PROPERTY, newFrame);
		delete newFrame;
	}
	
#endif
}

void PKWindow::centerOnScreen()
{
    PKScreenTool tool;

    int w = this->getWindowWidth();
    int h = this->getWindowHeight();

    std::vector <PKRect> screens = tool.getScreens();

    if(screens.size() > 0)
    {
        int x = screens[0].x + (screens[0].w - w)/2;
        int y = screens[0].y + (screens[0].h - h)/2;

        this->moveWindowTo(x, y);
    }
}


int PKWindow::getWindowX()
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);
	return frame.x;
}

int PKWindow::getWindowY()
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);
	return frame.y;
}

int PKWindow::getClientX()
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);

#ifdef WIN32
	
    RECT rc;

    rc.left   = frame.x;
    rc.top    = frame.y;
    rc.right  = frame.x + 200;
    rc.bottom = frame.y + 100;
    AdjustWindowRect(&rc, this->wstyle, this->hasMenu);

    return (frame.x + (frame.x - rc.left));
	
#endif
	
#ifdef MACOSX
	
	Rect clientRect;
	GetWindowBounds(this->macwindow, kWindowContentRgn, &clientRect);
	
	Rect windowRect;
	GetWindowBounds(this->macwindow, kWindowStructureRgn, &windowRect);
	
	// compute offsets between the structure
	// rectangle and the client rectangle
	
	int diffLeft   = clientRect.left   - windowRect.left;		
		
	return frame.x + diffLeft;

#endif

#ifdef LINUX

    // TODO?
    
#endif

	return 0;
}

int PKWindow::getClientY()
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);

#ifdef WIN32
	
    RECT rc;

    rc.left   = frame.x;
    rc.top    = frame.y;
    rc.right  = frame.x + 200;
    rc.bottom = frame.y + 100;
    AdjustWindowRect(&rc, this->wstyle, this->hasMenu);

    return (frame.y + (frame.y - rc.top));
	
#endif

#ifdef MACOSX
	
	Rect clientRect;
	GetWindowBounds(this->macwindow, kWindowContentRgn, &clientRect);
	
	Rect windowRect;
	GetWindowBounds(this->macwindow, kWindowStructureRgn, &windowRect);
	
	// compute offsets between the structure
	// rectangle and the client rectangle
	
	int diffTop    = clientRect.top    - windowRect.top;		
	
	return frame.y + diffTop;
	
#endif
	
#ifdef LINUX

    // TODO?
    
#endif

	return 0;
}

int PKWindow::getWindowWidth()
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);
	return frame.w;
}

int PKWindow::getWindowHeight()
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);
	return frame.h;
}

int PKWindow::getClientWidth()
{
#ifdef MACOSX

	if(this->macwindow != NULL)
	{
		Rect clientRect;
		GetWindowBounds(this->macwindow, kWindowContentRgn, &clientRect);
	
		return (clientRect.right - clientRect.left);
	}

#endif

#ifdef WIN32

	RECT rc;
	RECT rw;

	rc.left = rc.top = 100;
	rc.right  = 200;
	rc.bottom = 200;

	rw.left = rw.top = 100;
	rw.right  = 200;
	rw.bottom = 200;

	AdjustWindowRect(&rw, this->wstyle, this->hasMenu);

	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);

	return (frame.w - 
		   (rc.left - rw.left) - 
		   (rw.right - rc.right));

#endif

	return 0;
}

int PKWindow::getClientHeight()
{
#ifdef MACOSX

	if(this->macwindow != NULL)
	{
		Rect clientRect;
		GetWindowBounds(this->macwindow, kWindowContentRgn, &clientRect);
	
		return (clientRect.bottom - clientRect.top);
	}

#endif

#ifdef WIN32

	RECT rc;
	RECT rw;

	rc.left = rc.top = 100;
	rc.right  = 200;
	rc.bottom = 200;

	rw.left = rw.top = 100;
	rw.right  = 200;
	rw.bottom = 200;

	AdjustWindowRect(&rw, this->wstyle, this->hasMenu);

	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);

	return (frame.h - 
		   (rc.top - rw.top) - 
		   (rw.bottom - rc.bottom));

#endif

	return 0;
}

void PKWindow::resizeWindowAreaTo(int w, int h)
{
	PKVariant *f = this->get(FRAME_RECT_PROPERTY);
	PKRect frame = PKVALUE_RECT(f);

	frame.w = w;
	frame.h = h;

	PKVariant *newFrame = PKVariantRect_Create(frame);
	this->set(FRAME_RECT_PROPERTY, newFrame);
	delete newFrame;
}

void PKWindow::resizeClientAreaTo(int w, int h)
{
#ifdef WIN32
	
	if(this->window != NULL)
	{
		RECT rc;

		rc.left = rc.top = 0;
		rc.right  = w;
		rc.bottom = h;

        AdjustWindowRect(&rc, this->wstyle, this->hasMenu);
		
		PKVariant *f = this->get(FRAME_RECT_PROPERTY);
		PKRect frame = PKVALUE_RECT(f);

		frame.w = rc.right - rc.left;
		frame.h = rc.bottom - rc.top;

		PKVariant *newFrame = PKVariantRect_Create(frame);
		this->set(FRAME_RECT_PROPERTY, newFrame);
		delete newFrame;
	}

#endif

#ifdef LINUX 

    // TODO

#endif

#ifdef MACOSX

	if(this->macwindow != NULL)
	{
		Rect clientRect;
		GetWindowBounds(this->macwindow, kWindowContentRgn, &clientRect);
		
		Rect windowRect;
		GetWindowBounds(this->macwindow, kWindowStructureRgn, &windowRect);

		// compute offsets between the structure
		// rectangle and the client rectangle
		
		int diffLeft   = clientRect.left   - windowRect.left;		
		int diffRight  = windowRect.right  - clientRect.right;		
		int diffTop    = clientRect.top    - windowRect.top;		
		int diffBottom = windowRect.bottom - clientRect.bottom;	
		
		PKVariant *f = this->get(FRAME_RECT_PROPERTY);
		PKRect frame = PKVALUE_RECT(f);

		frame.w = diffLeft + w + diffRight;
		frame.h = diffTop  + h + diffBottom;

		PKVariant *newFrame = PKVariantRect_Create(frame);
		this->set(FRAME_RECT_PROPERTY, newFrame);
		delete newFrame;
	}
	
#endif
}

void PKWindow::setAlpha(unsigned char alpha)
{
#ifdef WIN32
	
	if(this->setLayeredWindowAttributes != NULL && this->window != NULL)
	{
		if(alpha == 255)
		{
			SetWindowLong(this->window, GWL_EXSTYLE, 
						  GetWindowLong(this->window, GWL_EXSTYLE) & !WS_EX_LAYERED);
			
			InvalidateRect(this->window, NULL, FALSE);
		}
		else
		{
			SetWindowLong(this->window, GWL_EXSTYLE, 
						  GetWindowLong(this->window, GWL_EXSTYLE) | WS_EX_LAYERED);

			this->setLayeredWindowAttributes(this->window, 
						                     0, 
											 alpha, // alpha 
											 LWA_ALPHA);
		}
	}

#endif

#ifdef LINUX

    if(this->window != NULL)
    {
        gdouble al = (gdouble) alpha / 255.0;
        gtk_window_set_opacity(GTK_WINDOW(this->window), al);
    }

#endif

#ifdef MACOSX

	if(this->macwindow != NULL)
	{
		float a = (float) alpha / 255.0;
		
		if(a < 0) a = 0;
		if(a > 1) a = 1;
		
		SetWindowAlpha(this->macwindow, a);
	}

#endif

}


// Show / Hide

void PKWindow::show()
{
	PKVariantBool s(true);
	this->set(SHOWING_BOOL_PROPERTY, &s);
}

void PKWindow::hide()
{
	PKVariantBool s(false);
	this->set(SHOWING_BOOL_PROPERTY, &s);

	this->showMouse();
}

bool PKWindow::isShowing()
{
	PKVariant *s = this->get(SHOWING_BOOL_PROPERTY);
	return PKVALUE_BOOL(s);
}

void PKWindow::raise()
{
	if(window == NULL)
	{
        return;
	}

#ifdef WIN32
	SetWindowPos(this->window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
#endif

#ifdef LINUX
    gtk_window_present(GTK_WINDOW(this->window));
#endif

#ifdef MACOSX
	if(this->macwindow != NULL)
	{
		BringToFront(this->macwindow);
	}
#endif
}

void PKWindow::minimize()
{
	if(window == NULL)
	{
        return;
	}

#ifdef WIN32
    ShowWindow(this->window, SW_SHOWMINIMIZED);
#endif

#ifdef LINUX
    gtk_window_iconify(GTK_WINDOW(this->window));
#endif

#ifdef MACOSX
    CollapseWindow(this->macwindow, true);
#endif
}

void PKWindow::maximize()
{
	if(window == NULL)
	{
        return;
	}

#ifdef WIN32
    ShowWindow(this->window, SW_SHOWMAXIMIZED);
#endif
    
#ifdef LINUX
    gtk_window_maximize(GTK_WINDOW(this->window));
#endif

#ifdef MACOSX
	Point pt;
    ZoomWindowIdeal(this->macwindow, inZoomIn, &pt);
#endif
}

void PKWindow::hideMouse()
{
	PKCritLock lock(&this->mouseMutex);

	if(!this->mouseHidden)
	{
#ifdef WIN32
		if(this->window != NULL)
		{
			SendMessage(this->window, WM_HIDECURSOR, 0, 0);
		}
#endif
#ifdef LINUX 
        // TODO
#endif
#ifdef MACOSX
		PKCursorTools::hideCursor();
#endif
		this->mouseHidden = true;
	}
}

void PKWindow::showMouse()
{
	PKCritLock lock(&this->mouseMutex);

	if(this->mouseHidden)
	{
		PKCursorTools::showCursor();
		this->mouseHidden = false;
	}
}

// Threading / Messages

void PKWindow::sendCustomMessage(uint32_t id)
{
#ifdef WIN32

	if(this->window != NULL)
	{
		PostMessage(this->window, WM_PK_CUSTOM + id, 0, 0);
	}

#endif

#ifdef LINUX

    // TODO

#endif

#ifdef MACOSX

	OSErr	 err;
	EventRef newEvent;
    
	if(CreateEvent(NULL, kEventClassDWP, kEventDWPCustom,
                   0, kEventAttributeNone, &newEvent) != noErr)
	{
        return;
	}
	
	EventTargetRef target = GetWindowEventTarget(this->macwindow);
	
    err = SetEventParameter(newEvent, kEventParamPostTarget,
                            typeEventTargetRef, sizeof(target), &target);
							
	void *param = (void *) id;
							
	err = SetEventParameter(newEvent, kEventParamUserData,
							typeVoidPtr, sizeof(param), &param);
							
    if (err == noErr) 
	{
        err = PostEventToQueue(GetMainEventQueue(), newEvent, kEventPriorityStandard);
        ReleaseEvent(newEvent);
    }

#endif
}

void PKWindow::receiveCustomMessage(uint32_t id)
{
#ifdef MACOSX
	if(id == CUSTOM_MESSAGE_DRAW32BPPBITMAP)
	{
		this->drawMutex.wait();

		for(unsigned int i=0; i < this->drawRequests.size(); i++)
		{
			if(this->drawRequests[i] != NULL)
			{
				PKBlitter::blitRGB32At(this->window,
								  	   this->drawRequests[i]->data,
									   this->drawRequests[i]->x,
									   this->drawRequests[i]->y,
									   this->drawRequests[i]->w,
									   this->drawRequests[i]->h);
		
				if(this->drawRequests[i]->data != NULL)
				{
					delete [] this->drawRequests[i]->data;
				}
		
				delete this->drawRequests[i];
			}
		}

		this->drawRequests.clear();

		this->drawMutex.release();
	}
#endif
}

//
// i18n
//

void PKWindow::updateUIStrings()
{
    //
    // Update our caption
    //

    PKPropertyBag *props = this->getProperties();

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

    //
    // See if we have a menu
    // and if so update it now
    //

#if(defined(WIN32) || defined(LINUX)) 
	
    if(this->menu != NULL)
    {
        this->menu->updateUIStrings();
    }

#endif
}

// Drawing

void PKWindow::redraw()
{
#ifdef WIN32

	if(this->window != NULL)
	{
		InvalidateRect(this->window, NULL, false);
	}

#endif

#ifdef LINUX

    if(this->window != NULL)
	{
        gtk_widget_queue_draw(this->window);        
	}    

#endif

#ifdef MACOSX
	
	if(this->macwindow)
	{
		Rect clientRect;
		GetWindowBounds(this->macwindow, kWindowContentRgn, &clientRect);
		InvalWindowRect(this->macwindow, &clientRect);
	}
	
#endif
}

void PKWindow::draw32BppBitmap(unsigned int x,
							    unsigned int y,
							    void        *data,
							    unsigned int w,
							    unsigned int h)
{
#ifdef WIN32

	if(this->window != NULL)
	{
		HDC     dc   = GetDC(this->window);
		HBITMAP bmp  = CreateCompatibleBitmap(dc, w, h);
		HDC     bdc  = CreateCompatibleDC(dc);

		if(bmp == NULL)
		{
			DeleteDC(bdc);
			ReleaseDC(this->window, dc);
			return;
		}

		BITMAPINFO dibInfo;
		memset(&dibInfo, 0, sizeof(BITMAPINFO));

		dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
		dibInfo.bmiHeader.biWidth         =  w;
		dibInfo.bmiHeader.biHeight        =  -(int) h;
		dibInfo.bmiHeader.biBitCount      =  32;
		dibInfo.bmiHeader.biClrUsed       =  0;
		dibInfo.bmiHeader.biClrImportant  =  0;
		dibInfo.bmiHeader.biCompression   =  BI_RGB;
		dibInfo.bmiHeader.biPlanes        =  1;
		dibInfo.bmiHeader.biSizeImage     =  w*h*4;
		dibInfo.bmiHeader.biXPelsPerMeter =  0;
		dibInfo.bmiHeader.biYPelsPerMeter =  0;

		HGDIOBJ old = SelectObject(bdc, bmp);

		SetDIBits(bdc, bmp, 0, h, data, &dibInfo, 0);

		UINT color = RGB(255, 0, 255);
		TransparentBlt(dc, x, y, w, h, bdc, 0, 0, w, h, color);

		SelectObject(bdc, old);
		DeleteObject(bmp);
		DeleteDC(bdc);

		ReleaseDC(this->window, dc);
	}

#endif

#ifdef MACOSX
	
	this->drawMutex.wait();

	PKDrawRequest *request = new PKDrawRequest;
	
	if(request != NULL)
	{
		request->data = new uint8_t[w*h*4];

		if(request->data != NULL)
		{
			memcpy(request->data, data, w*h*4);
		}
		else
		{
			delete request;
			this->drawMutex.release();
			
			return;
		}

		request->x = x;
		request->y = y;
		request->w = w;
		request->h = h;

		this->drawRequests.push_back(request);
		this->sendCustomMessage(CUSTOM_MESSAGE_DRAW32BPPBITMAP);
	}

	this->drawMutex.release();

#endif
}

void PKWindow::draw(PKDrawingDevice device, 
					PKRect rectToPaint)
{
	// Subclass to implement
}

void PKWindow::fillRect(PKDrawingDevice device,
						PKRect rect,
					    PKColor color)
{
#ifdef WIN32

	RECT prect;

	prect.left   = rect.x;
	prect.top    = rect.y;
	prect.right  = rect.x + rect.w;
	prect.bottom = rect.y + rect.h;

	HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	FillRect(device, &prect, brush);
	DeleteObject(brush);

#endif
}

// Drag & Drop

void PKWindow::filesDropped(std::vector<std::wstring> files)
{
	// Reimplement this to receive dropped files
}

// Misc

void PKWindow::menuItemAboutToBeShown(PKMenu *fromMenu,
									  PKMenuItem *item,
									  std::string id)
{
    // Reimplement this when you need it
}

void PKWindow::menuItemClicked(PKMenu *fromMenu,
                               PKMenuItem *item,
                               std::string id)
{
    // Reimplement this when you need it
}

void PKWindow::windowShowing()
{
    // Reimplement this when you need it
}

void PKWindow::windowHiding()
{
    // Reimplement this when you need it
}

void PKWindow::windowClosed()
{
    // Reimplement this when you need it
}

void PKWindow::windowMoved()
{
#ifdef WIN32

	// We must update our position field privately

	RECT wndRect;
	GetWindowRect(this->window, &wndRect);

	PKVariantRect *s = (PKVariantRect *) this->get(FRAME_RECT_PROPERTY);

	s->_val.x = wndRect.left;
	s->_val.y = wndRect.top;

#endif

#ifdef MACOSX 

	// We must update our position field privately
	
	Rect bounds;
	GetWindowBounds(this->macwindow, kWindowStructureRgn, &bounds);
	
	PKVariantRect *s = (PKVariantRect *) this->get(FRAME_RECT_PROPERTY);
	
	s->_val.x = bounds.left;
	s->_val.y = bounds.top;
	
#endif
}

void PKWindow::windowActivated(bool activated)
{
}

void PKWindow::windowResized()
{
#ifdef WIN32

    // We must update our size field privately

	RECT wndRect;
	GetWindowRect(this->window, &wndRect);

	PKVariantRect *s = (PKVariantRect *) this->get(FRAME_RECT_PROPERTY);

	s->_val.w = wndRect.right  - wndRect.left;
	s->_val.h = wndRect.bottom - wndRect.top;

#endif
	
#ifdef MACOSX 

	Rect bounds;
	GetWindowBounds(this->macwindow, kWindowStructureRgn, &bounds);

	PKVariantRect *s = (PKVariantRect *) this->get(FRAME_RECT_PROPERTY);
	
	s->_val.w = bounds.right  - bounds.left;
	s->_val.h = bounds.bottom - bounds.top;
	
#endif
}

void PKWindow::windowMinimized()
{
    // EMPTY
}

void PKWindow::windowMaximized()
{
    // EMPTY
}

void PKWindow::windowResizing(PKDirection dir,
							  PKRect *rect)
{
}

void PKWindow::windowGetMinMax(uint32_t *minWidth,
							   uint32_t *minHeight,
							   uint32_t *maxWidth,
							   uint32_t *maxHeight)
{
}

void PKWindow::displayChange()
{
}

//
// Mouse events
//
// Sent by the browser when the mouse
// hovers, or is down/up as well as for
// double clicks and mouse wheel
//

void PKWindow::mouseDown(PKButtonType button,
					     int x,
					     int y,
					     unsigned int mod)
{
	// Subclass to implement
}


void PKWindow::mouseUp(PKButtonType button,
					   int x,
					   int y,
					   unsigned int mod)
{
	// Subclass to implement
}

void PKWindow::mouseDblClick(PKButtonType button,
						     int x,
						     int y,
						     unsigned int mod)
{
	// Subclass to implement
}

void PKWindow::mouseMove(int x,
					     int y,
					     unsigned int mod)
{
	// Subclass to implement
}

void PKWindow::mouseLeave()
{
	// Subclass to implement
}

void PKWindow::mouseWheel(PKButtonType button,
						  int x,
						  int y,
						  unsigned int mod)
{
	// Subclass to implement
}

//
// keyDown() / keyUp()
//
// Sent when the window has
// the focus and a key is 
// pressed/released.
//

void PKWindow::keyDown(unsigned int modifier,
					   PKKey keyCode,
					   unsigned int virtualCode)
{
	// Subclass to implement
}

void PKWindow::keyUp(unsigned int modifier,
				     PKKey keyCode,
				     unsigned int virtualCode)
{
	// Subclass to implement
}

// IRunnable
//
// This thread is responsible for watching
// mouse movements on the mac and send mouse
// move events to the window
//

#ifdef MACOSX

void PKWindow::runThread2(PKThread *thread)
{
	while(!thread->getExitFlag())
	{
		PKTime::sleep(20);
		
		Point p;
		GetGlobalMouse(&p);

		CGrafPtr oldPort;
		Rect     clip;
    
		GetPort(&oldPort);    
		SetPort(this->window);
    
		GetPortBounds(this->window, &clip);
		SetOrigin(0,0);

		GlobalToLocal(&p);
    
		SetOrigin(clip.left, clip.top);
		SetPort(oldPort);

		int x = p.h;
		int y = p.v;
		
		if(this->lastMouseX == -1 &&
		   this->lastMouseY == -1)
		{
			this->lastMouseX = x;
			this->lastMouseY = y;
		}
		else
		{
			if(x != this->lastMouseX ||
			   y != this->lastMouseY)
			{
				this->lastMouseX = x;
				this->lastMouseY = y;
				
				int w = this->getClientWidth();
				int h = this->getClientHeight();
				
				if(x >= 0 && y >= 0 &&
				   x <= w && y <= h) 
				{
					OSErr	 err;
					EventRef newEvent;
    
					if(CreateEvent(NULL, kEventClassDWP, kEventDWPMouse,
								   0, kEventAttributeNone, &newEvent) != noErr)
					{
						return;
					}
	
					EventTargetRef target = GetWindowEventTarget(this->macwindow);
					Point pt;
					
					pt.h = x;
					pt.v = y;
					
					err = SetEventParameter(newEvent, kEventParamMouseLocation,
											typeQDPoint, sizeof(Point), &pt);
	
					err = SetEventParameter(newEvent, kEventParamPostTarget,
											typeEventTargetRef, sizeof(target), &target);
							
					if (err == noErr) 
					{
						err = PostEventToQueue(GetMainEventQueue(), newEvent, kEventPriorityStandard);
						ReleaseEvent(newEvent);
					}
				}
			}
		}
		
	}
}

#endif

#ifdef LINUX

void PKWindow::gtk_destroy(GtkWidget *widget,
                           gpointer   data)
{
    PKWindow *window = (PKWindow *) data;
    
    if(window != NULL)
    {
        window->windowClosed();
    }
}

gboolean PKWindow::gtk_expose(GtkWidget      *widget,
                              GdkEventExpose *event,
                              gpointer        user_data)
{
    PKWindow *window = (PKWindow *) user_data;
    
    if(window != NULL)
    {
        PKRect rect;
        window->draw(widget, rect);
        return FALSE;
    }

    return FALSE;
}                                


#endif

