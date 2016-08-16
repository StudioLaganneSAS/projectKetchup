//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKScrollView.h"
#include "PKLayout.h"
#include "PKImageView.h"

#ifdef WIN32

#include <windowsx.h>

#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wparam) ((short)HIWORD (wparam))
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

LRESULT CALLBACK PKScrollView::CustomWinProc(HWND hWnd, 
										     UINT msg, 
										     WPARAM wParam, 
										     LPARAM lParam)
{
	PKScrollView *ctrl = (PKScrollView *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(ctrl == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

    switch(msg)
    {
    case WM_LBUTTONDOWN:
        {
            // TODO: WIN32: We don't catch mouse clicks of our children but we should
            // Find a wa to do so, as we need to put focus back on ourselves
            // Maybe do this from PKDialog ?

            int i=0;
            i++;
        }
        break;

	case WM_MOUSEWHEEL:
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if(zDelta > 0)
			{
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int y      = root->getY();
                    int h      = ctrl->getH();
                    int height = root->getH();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_VERT, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos-si.nPage/10;

                    if(newVal < 0)
                    {
                        newVal = 0;
                    }

                    int pos = (newVal)*(height-h+off)/(1000-si.nPage);
                    root->moveBy(0, -y-pos);
#ifdef WIN32
					ctrl->invalidate();
#endif
                    SetScrollPos(hWnd, SB_VERT, newVal, TRUE);

                    return 0;
                }				
			}
			else
			{
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int y      = root->getY();
                    int h      = ctrl->getH();
                    int height = root->getH();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_VERT, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos+si.nPage/10;

                    if(newVal > (int) ((int) si.nMax-(int) si.nPage))
                    {
                        newVal = (si.nMax-si.nPage);
                    }

                    int pos = (newVal)*(height-h+off)/(1000-si.nPage);
                    root->moveBy(0, -y-pos);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_VERT, newVal, TRUE);

                    return 0;
                }				
			}
		}
		break;

    case WM_VSCROLL:
        {
			if(LOWORD(wParam) == SB_LINEDOWN)
			{
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int y      = root->getY();
                    int h      = ctrl->getH();
                    int height = root->getH();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_VERT, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos+si.nPage/10;

                    if(newVal > (int) ((int) si.nMax-(int) si.nPage))
                    {
                        newVal = (si.nMax-si.nPage);
                    }

                    int pos = (newVal)*(height-h+off)/(1000-si.nPage);
                    root->moveBy(0, -y-pos);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_VERT, newVal, TRUE);

                    return 0;
                }
			}

			if(LOWORD(wParam) == SB_LINEUP)
			{
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int y      = root->getY();
                    int h      = ctrl->getH();
                    int height = root->getH();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_VERT, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos-si.nPage/10;

                    if(newVal < 0)
                    {
                        newVal = 0;
                    }

                    int pos = (newVal)*(height-h+off)/(1000-si.nPage);
                    root->moveBy(0, -y-pos);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_VERT, newVal, TRUE);

                    return 0;
                }
			}

            if(LOWORD(wParam) == SB_PAGEDOWN)
            {
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int y      = root->getY();
                    int h      = ctrl->getH();
                    int height = root->getH();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_VERT, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos+si.nPage;

                    if(newVal > (int) ((int) si.nMax-(int) si.nPage))
                    {
                        newVal = (si.nMax-si.nPage);
                    }

                    int pos = (newVal)*(height-h+off)/(1000-si.nPage);
                    root->moveBy(0, -y-pos);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_VERT, newVal, TRUE);

                    return 0;
                }
            }

            if(LOWORD(wParam) == SB_PAGEUP)
            {
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int y      = root->getY();
                    int h      = ctrl->getH();
                    int height = root->getH();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_VERT, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos-si.nPage;

                    if(newVal < 0)
                    {
                        newVal = 0;
                    }

                    int pos = (newVal)*(height-h+off)/(1000-si.nPage);
                    root->moveBy(0, -y-pos);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_VERT, newVal, TRUE);

                    return 0;
                }
            }

            if(LOWORD(wParam) == SB_THUMBPOSITION ||
               LOWORD(wParam) == SB_THUMBTRACK)
            {
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int y      = root->getY();
                    int h      = ctrl->getH();
                    int height = root->getH();
                    int value  = HIWORD(wParam);

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_VERT, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int pos = (value)*(height-h+off)/(1000-si.nPage);
                    root->moveBy(0, -y-pos);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_VERT, value, TRUE);

                    return 0;
                }
            }
        }
        break;

    case WM_HSCROLL:
        {
			if(LOWORD(wParam) == SB_LINEDOWN)
			{
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.right - window.left) - (client.right - client.left);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int x      = root->getX();
                    int w      = ctrl->getW();
                    int width  = root->getW();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_HORZ, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos+si.nPage/10;

                    if(newVal > (int) ((int) si.nMax-(int)si.nPage))
                    {
                        newVal = (si.nMax-si.nPage);
                    }

                    int pos = (newVal)*(width-w+off)/(1000-si.nPage);
                    root->moveBy(-x-pos, 0);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_HORZ, newVal, TRUE);

                    return 0;
                }
			}
			
			if(LOWORD(wParam) == SB_LINEUP)
			{
			                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int x      = root->getX();
                    int w      = ctrl->getW();
                    int width  = root->getW();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_HORZ, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos-si.nPage/10;

                    if(newVal < 0)
                    {
                        newVal = 0;
                    }

                    int pos = (newVal)*(width-w+off)/(1000-si.nPage);
                    root->moveBy(-x-pos, 0);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_HORZ, newVal, TRUE);

                    return 0;
                }
			}

			if(LOWORD(wParam) == SB_PAGEDOWN)
            {
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.right - window.left) - (client.right - client.left);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int x      = root->getX();
                    int w      = ctrl->getW();
                    int width  = root->getW();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_HORZ, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos+si.nPage;

                    if(newVal > (int) ((int) si.nMax-(int)si.nPage))
                    {
                        newVal = (si.nMax-si.nPage);
                    }

                    int pos = (newVal)*(width-w+off)/(1000-si.nPage);
                    root->moveBy(-x-pos, 0);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_HORZ, newVal, TRUE);

                    return 0;
                }
            }

            if(LOWORD(wParam) == SB_PAGEUP)
            {
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.bottom - window.top) - (client.bottom - client.top);

		        PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int x      = root->getX();
                    int w      = ctrl->getW();
                    int width  = root->getW();

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_HORZ, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int newVal = si.nPos-si.nPage;

                    if(newVal < 0)
                    {
                        newVal = 0;
                    }

                    int pos = (newVal)*(width-w+off)/(1000-si.nPage);
                    root->moveBy(-x-pos, 0);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_HORZ, newVal, TRUE);

                    return 0;
                }
            }

            if(LOWORD(wParam) == SB_THUMBPOSITION ||
               LOWORD(wParam) == SB_THUMBTRACK)
            {
                RECT client;
                RECT window;
                GetClientRect(hWnd, &client);
                GetWindowRect(hWnd, &window);

                int off = (window.right - window.left) - (client.right - client.left);

                PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

		        if(root)
		        {
                    int x      = root->getX();
                    int w      = ctrl->getW();
                    int width  = root->getW();
                    int value  = HIWORD(wParam);

                    SCROLLINFO si = {sizeof(SCROLLINFO), 
                                     SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS, 0, 0, 0, 0, 0};

                    GetScrollInfo (hWnd, SB_HORZ, &si);

                    if(si.nPage == 1000)
                    {
                        return 0;
                    }

                    int pos = (value)*(width-w+off)/(1000-si.nPage);
                    
                    root->moveBy(-x-pos, 0);
#ifdef WIN32
					ctrl->invalidate();
#endif

                    SetScrollPos(hWnd, SB_HORZ, value, TRUE);

                    return 0;
                }
            }
        }
        break;

	case WM_DRAWITEM:
		{
			// This is called for owner drawn
			// static controls such as the 
			// DWPImage class when it needs to
			// have an alpha channel...

			DRAWITEMSTRUCT *drawStruct = (DRAWITEMSTRUCT *) lParam;

			if(drawStruct != NULL)
			{
				if(ctrl->getChildrenCount() == 1)
				{
					PKControl *root = dynamic_cast<PKControl*>(ctrl->getChildAt(0));

					if(root)
					{
						PKView *obj = root->findObjectByWindow(drawStruct->hwndItem);

						if(obj != NULL)
						{
							if(dynamic_cast<PKImageView*>(obj))
							{
								PKImageView *img = dynamic_cast<PKImageView*>(obj);

								HBRUSH brush = NULL;
                                
                                // See if the image is a child of a control
                                // that draws its background 

                                if(!img->parentDrawsItsBackground())
                                {
                                    brush = (HBRUSH) ctrl->getBackgroundBrush();
                                }

								// Draw the image

								img->alphaDraw(drawStruct->hDC, 
											   drawStruct->rcItem, 
											   brush);
								
								return TRUE;
							}
						}
					}
				}
			}
		}
		break;

    }

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif

