//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCustomControl.h"
#include "PKStr.h"
#include "PKEdit.h"
#include "PKOS.h"
#include "PKDialog.h"

//
// WIN32 proc
//

#ifdef WIN32

#include <windowsx.h>

void PKCustomControlChildrenWalk(PKObject *child, void *context)
{
    std::vector<RECT> *rects = (std::vector<RECT> *) context;
    PKEdit *edit = dynamic_cast<PKEdit*>(child);

    if(edit)
    {
        HWND hwnd = edit->getWindowHandle();

        RECT client;
        GetWindowRect(hwnd, &client);

        if(edit->isShowing())
        {
            rects->push_back(client);
        }
    }
}

LRESULT CALLBACK PKCustomControl::CustomWinProc(HWND hWnd, 
									  	        UINT msg, 
										        WPARAM wParam, 
										        LPARAM lParam)
{
	PKCustomControl *ctrl = (PKCustomControl *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(ctrl == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

    bool hit = false;

    for(uint32_t i=0; i<ctrl->getChildrenCount(); i++)
    {
        PKObject *child = ctrl->getChildAt(i);

        if(child)
        {
            PKControl *control = dynamic_cast<PKControl*>(child);

            if(control)
            {
                hit = control->relayWIN32Event(hWnd, 
                                               msg, 
                                               wParam, 
                                               lParam);

                if(hit && (msg != WM_PAINT && 
                           msg != WM_PRINTCLIENT))
                {
                    break;
                }
            }
        }
    }

    switch(msg)
	{
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);

            // Setup clipping of the DC, for any
            // child controls that we have that are
            // Edit controls since it seems to be
            // messed up (erases the edit's border)

            WindowsVersion winver = PKOS::getWindowsVersion();

            //if(winver == WIN_XP)
            {
                std::vector<RECT> clientRects;
                ctrl->walkChildren(PKCustomControlChildrenWalk, &clientRects);

                if(clientRects.size() > 0)
                {
                    RECT ours;
                    GetWindowRect(hWnd, &ours);
                                    
                    RECT c;
                    GetClientRect(hWnd, &c);
                    int w = c.right - c.left;
                    int h = c.bottom - c.top;

                    HRGN fullRegion = CreateRectRgn(0, 0, w, h);

                    for(uint32_t i=0; i < clientRects.size(); i++)
                    {
                        RECT client = clientRects[i];

                        HRGN region = CreateRectRgn(client.left - ours.left,
                                                    client.top - ours.top,
                                                    client.right - ours.left,
                                                    client.bottom - ours.top); 

                        CombineRgn(fullRegion, fullRegion, region, RGN_DIFF);
                        DeleteObject(region);
                    }
        
                    SelectClipRgn(ps.hdc, NULL);
                    SelectClipRgn(ps.hdc, fullRegion);

                    DeleteObject(fullRegion);
                }
            }

            PKRect rect = PKRectCreate(ps.rcPaint.left, 
                                       ps.rcPaint.top, 
                                      (ps.rcPaint.right - ps.rcPaint.left), 
                                      (ps.rcPaint.bottom - ps.rcPaint.top));
            ctrl->drawWin32(ps.hdc, rect);
            EndPaint(hWnd, &ps);

            for(uint32_t i=0; i<ctrl->getChildrenCount(); i++)
            {
                PKObject *child = ctrl->getChildAt(i);

                if(child)
                {
                    PKControl *control = dynamic_cast<PKControl*>(child);

                    if(control)
                    {
                        hit = control->relayWIN32Event(hWnd, 
                                                       msg, 
                                                       wParam, 
                                                       lParam);
                    }
                }
            }
            
            return 0;
        }
        break;

    case WM_PRINTCLIENT:
        {
            HDC hdc = (HDC) wParam;
            PKRect rect = PKRectCreate(0, 0, ctrl->getW(), ctrl->getH());
            ctrl->drawWin32(hdc, rect);
        }
        break;

    // Keyboard

    case WM_GETDLGCODE:
        {
            return DLGC_WANTCHARS|DLGC_WANTARROWS|DLGC_WANTALLKEYS;
        }
        break;
    
    case WM_KEYDOWN:
    case WM_KEYUP:
        {
			unsigned int modifier  = PK_MOD_NONE;
			PKKey key = PK_KEY_UNKNOWN;
			unsigned int vcode = 0;

			PKUIComputeKeyState(wParam, &modifier, &key, &vcode);

			if(msg == WM_KEYDOWN)
			{
                // Handle the TAB key

                if(key == PK_KEY_TAB)
                {
                    if(modifier & PK_MOD_SHIFT)
                    {
                        bool prevOK = ctrl->moveFocusToPreviousControlPart();

                        if(!prevOK)
                        {
                            HWND hDlg;
                            hDlg = GetAncestor(hWnd, GA_ROOT);

                            // Move away
                            HWND next = GetNextDlgTabItem(hDlg, hWnd, TRUE);

                            if(next != NULL)
                            {
                                SetFocus(next);
                            }
                        }
                    }
                    else
                    {
                        bool nextOK = ctrl->moveFocusToNextControlPart();

                        if(!nextOK)
                        {
                            HWND hDlg;
                            hDlg = GetAncestor(hWnd, GA_ROOT);

                            // Move away
                            HWND next = GetNextDlgTabItem(hDlg, hWnd, FALSE);

                            if(next != NULL)
                            {
                                SetFocus(next);
                            }
                        }
                    }
                }

                // And forward

				ctrl->keyDown(modifier, key, vcode);
			}

			if(msg == WM_KEYUP)
			{
				ctrl->keyUp(modifier, key, vcode);
			}

			return 0;
        }
        break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        {
			unsigned int modifier  = PK_MOD_NONE;
			PKKey key = PK_KEY_UNKNOWN;
			unsigned int vcode = 0;

			PKUIComputeKeyState(wParam, &modifier, &key, &vcode);
			modifier |= PK_MOD_ALT;

			if(msg == WM_SYSKEYDOWN)
			{
				ctrl->keyDown(modifier, key, vcode);
			}

			if(msg == WM_SYSKEYUP)
			{
				ctrl->keyUp(modifier, key, vcode);
			}

			return 0;
        }
        break;

    case WM_CHAR:
        {
            // Get the character and call our function
			std::wstring wc = std::wstring(1, (wchar_t) wParam);
            ctrl->characterInput(wc);
            return 0;
        }
        break;
    
    // Mouse

	case WM_LBUTTONDOWN:
		{
			int x;
			int y;
			unsigned int mod;

            if(!hit)
            {
    			SetCapture(hWnd);
            }

			ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
			ctrl->mouseDown(PK_BUTTON_LEFT, x, y, mod);
        }
		break;

	case WM_RBUTTONDOWN:
		{
			int x;
			int y;
			unsigned int mod;

			ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
			ctrl->mouseDown(PK_BUTTON_RIGHT, x, y, mod);
        }
		break;

	case WM_LBUTTONUP:
		{
			int x;
			int y;
			unsigned int mod;

   			ReleaseCapture();

            ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
			ctrl->mouseUp(PK_BUTTON_LEFT, x, y, mod);
		}
		break;

	case WM_RBUTTONUP:
		{
			int x;
			int y;
			unsigned int mod;

			ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
			ctrl->mouseUp(PK_BUTTON_RIGHT, x, y, mod);
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			int x;
			int y;
			unsigned int mod;

			ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
			ctrl->mouseDblClick(PK_BUTTON_LEFT, x, y, mod);
		}
		break;

	case WM_RBUTTONDBLCLK:
		{
			int x;
			int y;
			unsigned int mod;

			ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
			ctrl->mouseDblClick(PK_BUTTON_RIGHT, x, y, mod);
		}
		break;

	case WM_MOUSEMOVE:
		{
			int x;
			int y;
			unsigned int mod;

			ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
            ctrl->mouseMove(x, y, mod);

            if(!ctrl->dontTrack)
            {
                TRACKMOUSEEVENT tme;

    	        tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;

	            _TrackMouseEvent(&tme);
            }

            if(!hit)
            {
                // We must call WM_MOUSELEAVE
                // on our children to be safe
                // since they won't be tracking the mouse

                for(uint32_t i=0; i<ctrl->getChildrenCount(); i++)
                {
                    PKObject *child = ctrl->getChildAt(i);

                    if(child)
                    {
                        PKControl *control = dynamic_cast<PKControl*>(child);

                        if(control)
                        {
                            hit = control->relayWIN32Event(hWnd, 
                                                           WM_MOUSELEAVE, 
                                                           0, 0);

                            if(hit)
                            {
                                break;
                            }
                        }
                    }
                }
            }
		}
		break;

	case WM_MOUSELEAVE:
		{
			ctrl->mouseLeave();
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void PKCustomControl::getMouseParams(WPARAM wParam, 
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

#endif

//
// PROPERTIES
// 

std::string PKCustomControl::MINIMUMWIDTH_INT32_PROPERTY  = "minimumWidth";
std::string PKCustomControl::MINIMUMHEIGHT_INT32_PROPERTY = "minimumHeight";
std::string PKCustomControl::WANTFOCUS_BOOL_PROPERTY      = "wantFocus";

//
// PKCustomControl
//

PKCustomControl::PKCustomControl()
{
    PKOBJECT_ADD_INT32_PROPERTY(minimumWidth,  0);
    PKOBJECT_ADD_INT32_PROPERTY(minimumHeight, 0);
	PKOBJECT_ADD_BOOL_PROPERTY(wantFocus, false);
	
#ifdef MACOSX
	this->sx = 0;
	this->sy = 0;	
#endif

#ifdef LINUX
	this->draw_area = NULL;
#endif
}

PKCustomControl::~PKCustomControl()
{
#ifdef WIN32

	if(this->hwnd != NULL)
	{
		DestroyWindow(this->hwnd);
	}

#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		DisposeControl(this->controlRef);
	}
	
#endif
}

void PKCustomControl::build()
{
	PKObject *parent = this->getParent();

	if(parent == NULL)
	{
		return;
	}

	PKView *parentView = dynamic_cast<PKView*>(parent);

	if(parentView == NULL)
	{
		return;
	}

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    // Register a new custom window class

    WNDCLASSEX wclass;

	wclass.cbSize	     = sizeof(WNDCLASSEX);
	wclass.style	     = CS_DBLCLKS;
	wclass.lpfnWndProc   = PKCustomControl::CustomWinProc;
	wclass.cbClsExtra    = 0;
	wclass.cbWndExtra    = 0;
	wclass.hInstance     = hInst;
	wclass.hIcon         = NULL;
	wclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wclass.hbrBackground = NULL;
	wclass.lpszMenuName  = NULL;
	wclass.hIconSm       = NULL;
	
	std::wstring className = L"PKCUSTOMCONTROL_" + PKStr::uint32ToWstring((uint32_t)PKCustomControl::CustomWinProc);
	wclass.lpszClassName = className.c_str(); 

	DWORD result = RegisterClassEx(&wclass);

    // Now create the control

    DWORD flags = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN /*| WS_CLIPSIBLINGS don't add back */;

    PKVariant *f   = this->get(PKCustomControl::WANTFOCUS_BOOL_PROPERTY);
    bool wantFocus = PKVALUE_BOOL(f);

    if(wantFocus)
    {
        flags |= WS_TABSTOP;
    }

    this->hwnd = CreateWindow(className.c_str(), L"",
							  flags, 
                              0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
                              hInst, 0);

    if(this->hwnd)
    {
    	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID) (PKCustomControl *) this);
    }

#endif

#ifdef LINUX

    PKVariant *mw = this->get(PKCustomControl::MINIMUMWIDTH_INT32_PROPERTY);
	int32_t mmw = PKVALUE_INT32(mw);

    PKVariant *mh = this->get(PKCustomControl::MINIMUMHEIGHT_INT32_PROPERTY);
	int32_t mmh = PKVALUE_INT32(mh);

    this->draw_area = gtk_drawing_area_new();
    this->widget = gtk_event_box_new();

    if(this->widget != NULL && this->draw_area != NULL)
    {
        PKVariant *f   = this->get(PKCustomControl::WANTFOCUS_BOOL_PROPERTY);
        bool wantFocus = PKVALUE_BOOL(f);

        gtk_widget_set_can_focus(this->widget, wantFocus);

        // Show

        gtk_widget_show(this->draw_area);
        gtk_container_add(GTK_CONTAINER(this->widget), this->draw_area);

        // Setup
        
        gtk_widget_set_size_request(this->widget, mmw, mmh);
        gtk_widget_set_size_request(this->draw_area, mmw, mmh);
    
        gtk_widget_set_events(this->widget, GDK_ALL_EVENTS_MASK);

        // Connect
    
        g_signal_connect(G_OBJECT(this->draw_area), "expose_event",
                         G_CALLBACK (PKCustomControl::gtk_expose_event_callback), this);      
                        
        g_signal_connect(G_OBJECT(this->widget), "motion-notify-event",
                         G_CALLBACK(PKCustomControl::gtk_motion_notify_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "leave-notify-event",
                         G_CALLBACK(PKCustomControl::gtk_leave_notify_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "button-press-event",
                         G_CALLBACK(PKCustomControl::gtk_button_press_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "button-release-event",
                         G_CALLBACK(PKCustomControl::gtk_button_release_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "key-press-event",
                         G_CALLBACK(PKCustomControl::gtk_key_press_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "key-release-event",
                         G_CALLBACK(PKCustomControl::gtk_key_release_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "focus",
                         G_CALLBACK(PKCustomControl::gtk_focus_event_callback), this);

        g_signal_connect(G_OBJECT(this->widget), "keynav-failed",
                         G_CALLBACK(PKCustomControl::gtk_keynav_failed_event_callback), this);
    }
    

#endif

#ifdef MACOSX

	WindowRef parentW = GetWindowFromPort(parentView->getWindowHandle());
	
	HIRect rect;
	
	rect.origin.x = 0;
	rect.origin.y = 0;
	rect.size.width   = 100;
	rect.size.height   = 100;
	
	OSStatus err = HIPKMacCustomViewCreate(parentW,
										   &rect,
										   this,
										   &this->controlRef);
	
	if(err == noErr)
	{
		
		HIViewSetVisible(this->controlRef, true);
	}
        
#endif
}

#ifdef WIN32

void PKCustomControl::drawWin32(PKDrawingDevice device, 
                                PKRect          rect)
{
    // Subclass will implement
#ifdef _DEBUG
    
    HBRUSH brush = CreateSolidBrush(RGB(128, 128, 128));
    
    RECT r;
    r.left   = rect.x;
    r.top    = rect.y;
    r.right  = rect.x + rect.w;
    r.bottom = rect.y + rect.h;

    FillRect(device, &r, brush);
    DeleteObject(brush);

#endif
}

#endif

#ifdef LINUX

void PKCustomControl::setDesiredWidgetSize(uint32_t w, uint32_t h)
{
    if(this->widget != NULL && this->draw_area != NULL)
    {
        gtk_widget_set_size_request(this->draw_area, w, h);
        gtk_widget_set_size_request(this->widget, w, h);
    }
}

void PKCustomControl::drawLinux(GtkWidget   *widget,
                                GdkDrawable *drawable,
                                GdkGC       *gc,
                                PKRect       rect)    
{
    // Subclass will implement

    GdkColor old;
    GdkColor fg;
    fg.red   = 32768;
    fg.green = 32768;
    fg.blue  = 32768;

    GdkGCValues values;
    gdk_gc_get_values(gc, &values);

    gdk_gc_set_rgb_fg_color(gc, &fg);

    gdk_draw_rectangle(drawable,
                       gc, TRUE, 
                       rect.x, rect.y, rect.w, rect.h);                               

    gdk_gc_set_foreground(gc, &values.foreground);    
}                                
#endif

#ifdef LINUX

gboolean PKCustomControl::gtk_expose_event_callback(GtkWidget      *widget, 
                                                    GdkEventExpose *event, 
                                                    gpointer        data)
{
    PKCustomControl *ctrl = (PKCustomControl *) data;
    PKRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = widget->allocation.width;
    rect.h = widget->allocation.height;
    
    GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state (widget)];
    
    ctrl->drawLinux(widget, widget->window, gc, rect);

    return TRUE;
}                                

gboolean PKCustomControl::gtk_leave_notify_event_callback(GtkWidget        *widget,
                                                          GdkEventCrossing *event,
                                                          gpointer          user_data)
{
    PKCustomControl *ctrl = (PKCustomControl *) user_data;

    if(ctrl == NULL)
        return TRUE;

    ctrl->mouseLeave();

    return TRUE;
}                                
                                                           
gboolean PKCustomControl::gtk_motion_notify_event_callback(GtkWidget      *widget,
                                                           GdkEventMotion *event,
                                                           gpointer        user_data)
{
    PKCustomControl *ctrl = (PKCustomControl *) user_data;

    if(ctrl == NULL)
        return TRUE;

    unsigned int mod = 0;
    
    if(event->state & GDK_SHIFT_MASK)
        mod |= PK_MOD_SHIFT;

    if(event->state & GDK_CONTROL_MASK)
        mod |= PK_MOD_CONTROL;

    if(event->state & GDK_MOD1_MASK)
        mod |= PK_MOD_ALT;

    ctrl->mouseMove(event->x, event->y, mod);

    return TRUE;
}                                
                                                           
gboolean PKCustomControl::gtk_button_press_event_callback(GtkWidget      *widget,
                                                          GdkEventButton *event,
                                                          gpointer        user_data)
{
    PKCustomControl *ctrl = (PKCustomControl *) user_data;

    if(ctrl == NULL)
        return TRUE;
        
    PKButtonType button = PK_BUTTON_LEFT;
    
    if(event->button == 3)
        button = PK_BUTTON_RIGHT;

    unsigned int mod = 0;
    
    if(event->state & GDK_SHIFT_MASK)
        mod |= PK_MOD_SHIFT;

    if(event->state & GDK_CONTROL_MASK)
        mod |= PK_MOD_CONTROL;

    if(event->state & GDK_MOD1_MASK)
        mod |= PK_MOD_ALT;

    ctrl->mouseDown(button, event->x, event->y, mod);

    return TRUE;
}                                
                                                          
gboolean PKCustomControl::gtk_button_release_event_callback(GtkWidget      *widget,
                                                            GdkEventButton *event,
                                                            gpointer        user_data)
{
    PKCustomControl *ctrl = (PKCustomControl *) user_data;

    if(ctrl == NULL)
        return TRUE;
        
    PKButtonType button = PK_BUTTON_LEFT;
    
    if(event->button == 3)
        button = PK_BUTTON_RIGHT;

    unsigned int mod = 0;
    
    if(event->state & GDK_SHIFT_MASK)
        mod |= PK_MOD_SHIFT;

    if(event->state & GDK_CONTROL_MASK)
        mod |= PK_MOD_CONTROL;

    if(event->state & GDK_MOD1_MASK)
        mod |= PK_MOD_ALT;

    ctrl->mouseUp(button, event->x, event->y, mod);

    return TRUE;
}                                

gboolean PKCustomControl::gtk_key_press_event_callback(GtkWidget   *widget,
                                                       GdkEventKey *event,
                                                       gpointer     user_data)
{
    PKCustomControl *ctrl = (PKCustomControl *) user_data;

    if(ctrl == NULL)
        return FALSE;

    unsigned int mod = 0;
    unsigned int vk  = 0;
    PKKey        key = PK_KEY_UNKNOWN;

    PKUIComputeKeyState(event, &mod, &key, &vk);
    return ctrl->keyDown(mod, key, vk);
}                                                 

gboolean PKCustomControl::gtk_key_release_event_callback(GtkWidget   *widget,
                                                         GdkEventKey *event,
                                                         gpointer     user_data)
{
    PKCustomControl *ctrl = (PKCustomControl *) user_data;

    if(ctrl == NULL)
        return FALSE;

    unsigned int mod = 0;
    unsigned int vk  = 0;
    PKKey        key = PK_KEY_UNKNOWN;

    PKUIComputeKeyState(event, &mod, &key, &vk);
    return ctrl->keyUp(mod, key, vk);    
}                                                 

gboolean PKCustomControl::gtk_focus_event_callback(GtkWidget       *widget,
                                                   GtkDirectionType direction,
                                                   gpointer         user_data)
{
    PKCustomControl *ctrl = (PKCustomControl *) user_data;

    if(ctrl == NULL)
    {
        return FALSE;
    }
    
    PKVariant *f   = ctrl->get(PKCustomControl::WANTFOCUS_BOOL_PROPERTY);
    bool wantFocus = PKVALUE_BOOL(f);

    if(!wantFocus)
    {
        return FALSE;
    }
    
    if(direction == GTK_DIR_TAB_FORWARD)
    {
        if(ctrl->hasFocus())
        {
            if(ctrl->moveFocusToNextControlPart())
            {
                return TRUE;
            }

            return FALSE;
        }
        else
        {
            ctrl->focusDirection = PK_DIR_FORWARD;
            gtk_widget_grab_focus(widget);
            return TRUE;
        }
    }

    if(direction == GTK_DIR_TAB_BACKWARD)
    {
        if(ctrl->hasFocus())
        {
            if(ctrl->moveFocusToPreviousControlPart())
            {
                return TRUE;
            }

            return FALSE;
        }
        else
        {
            ctrl->focusDirection = PK_DIR_BACKWARD;
            gtk_widget_grab_focus(widget);
            return TRUE;
        }
    }

    return FALSE;
}

gboolean PKCustomControl::gtk_keynav_failed_event_callback(GtkWidget       *widget,
                                                           GtkDirectionType direction,
                                                           gpointer         user_data)
{
    // We are OK to loose focus
    return FALSE;
}                                                           


#endif

#ifdef MACOSX

void PKCustomControl::drawMacOSXInternal(RgnHandle	 inLimitRgn,
										 CGContextRef inContext)
{
	CGContextTranslateCTM(inContext, -this->sx, -this->sy);
	this->drawMacOSX(inLimitRgn, inContext);
}

void PKCustomControl::drawMacOSX(RgnHandle	 inLimitRgn,
								 CGContextRef inContext)
{
    // Subclass will implement
#ifdef _DEBUG

	CGRect  rect;
	float   grey[4] = {0.5, 0.5, 0.5, 1};
	
	rect.origin.x = 0;
	rect.origin.y = 0;
	
	rect.size.width  = this->getW();
	rect.size.height = this->getH();
	
	CGContextSetFillColor( inContext, grey);
	CGContextFillRect(inContext, rect);
	
#endif
}

int PKCustomControl::getScrollableWidth()
{
	return this->getW();	
}

int PKCustomControl::getScrollableHeight()
{
	return this->getH();
}

int PKCustomControl::getScrollLineWidth()
{
	return this->getW()/10;	
}

int PKCustomControl::getScrollLineHeight()
{
	return this->getH()/10;
}

void PKCustomControlWalkChildren(PKObject *child, void *context)
{
	PKCustomControl *parent = (PKCustomControl *) context;
	PKCustomControl *childc = dynamic_cast<PKCustomControl *> (child);
	
	if(childc != NULL)
	{
		childc->sx = parent->getScrollOriginX();
		childc->sy = parent->getScrollOriginY();
	}
}

void PKCustomControl::setScrollOrigin(int x, int y)
{
	this->sx = x;
	this->sy = y;
	
	this->walkChildren(PKCustomControlWalkChildren, this);
}

int PKCustomControl::getScrollOriginX()
{
	return this->sx;
}

int PKCustomControl::getScrollOriginY()
{
	return this->sy;
}

#endif

uint32_t PKCustomControl::getMinimumWidth()
{
	if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
		return this->getW();
	}
	
    PKVariant *mw = this->get(PKCustomControl::MINIMUMWIDTH_INT32_PROPERTY);
	return PKVALUE_INT32(mw);
}

uint32_t PKCustomControl::getMinimumHeight()
{
	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

    PKVariant *mh = this->get(PKCustomControl::MINIMUMHEIGHT_INT32_PROPERTY);
	return PKVALUE_INT32(mh);
}

//
// Mouse events
//
// Sent by the window when the mouse
// hovers, or is down/up as well as for
// double clicks and mouse wheel
//

void PKCustomControl::mouseDown(PKButtonType button,
					            int x,
					            int y,
					            unsigned int mod)
{
}

void PKCustomControl::mouseUp(PKButtonType button,
					          int x,
					          int y,
					          unsigned int mod)
{
}

void PKCustomControl::mouseDblClick(PKButtonType button,
						            int x,
						            int y,
						            unsigned int mod)
{
}

void PKCustomControl::mouseMove(int x,
					            int y,
					            unsigned int mod)
{
}

void PKCustomControl::mouseLeave()
{
}

void PKCustomControl::mouseWheel(PKButtonType button,
					             int x,
					             int y,
					             unsigned int mod)
{
}

//
// keyDown() / keyUp()
//
// Sent when the window has
// the focus and a key is 
// pressed/released.
//

bool PKCustomControl::keyDown(unsigned int modifier,
					          PKKey        keyCode,
					          unsigned int virtualCode)
{
    return false;
}


bool PKCustomControl::keyUp(unsigned int modifier,
				            PKKey        keyCode,
				            unsigned int virtualCode)
{
    return false;
}

void PKCustomControl::characterInput(std::wstring wstr)
{

}

bool PKCustomControl::moveFocusToNextControlPart()
{
	// No next part by default
	return false;
}

bool PKCustomControl::moveFocusToPreviousControlPart()
{
	// No previous part by default
	return false;
}

// From PKObject
void PKCustomControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKControl::ENABLED_BOOL_PROPERTY)
	{
		this->invalidate();
	}

	PKControl::selfPropertyChanged(prop, oldValue);
}



