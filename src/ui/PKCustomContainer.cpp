//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCustomContainer.h"
#include "PKStr.h"

//
// WIN32 proc
//

#ifdef WIN32

#include <windowsx.h>

LRESULT CALLBACK PKCustomContainer::CustomWinProc(HWND hWnd, 
									  	          UINT msg, 
										          WPARAM wParam, 
										          LPARAM lParam)
{
	PKCustomContainer *ctrl = (PKCustomContainer *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

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

			ctrl->getMouseParams(wParam, lParam, &x, &y, &mod);
			ctrl->mouseUp(PK_BUTTON_LEFT, x, y, mod);

   			ReleaseCapture();
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

	return ctrl->lpOldProc(hWnd, msg, wParam, lParam);
}

void PKCustomContainer::getMouseParams(WPARAM wParam, 
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

std::string PKCustomContainer::MINIMUMWIDTH_INT32_PROPERTY  = "minimumWidth";
std::string PKCustomContainer::MINIMUMHEIGHT_INT32_PROPERTY = "minimumHeight";

//
// PKCustomContainer
//

PKCustomContainer::PKCustomContainer()
{
    PKOBJECT_ADD_INT32_PROPERTY(minimumWidth,  0);
    PKOBJECT_ADD_INT32_PROPERTY(minimumHeight, 0);
	
#ifdef MACOSX
	this->sx = 0;
	this->sy = 0;	
#endif

#ifdef LIUNX
    this->layout = NULL;
#endif
}

PKCustomContainer::~PKCustomContainer()
{
#ifdef WIN32

	if(this->hwnd != NULL)
	{
    	SubclassWindow(this->hwnd, (WNDPROC) this->lpOldProc);
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

void PKCustomContainer::build()
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

    this->hwnd = CreateWindow(L"STATIC", L"",
							  WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY | WS_CLIPCHILDREN, 0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
                              hInst, 0);

    if(this->hwnd)
    {
        this->lpOldProc = SubclassWindow(this->hwnd, (WNDPROC) CustomWinProc);
    	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID) (PKCustomContainer *) this);
    }

#endif

#ifdef LINUX


    this->widget = gtk_event_box_new();
    this->layout = gtk_layout_new(NULL, NULL);

    if(this->widget != NULL && this->layout != NULL)
    {
        // Setup
        
        gtk_widget_show(this->layout);
        gtk_container_add(GTK_CONTAINER(this->widget), this->layout);
    
        // Connect to events

        gtk_widget_set_events(this->widget, GDK_ALL_EVENTS_MASK);
    
        g_signal_connect (G_OBJECT(this->layout), "expose_event",
                          G_CALLBACK (PKCustomContainer::gtk_expose_event_callback), this);        

        g_signal_connect(G_OBJECT(this->widget), "motion-notify-event",
                         G_CALLBACK(PKCustomContainer::gtk_motion_notify_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "leave-notify-event",
                         G_CALLBACK(PKCustomContainer::gtk_leave_notify_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "button-press-event",
                         G_CALLBACK(PKCustomContainer::gtk_button_press_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "button-release-event",
                         G_CALLBACK(PKCustomContainer::gtk_button_release_event_callback), this);          

        g_signal_connect(G_OBJECT(this->widget), "key-press-event",
                         G_CALLBACK(PKCustomContainer::gtk_key_press_event_callback), this);  

        g_signal_connect(G_OBJECT(this->widget), "key-release-event",
                         G_CALLBACK(PKCustomContainer::gtk_key_release_event_callback), this);  
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

void PKCustomContainer::drawWin32(PKDrawingDevice device, 
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

void PKCustomContainer::setDesiredWidgetSize(uint32_t w, uint32_t h)
{
    if(this->widget != NULL && this->layout != NULL)
    {
        gtk_widget_set_size_request(this->widget, w, h);
        gtk_widget_set_size_request(this->layout, w, h);
    }
}

void PKCustomContainer::drawLinux(GtkWidget   *widget,
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

gboolean PKCustomContainer::gtk_expose_event_callback(GtkWidget      *widget, 
                                                    GdkEventExpose *event, 
                                                    gpointer        data)
{
    PKCustomContainer *ctrl = (PKCustomContainer *) data;
    PKRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = widget->allocation.width;
    rect.h = widget->allocation.height;
    
    GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state (widget)];
    
    GdkWindow *win = gtk_layout_get_bin_window(GTK_LAYOUT(widget));
    
    ctrl->drawLinux(widget, GDK_DRAWABLE(win), gc, rect);
    
    return FALSE;
}                                

gboolean PKCustomContainer::gtk_leave_notify_event_callback(GtkWidget        *widget,
                                                          GdkEventCrossing *event,
                                                          gpointer          user_data)
{
    PKCustomContainer *ctrl = (PKCustomContainer *) user_data;

    if(ctrl == NULL)
        return TRUE;

    ctrl->mouseLeave();

    return TRUE;
}                                
                                                           
gboolean PKCustomContainer::gtk_motion_notify_event_callback(GtkWidget      *widget,
                                                           GdkEventMotion *event,
                                                           gpointer        user_data)
{
    PKCustomContainer *ctrl = (PKCustomContainer *) user_data;

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
                                                           
gboolean PKCustomContainer::gtk_button_press_event_callback(GtkWidget      *widget,
                                                          GdkEventButton *event,
                                                          gpointer        user_data)
{
    PKCustomContainer *ctrl = (PKCustomContainer *) user_data;

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
                                                          
gboolean PKCustomContainer::gtk_button_release_event_callback(GtkWidget      *widget,
                                                              GdkEventButton *event,
                                                              gpointer        user_data)
{
    PKCustomContainer *ctrl = (PKCustomContainer *) user_data;

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

gboolean PKCustomContainer::gtk_key_press_event_callback(GtkWidget   *widget,
                                                       GdkEventKey *event,
                                                       gpointer     user_data)
{
    PKCustomContainer *ctrl = (PKCustomContainer *) user_data;

    if(ctrl == NULL)
        return FALSE;

    unsigned int mod = 0;
    unsigned int vk  = 0;
    PKKey        key = PK_KEY_UNKNOWN;

    PKUIComputeKeyState(event, &mod, &key, &vk);
    ctrl->keyDown(mod, key, vk);

    return FALSE;
}                                                 

gboolean PKCustomContainer::gtk_key_release_event_callback(GtkWidget   *widget,
                                                         GdkEventKey *event,
                                                         gpointer     user_data)
{
    PKCustomContainer *ctrl = (PKCustomContainer *) user_data;

    if(ctrl == NULL)
        return FALSE;

    unsigned int mod = 0;
    unsigned int vk  = 0;
    PKKey        key = PK_KEY_UNKNOWN;

    PKUIComputeKeyState(event, &mod, &key, &vk);
    ctrl->keyUp(mod, key, vk);    

    // Also, compute the unicode character
    // that was generated by this stroke
    //
    // TODO: use input methods

    return FALSE;
}                                                 

#endif

#ifdef MACOSX

void PKCustomContainer::drawMacOSXInternal(RgnHandle	 inLimitRgn,
										 CGContextRef inContext)
{
	CGContextTranslateCTM(inContext, -this->sx, -this->sy);
	this->drawMacOSX(inLimitRgn, inContext);
}

void PKCustomContainer::drawMacOSX(RgnHandle	 inLimitRgn,
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

int PKCustomContainer::getScrollableWidth()
{
	return this->getW();	
}

int PKCustomContainer::getScrollableHeight()
{
	return this->getH();
}

int PKCustomContainer::getScrollLineWidth()
{
	return this->getW()/10;	
}

int PKCustomContainer::getScrollLineHeight()
{
	return this->getH()/10;
}

void PKCustomContainer::setScrollOrigin(int x, int y)
{
	this->sx = x;
	this->sy = y;
}

#endif

uint32_t PKCustomContainer::getMinimumWidth()
{
    PKVariant *mw = this->get(PKCustomContainer::MINIMUMWIDTH_INT32_PROPERTY);
	return PKVALUE_INT32(mw);
}

uint32_t PKCustomContainer::getMinimumHeight()
{
    PKVariant *mh = this->get(PKCustomContainer::MINIMUMHEIGHT_INT32_PROPERTY);
	return PKVALUE_INT32(mh);
}

//
// Mouse events
//
// Sent by the window when the mouse
// hovers, or is down/up as well as for
// double clicks and mouse wheel
//

void PKCustomContainer::mouseDown(PKButtonType button,
					            int x,
					            int y,
					            unsigned int mod)
{
}

void PKCustomContainer::mouseUp(PKButtonType button,
					          int x,
					          int y,
					          unsigned int mod)
{
}

void PKCustomContainer::mouseDblClick(PKButtonType button,
						            int x,
						            int y,
						            unsigned int mod)
{
}

void PKCustomContainer::mouseMove(int x,
					            int y,
					            unsigned int mod)
{
}

void PKCustomContainer::mouseLeave()
{
}

void PKCustomContainer::mouseWheel(PKButtonType button,
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

bool PKCustomContainer::keyDown(unsigned int modifier,
					          PKKey        keyCode,
					          unsigned int virtualCode)
{
	return false;
}


bool PKCustomContainer::keyUp(unsigned int modifier,
				            PKKey        keyCode,
				            unsigned int virtualCode)
{
	return false;
}

void PKCustomContainer::characterInput(std::wstring wstr)
{
}