//
// Properties
//

std::string PKScrollView::BACKGROUND_COLOR_PROPERTY   = "backgroundColor";
std::string PKScrollView::VSCROLL_BOOL_PROPERTY       = "vScroll";
std::string PKScrollView::HSCROLL_BOOL_PROPERTY       = "hScroll";
std::string PKScrollView::MACMARGIN_INT32_PROPERTY    = "macMargin";

//
// PKScrollView
//

PKScrollView::PKScrollView()
{
    PKColor b = PKColorCreate(255, 255, 255, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(backgroundColor, b);
    PKOBJECT_ADD_BOOL_PROPERTY(vScroll, false);
    PKOBJECT_ADD_BOOL_PROPERTY(hScroll, false);
    PKOBJECT_ADD_INT32_PROPERTY(macMargin, 3);

#ifdef WIN32
    bkg = NULL;
#endif

#ifdef MACOSX
    this->panel  = NULL;
	this->layout = NULL;
#endif
}

PKScrollView::~PKScrollView()
{
#ifdef WIN32
    if(bkg != NULL)
    {
        DeleteObject(bkg);
    }
#endif

#ifdef MACOSX
	// Panel & layout will be deleted automatically
	// since they are made part of the UI hierarchy
#endif
}

void PKScrollView::build()
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

    PKVariant *vs = this->get(PKScrollView::VSCROLL_BOOL_PROPERTY);
    bool vScroll  = PKVALUE_BOOL(vs);

    PKVariant *hs = this->get(PKScrollView::HSCROLL_BOOL_PROPERTY);
    bool hScroll  = PKVALUE_BOOL(hs);
    
#ifdef LINUX

    this->widget = gtk_scrolled_window_new(NULL, NULL); 
    
#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);
    
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | WS_CLIPCHILDREN | SS_NOTIFY;

    if(vScroll)
    {
        dwStyle |= WS_VSCROLL;
    }

    if(hScroll)
    {
        dwStyle |= WS_VSCROLL;
    }
    
    WNDCLASSEX wclass;

	wclass.cbSize	     = sizeof(WNDCLASSEX);
	wclass.style	     = CS_DBLCLKS;
	wclass.lpfnWndProc   = PKScrollView::CustomWinProc;
	wclass.cbClsExtra    = 0;
	wclass.cbWndExtra    = 0;
	wclass.hInstance     = hInst;
	wclass.hIcon = NULL;
	wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
	wclass.lpszMenuName  = NULL;
	wclass.hIconSm       = NULL;
	wclass.lpszClassName = L"PKSCROLLVIEW"; 

	DWORD result = RegisterClassEx(&wclass);


    this->hwnd = CreateWindowEx(WS_EX_STATICEDGE |   // looks better than CLIENTEDGE
                                WS_EX_CONTROLPARENT, // for TAB key handling
                                L"PKSCROLLVIEW", L"",
							    dwStyle, 0, 0, 100, 80,
							    parentView->getWindowHandle(), 
							    (HMENU) this->controlId, 
                                hInst, 0);

    if(this->hwnd)
    {
        SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID)this);
    }

#endif

#ifdef MACOSX
	
	// Create the panel
	
	this->panel = new PKPanelControl();
	
	PKVariantUInt32 bs(2);
	this->panel->set(PKPanelControl::BORDER_SIZE_UINT32_PROPERTY, &bs);
	
	PKVariantColor fg(180,180,190,255);
	this->panel->set(PKPanelControl::BORDER_COLOR_PROPERTY, &fg);

	PKVariantInt32 lx(this->getLayoutPolicyX());
	this->panel->set(PKControl::LAYOUTX_INT32_PROPERTY, &lx);

	PKVariantInt32 ly(this->getLayoutPolicyY());
	this->panel->set(PKControl::LAYOUTY_INT32_PROPERTY, &ly);
	
	PKVariant *size = this->get(PKControl::FRAME_RECT_PROPERTY);
	PKRect    frame = PKVALUE_RECT(size);
	PKVariant *m = this->get(PKScrollView::MACMARGIN_INT32_PROPERTY);
	int32_t   mm = PKVALUE_INT32(m);
	
	frame.w += 2*mm;
	frame.h += 2*mm;

	PKVariantRect newSize(frame.x, frame.y, frame.w, frame.h);
	this->panel->set(PKControl::FRAME_RECT_PROPERTY, &newSize);
	
	parent->replaceChild(this, this->panel);
	panel->build();
	
	// Create the layout
	
	this->layout = new PKHLayout();
	
	this->layout->set(PKLayout::MARGIN_INT32_PROPERTY, m);
	
	this->layout->set(PKControl::LAYOUTX_INT32_PROPERTY, &lx);
	this->layout->set(PKControl::LAYOUTY_INT32_PROPERTY, &ly);
	this->layout->set(PKControl::FRAME_RECT_PROPERTY, &newSize);

	this->panel->addChild(this->layout);
	this->layout->build();
	
	this->layout->addChild(this);
	
	// Create the scrollview

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OptionBits options = 0;
	
	if(vScroll)
	{
		options |= kHIScrollViewOptionsVertScroll;
	}
	
	if(hScroll)
	{
		options |= kHIScrollViewOptionsHorizScroll;
	}
	
	OSStatus err = HIScrollViewCreate(options, &this->controlRef);
	
	if(err == noErr)
	{
		// Setup the scroll view

		HIViewRef mContent;
		
		HIViewFindByID(HIViewGetRoot(window),
					   kHIViewWindowContentID, &mContent);
		
		HIViewAddSubview(mContent, this->controlRef);
		HIViewSetVisible(this->controlRef, true);
	}
        
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->build();
	
#ifdef MACOSX	
			ControlRef contentView = root->getControlRef();
			
			HIViewRemoveFromSuperview(contentView);
			HIViewAddSubview(this->controlRef, contentView);
			HIViewSetNeedsDisplay(contentView, true);
#endif

#ifdef LINUX	

            if(this->widget != NULL)
        	{
        	    // Setup
        	    
        	    GtkPolicyType hp = GTK_POLICY_AUTOMATIC;
        	    GtkPolicyType vp = GTK_POLICY_AUTOMATIC;
        	    
        	    if(vScroll)
                {
                    vp = GTK_POLICY_ALWAYS;
                }
        	    
        	    if(hScroll)
                {
                    hp = GTK_POLICY_ALWAYS;
                }

        	    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(this->widget), hp, vp);
        	
                gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(this->widget), 
                                                      root->getWindowHandle());           
    		}

#endif
		}
	}

#ifdef LINUX
    
    if(this->widget != NULL)
    {
        gtk_widget_set_size_request(this->widget, 
                                    this->getW()+8, 
                                    this->getH()+8);
    }

#endif // LINUX
}

void PKScrollView::initialize(PKDialog *dialog)
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->initialize(dialog);
		}
	}
	
	PKControl::initialize(dialog);
}

void PKScrollView::destroy()
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->destroy();
            root = NULL;
		}
	}
    this->clearChildren();

    PKControl::destroy();
}



PKWindowHandle PKScrollView::getWindowHandle()
{
#ifdef LINUX
    return this->widget;
#endif


#ifdef WIN32
    if(this->hwnd)
    {
        return this->hwnd;
    }
#endif
	
	return NULL;
}

#ifdef WIN32

HBRUSH PKScrollView::getBackgroundBrush()
{
    if(bkg == NULL)
    {
        PKVariant *bk = this->get(PKScrollView::BACKGROUND_COLOR_PROPERTY);  
        PKColor bkgColor = PKVALUE_COLOR(bk);

        bkg = CreateSolidBrush(RGB(bkgColor.r, bkgColor.g, bkgColor.b));
    }

    return bkg;
}

bool PKScrollView::drawsItsBackground()
{
    return true;
}

#endif

void PKScrollView::move(int32_t x, 
					    int32_t y)
{
	PKControl::move(x, y);
    // Do not move our children, 
    // as they are relative to us
}

void PKScrollView::scrollToTop()
{
	PKControl *control = dynamic_cast<PKControl*>(this->getChildAt(0));

	if(control != NULL)
	{
		control->move(control->getX(), 0);
	}

#ifdef WIN32

	if(this->hwnd)
	{
        SetScrollPos(this->hwnd, SB_VERT, 0, TRUE);		
	}

#endif
}

void PKScrollView::updateScrollbars()
{
	// However since we are resized, we must update
	// the srolling units and such things

	if(this->getChildrenCount() != 1)
	{
		return;
    }

    PKVariant *vs = this->get(PKScrollView::VSCROLL_BOOL_PROPERTY);
    bool vScroll  = PKVALUE_BOOL(vs);

    PKVariant *hs = this->get(PKScrollView::HSCROLL_BOOL_PROPERTY);
    bool hScroll  = PKVALUE_BOOL(hs);

#ifdef WIN32

	PKControl *control = dynamic_cast<PKControl*>(this->getChildAt(0));

	if(control == NULL)
	{
		return;
	}

	uint32_t cw = control->getW();
	uint32_t ch = control->getH();

    if(this->hwnd && vScroll)
    {
		if(ch > this->getH())
		{
	        SCROLLINFO si;

		    si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE; 
			si.nMin   = 0; 
			si.nMax   = 1000; 

            double ratio = 0;

            if(ch != 0)
            {
                ratio = 1.0 - (((double) ch - (double) this->getH())/ch);
            }
            else
            {
                ratio = 0.5;
            }

			si.nPage  = (int) (ratio*1000.0); 

	        SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
		    EnableScrollBar(hwnd, SB_VERT, ESB_ENABLE_BOTH);
		}
		else
		{
		    EnableScrollBar(hwnd, SB_VERT, ESB_DISABLE_BOTH);
		}
    }

    if(this->hwnd && hScroll)
    {
		if(cw > this->getW())
		{
	        SCROLLINFO si;

		    si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE; 
			si.nMin   = 0; 
			si.nMax   = 1000; 

            double ratio = 0;

            if(cw != 0)
            {
                ratio = 1.0 - (((double) cw - (double) this->getW())/cw);
            }
            else
            {
                ratio = 0.5;
            }

			si.nPage  = (int) (ratio*1000.0); 

	        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
		    EnableScrollBar(hwnd, SB_HORZ, ESB_ENABLE_BOTH);
		}
		else
		{
		    EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
		}
    }

#endif
}

void PKScrollView::resize(uint32_t w, 
					      uint32_t h)
{
	PKControl::resize(w, h);
	
#ifdef LINUX
    return;
#endif

	// Do not update the size of our child layout blindly
	// Since we are a scroll view we consider that
	// the size of the child is set and managed elsewhere
	// when it is declared as FIXED, so leave it as it is
    // otherwise set it to the max of it's own min size
	// and our current assigned size

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
            uint32_t nw = root->getMinimumWidth();
            uint32_t nh = root->getMinimumHeight();

            // See if our child has a fixed size

			if(root->getLayoutPolicyX() == PK_LAYOUT_FIXED)
			{
				nw = root->getW();
			}

			if(root->getLayoutPolicyY() == PK_LAYOUT_FIXED)
			{
				nh = root->getH();
			}

            // See if our child needs to stretch
            // and if so, we need to stretch it, 
            // unless it is bigger than us already
            // Take into account the size of the
            // scrollbars

            int xpad = 0;
            int ypad = 0;

#ifdef WIN32
            PKVariant *vs = this->get(PKScrollView::VSCROLL_BOOL_PROPERTY);
            bool vScroll  = PKVALUE_BOOL(vs);
			
            PKVariant *hs = this->get(PKScrollView::HSCROLL_BOOL_PROPERTY);
            bool hScroll  = PKVALUE_BOOL(hs);
			
            if(vScroll)
            {
                xpad = GetSystemMetrics(SM_CXVSCROLL);
            }
			
            if(hScroll)
            {
                ypad = GetSystemMetrics(SM_CYHSCROLL);
            }
#endif

#ifdef MACOSX	            
			PKVariant *vs = this->get(PKScrollView::VSCROLL_BOOL_PROPERTY);
            bool vScroll  = PKVALUE_BOOL(vs);
			
            PKVariant *hs = this->get(PKScrollView::HSCROLL_BOOL_PROPERTY);
            bool hScroll  = PKVALUE_BOOL(hs);
			
            if(vScroll)
            {
                xpad = 13;
            }
			
            if(hScroll)
            {
                ypad = 13;
            }
#endif
			
            if(root->getLayoutPolicyX() == PK_LAYOUT_MAX)
            {
                if(nw < w)
                {
                    nw = w - xpad;
                }
            }

            if(root->getLayoutPolicyY() == PK_LAYOUT_MAX)
            {
                if(nh < h)
                {
                    nh = h - ypad;
                }
            }

            root->resize(nw, nh);
		}
	}

	// Update our scrollbars
	this->updateScrollbars();
}

void PKScrollView::show()
{
	PKControl::show();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->show();
		}
	}
}

void PKScrollView::hide()
{
	PKControl::hide();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->hide();			
		}
	}
}

void PKScrollView::enable()
{
	PKControl::enable();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->enable();
		}
	}
}

void PKScrollView::disable()
{
	PKControl::disable();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->disable();
		}
	}
}

int32_t PKScrollView::getAbsoluteX()
{
    // We are the toplevel for our children
    return 0;
}

int32_t PKScrollView::getAbsoluteY()
{
    // We are the toplevel for our children
    return 0;
}

uint32_t PKScrollView::getViewportWidth()
{
#ifdef WIN32
    PKVariant *vs = this->get(PKScrollView::VSCROLL_BOOL_PROPERTY);
    bool vScroll  = PKVALUE_BOOL(vs);

    if(vScroll)
    {
        return this->getW() - GetSystemMetrics(SM_CXVSCROLL);
    }
#endif

#ifdef MACOSX
    PKVariant *vs = this->get(PKScrollView::VSCROLL_BOOL_PROPERTY);
    bool vScroll  = PKVALUE_BOOL(vs);
	
    if(vScroll)
    {
        return this->getW() - 13;
    }
#endif

#ifdef LINUX
    PKVariant *vs = this->get(PKScrollView::VSCROLL_BOOL_PROPERTY);
    bool vScroll  = PKVALUE_BOOL(vs);
	
    if(vScroll)
    {
        return this->getW() - 18;
    }
#endif

    return this->getW();
}

uint32_t PKScrollView::getViewportHeight()
{
#ifdef WIN32
    PKVariant *hs = this->get(PKScrollView::HSCROLL_BOOL_PROPERTY);
    bool hScroll  = PKVALUE_BOOL(hs);

    if(hScroll)
    {
        return this->getH() - GetSystemMetrics(SM_CYHSCROLL);
    }
#endif

#ifdef MACOSX
    PKVariant *hs = this->get(PKScrollView::HSCROLL_BOOL_PROPERTY);
    bool hScroll  = PKVALUE_BOOL(hs);
	
    if(hScroll)
    {
        return this->getH() - 13;
    }
#endif

#ifdef LINUX
    PKVariant *hs = this->get(PKScrollView::HSCROLL_BOOL_PROPERTY);
    bool hScroll  = PKVALUE_BOOL(hs);
	
    if(hScroll)
    {
        return this->getH() - 18;
    }
#endif

    return this->getH();
}

uint32_t PKScrollView::getMinimumWidth()
{
    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    {
        return this->getW();
    }

    // We don't have a minmum
    return 0;

    /*
    if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumWidth());
		}
	}
    */
}

uint32_t PKScrollView::getMinimumHeight()
{
    if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
    {
        return this->getH();
    }

    return 0;

    /*
	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumHeight());
		}
	}
    */
	return 0;
}
