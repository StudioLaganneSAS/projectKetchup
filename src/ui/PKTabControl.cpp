//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKTabControl.h"
#include "PKStr.h"
#include "PKCursorTools.h"

#include "PKDialog.h"
#include "PKImageView.h"
#include "PKComboBox.h"
#include "PKGroupBox.h"
#include "PKButton.h"
#include "PKCheckBox.h"
#include "PKSlider.h"
#include "PKLink.h"
#include "PKPanelControl.h"
#include "PKLabel.h"
#include "PKGradientPanel.h"
#include "PKScrollView.h"
#include "PKImageView.h"

#pragma warning(disable : 4800)

//
// Properties
//

std::string PKTabControl::TABS_WSTRINGLIST_PROPERTY = "tabs";

//
// Windows stuff
//

#ifdef WIN32

#include <windowsx.h>

//
// This is the dialog window proc
// we override the original window
// proc from DWPWindow to be able to
// catch WM_COMMAND messages and then
// we call the original proc
//

LRESULT CALLBACK PKTabControl::TabWinProc(HWND hWnd, 
										   UINT msg, 
										   WPARAM wParam, 
										   LPARAM lParam)
{
	PKTabControl *tab = (PKTabControl *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(tab == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	PKDialog *dialog = tab->dialog;

	if(dialog == NULL)
	{
        return tab->lpOldProc(hWnd, msg, wParam, lParam);
	}

	switch(msg)
	{
	case WM_COMMAND:
		{
			// Find which control is sending this
			// and fire the appropriate callback

			uint32_t msg = HIWORD(wParam);
			uint32_t id  = LOWORD(wParam);

			PKControl *obj = PKControl::getControlWithId(id);

			if(obj != NULL)
			{
                if(msg == STN_CLICKED)
                {
                    PKLink *link = dynamic_cast<PKLink*>(obj);

                    if(link)
                    {
                        // Handle the click
                        link->clicked();
                    }
                }

				if(msg == CBN_SELCHANGE)
				{
                    PKComboBox *combo = dynamic_cast<PKComboBox*>(obj);
					
                    if(combo)
					{
						int32_t oldValue = PKVALUE_INT32(obj->get(PKComboBox::CURRENTITEM_INT32_PROPERTY));
						dialog->comboBoxChanged(combo->getControlId(), oldValue);
					}
				}

                PKButton *button = dynamic_cast<PKButton*>(obj);

				if(button)
				{
                    dialog->buttonClicked(button->getControlId());
				}

				if(dynamic_cast<PKGroupBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKGroupBox::CHECKED_BOOL_PROPERTY));
                    dialog->checkBoxAction(obj->getControlId(), oldValue);
				}

				if(dynamic_cast<PKCheckBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKCheckBox::CHECKED_BOOL_PROPERTY));
                    dialog->checkBoxAction(obj->getControlId(), oldValue);
				}

				return 0;
			}
		}
		break;

    case WM_HSCROLL:
		{
			HWND controlHWND = (HWND) lParam;

			if(tab->getChildrenCount()-1 >= tab->currentTab)
			{
				PKControl *root = dynamic_cast<PKControl*>(tab->getChildAt(tab->currentTab));

				if(root)
				{
    				PKView *obj = root->findObjectByWindow(controlHWND);

				    if(obj != NULL)
				    {
                        PKSlider *slider = dynamic_cast<PKSlider*>(obj);

                        if(slider)
					    {
							int32_t previousValue = PKVALUE_INT32(slider->get(PKSlider::VALUE_INT32_PROPERTY));
							int32_t newValue = (int32_t) SendMessage(slider->getWindowHandle(), TBM_GETPOS, 0, 0);

							PKVariantInt32 newV(newValue);
							slider->set(PKSlider::VALUE_INT32_PROPERTY, &newV);

					    	dialog->sliderChanged(slider->getControlId(), previousValue);
					    }
				    }
			    }
            }
		}
		break;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		{
			HWND control = (HWND) lParam;
			HDC  hdc     = (HDC)  wParam;

            // Handle links

			if(tab->getChildrenCount()-1 >= tab->currentTab)
    		{
	    		PKControl *root = dynamic_cast<PKControl*>(tab->getChildAt(tab->currentTab));

		    	if(root)
			    {
				    PKView *obj = root->findObjectByWindow(control);

                    if(obj != NULL)
	    	        {
                        // Check if it's a link

                        PKLink *link = dynamic_cast<PKLink*>(obj);

                        if(link)
                        {
                            PKVariant *color = link->get(PKLink::COLOR_COLOR_PROPERTY);
                            PKColor c = PKVALUE_COLOR(color);

                            SetTextColor(hdc, RGB(c.r, c.g, c.b));
                        }

                        // Check if it's a label

                        PKLabel *label = dynamic_cast<PKLabel*>(obj);

                        if(label)
                        {
                            PKVariant *color = label->get(PKLink::COLOR_COLOR_PROPERTY);
                            PKColor c = PKVALUE_COLOR(color);

                            SetTextColor(hdc, RGB(c.r, c.g, c.b));
                        }

                        // Check if the object is in a panel

                        PKObject *item = obj;

                        do
                        {
                            item = item->getParent();

                            if(item != NULL)
                            {
                                PKPanelControl  *panel1 = dynamic_cast<PKPanelControl *>(item);
                                PKGradientPanel *panel2 = dynamic_cast<PKGradientPanel *>(item);
                                PKScrollView    *scroll = dynamic_cast<PKScrollView *>(item);
                                PKImageView     *image  = dynamic_cast<PKImageView *>(item);

                                if(panel1)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) panel1->getBackgroundBrush();
                                    break;
                                }

                                if(panel2)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) GetStockObject(HOLLOW_BRUSH);
                                    break;
                                }

                                if(image)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) GetStockObject(HOLLOW_BRUSH);
                                    break;
                                }

                                if(scroll)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) scroll->getBackgroundBrush();
                                    break;
                                }
                            }
                        }
                        while(item != NULL);
                    }
                }
            }

            // Handle backgroud

            if(dialog->isUsingXPThemes())
			{
				RECT rc;

		        GetWindowRect(control, &rc);

				MapWindowPoints(NULL, 
                                tab->getWindowHandle(), 
								(LPPOINT)(&rc), 2);

			    SetBrushOrgEx(hdc, -rc.left, -rc.top, NULL);
				SetBkMode(hdc, TRANSPARENT);

				return (LRESULT) tab->getBkgBrush();
			}
		}
		break;

	case WM_DRAWITEM:
		{
			// This is called for owner drawn
			// static controls such as the 
			// PKImageView class when it needs to
			// have an alpha channel...

			DRAWITEMSTRUCT *drawStruct = (DRAWITEMSTRUCT *) lParam;

			if(drawStruct != NULL)
			{
				if(tab->getChildrenCount()-1 >= tab->currentTab)
				{
                    PKControl *root = dynamic_cast<PKControl*>(tab->getChildAt(tab->currentTab));

					if(root)
					{
						PKView *obj = root->findObjectByWindow(drawStruct->hwndItem);

						if(obj != NULL)
						{
							if(dynamic_cast<PKImageView*>(obj))
							{
								PKImageView *img = dynamic_cast<PKImageView*>(obj);

                                HBRUSH brush = NULL;

                                if(dialog->isUsingXPThemes())
			                    {
                                    brush = tab->getBkgBrush();
                                }
                                else
                                {
                                    brush = (HBRUSH) GetClassLongPtr(dialog->getWindowHandle(), GCLP_HBRBACKGROUND);
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

	return tab->lpOldProc(hWnd, msg, wParam, lParam);
}

#endif


//
// PKTabControl
//

PKTabControl::PKTabControl()
{
    std::vector<std::wstring> empty;
    PKOBJECT_ADD_WSTRINGLIST_PROPERTY(tabs, empty);

    this->dialog = NULL;
	this->currentTab = 0;
}

PKTabControl::~PKTabControl()
{
#ifdef WIN32

	SubclassWindow(this->hwnd, (WNDPROC) this->lpOldProc);

	if(this->hwnd != NULL)
	{
		DestroyWindow(this->hwnd);
	}

	if(this->backBrush != NULL)
	{
		DeleteObject(this->backBrush);
	}

#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		DisposeControl(this->controlRef);
	}

#endif
}

void PKTabControl::build()
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

#ifdef LINUX

    this->widget = gtk_notebook_new();

#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(WC_TABCONTROL, L"",
							  WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
							  0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

	if(this->hwnd)
	{
		SendMessage(this->hwnd, WM_SETFONT, 
				   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	
	}

	this->backBrush = NULL;

	// Install the new window proc

    this->lpOldProc = SubclassWindow(this->hwnd, (WNDPROC) TabWinProc);
	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID) (PKTabControl *) this);

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	OSStatus err = CreateTabsControl(window, &rect, kControlSizeNormal, 
									 kControlTabDirectionNorth, 0, NULL,
									 &this->controlRef);

	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);
	}


#endif

    // Build children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->build();
		}
	}

    // Triger creation of the tabs

#if(defined(MACOSX) || defined(WIN32))
    PKVariant *list = this->get(PKTabControl::TABS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

    PKVariantWStringList newList(items);
    this->set(PKTabControl::TABS_WSTRINGLIST_PROPERTY, &newList);
#endif

#if(defined(LINUX))

    PKVariant *tabs = this->get(PKTabControl::TABS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> items = PKVALUE_WSTRINGLIST(tabs);

    if(items.size() != this->getChildrenCount())
    {
        // Wrong config
        return;
    }

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
    {
        if(this->widget != NULL)
        {
            PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));
        
            if(child != NULL)
            {
                std::string title = PKStr::wStringToUTF8string(items[i]);
            
                gtk_notebook_append_page(GTK_NOTEBOOK(this->widget),
                                         child->getWindowHandle(),
                                         gtk_label_new(title.c_str()));
            }                               
        }                
	}

#endif
}

void PKTabControl::initialize(PKDialog *dialog)
{
    this->dialog = dialog;
    
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->initialize(dialog);
		}
	}

    PKControl::initialize(dialog);

    PKVariant *list = this->get(PKTabControl::TABS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

    if(items.size() > 0)
    {
        this->setCurrentTab(0);
    }
}

void PKTabControl::destroy()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            child->destroy();
            child = NULL;
		}
	}
    this->clearChildren();

    PKControl::destroy();
}
// From PKControl

#ifdef WIN32

HBRUSH PKTabControl::getBackgroundBrush()
{
    return this->getBkgBrush();
}

#endif

// From PKObject

void PKTabControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKTabControl::TABS_WSTRINGLIST_PROPERTY)
    {
        PKVariant *tabs = this->get(PKTabControl::TABS_WSTRINGLIST_PROPERTY);
        std::vector<std::wstring> items = PKVALUE_WSTRINGLIST(tabs);

        if(items.size() != this->getChildrenCount())
        {
            // Wrong config
            return;
        }

        // Clear the tabs first

#ifdef WIN32
        TabCtrl_DeleteAllItems(this->hwnd);
#endif

#ifdef MACOSX
		SetControlMaximum(this->controlRef, 0);
#endif
        // Now add items back

        for(unsigned int i=0; i < this->getChildrenCount(); i++)
        {
#ifdef WIN32

            if(this->hwnd == NULL)
            {
	            return;
            }

            TCITEM item;

            item.mask    = TCIF_TEXT;
            item.pszText = (LPWSTR) items[i].c_str();

            TabCtrl_InsertItem(this->hwnd, i, &item);

#endif

#ifdef LINUX
            
            if(this->widget != NULL)
            {
                PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));
            
                if(child != NULL)
                {
                    std::string title = PKStr::wStringToUTF8string(items[i]);
                
                    gtk_notebook_set_tab_label(GTK_NOTEBOOK(this->widget),
                                               child->getWindowHandle(),
                                               gtk_label_new(title.c_str()));
                }                               
            }                
        
#endif

#ifdef MACOSX

	        ControlTabInfoRecV1 infoRec;
        	
	        infoRec.version     = kControlTabInfoVersionOne;
	        infoRec.iconSuiteID = 0;
	        infoRec.name        = PKStr::wStringToCFString(items[i]);
        	
	        // Use SetControlMaximum to set the
	        // number of tabs to the new number
        	
	        SetControlMaximum(this->controlRef, i+1);
        	
	        // Use SetControlData to give
	        // data on the new Tab, the 
	        // control part code is set to 
	        // the tab number from 1...N
        	
	        SetControlData(this->controlRef,
				           i+1,
				           kControlTabInfoTag,
				           sizeof(infoRec),
				           &infoRec);

#endif
        }
    }

    PKControl::selfPropertyChanged(prop, oldValue);
}

void PKTabControl::setParentDialog(PKDialog *parent)
{
	this->dialog = parent;
}


void PKTabControl::clearTabs()
{
	PKCritLock lock(&this->mutex);

    PKVariant *list = this->get(PKTabControl::TABS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

	items.clear();

    PKVariantWStringList newList(items);
    this->set(PKTabControl::TABS_WSTRINGLIST_PROPERTY, &newList);
}

void PKTabControl::addTab(PKLayout    *object, 
						  std::wstring tabTitle)
{
	if(object == NULL)
	{
		return;
	}

    PKCritLock lock(&this->mutex);

    PKVariant *list = this->get(PKTabControl::TABS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

    items.push_back(tabTitle);
    this->addChild(object);

    PKVariantWStringList newList(items);
    this->set(PKTabControl::TABS_WSTRINGLIST_PROPERTY, &newList);
}

void PKTabControl::setCurrentTab(unsigned int index)
{
    PKVariant *list = this->get(PKTabControl::TABS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

    if(index >= items.size())
	{
		return;
	}

	this->currentTab = index;

#ifdef WIN32
	TabCtrl_SetCurSel(this->hwnd, index);
#endif

#ifdef LINUX
    if(this->widget != NULL)
	{
        gtk_notebook_set_current_page(GTK_NOTEBOOK(this->widget), index);
	}
	
	return;
	
#endif

#ifdef MACOSX
	SetControl32BitValue(this->controlRef, index + 1);
#endif

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);

        PKLayout *layout = dynamic_cast<PKLayout*>(child);

        if(layout)
        {
		    if(i == this->currentTab)
		    {
			    layout->show();
		    }
		    else
		    {
			    layout->hide();
		    }
        }
	}
}

uint32_t PKTabControl::getMinimumWidth()
{
	PKCritLock lock(&this->mutex);

	uint32_t minClientWidth  = 0;

	// Get the max width from
	// all our child layouts

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
		    uint32_t minLayoutWidth = ctrl->getMinimumWidth();

		    if(minLayoutWidth > minClientWidth)
		    {
			    minClientWidth = minLayoutWidth;
		    }
        }
	}

#ifdef WIN32

	uint32_t minClientHeight = 100;

	// Adjust the client width to the 
	// total control width & height 
	// using the TCM_ADJUSTRECT msg...

	RECT rect;

	rect.left   = 0;
	rect.right  = minClientWidth;
	rect.top    = 0;
	rect.bottom = minClientHeight;

	SendMessage(this->hwnd, 
				TCM_ADJUSTRECT, 
				(WPARAM) TRUE, // true to expand from client to window rect
				(LPARAM) (LPRECT) &rect);

	// Return adjusted width

	return (rect.right - rect.left + 10);

#endif

#ifdef MACOSX

	return (minClientWidth + 20);
	
#endif

    return 0;
}

uint32_t PKTabControl::getMinimumHeight()
{
	PKCritLock lock(&this->mutex);

	uint32_t minClientHeight = 0;

	// Get the max width from
	// all our child layouts

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
		    uint32_t minLayoutHeight = ctrl->getMinimumHeight();

		    if(minLayoutHeight > minClientHeight)
		    {
			    minClientHeight = minLayoutHeight;
		    }
        }
	}

#ifdef WIN32

	uint32_t minClientWidth  = 200;

	// Adjust the client width to the 
	// total control width & height 
	// using the TCM_ADJUSTRECT msg...

	RECT rect;

	rect.left   = 0;
	rect.right  = minClientWidth;
	rect.top    = 0;
	rect.bottom = minClientHeight;

	SendMessage(this->hwnd, 
				TCM_ADJUSTRECT, 
				(WPARAM) TRUE, // true to expand from client to window rect
				(LPARAM) (LPRECT) &rect);

	// Return adjusted width

	return (rect.bottom - rect.top + 10);

#endif

#ifdef MACOSX

	return (minClientHeight + 50);

#endif

    return 0;
}

// Gets x and y in parent coordinates

int32_t PKTabControl::getAbsoluteX()
{
    // We are the top level parent
    return 0;
}

int32_t PKTabControl::getAbsoluteY()
{
    // We are the top level parent
    return 0;
}

void PKTabControl::move(int32_t x, 
						 int32_t y)
{
	PKCritLock lock(&this->mutex);

	PKControl::move(x, y);

	int tx = x;
	int ty = y;

#ifdef WIN32

	// Adjust the client width to the 
	// total control width & height 
	// using the TCM_ADJUSTRECT msg...

	RECT rect;

	rect.left   = 0;
	rect.right  = 300;
	rect.top    = 0;
	rect.bottom = 200;

	SendMessage(this->hwnd, 
				TCM_ADJUSTRECT, 
				(WPARAM) FALSE, // false to shrink from window to client rect
				(LPARAM) (LPRECT) &rect);

	// Return adjusted width

	tx = rect.left+5;
	ty = rect.top+5;

#endif

#ifdef MACOSX

	tx += 10;
	ty += 35;

#endif

#ifdef LINUX
    return;
#endif

	// Move tabs

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
    		ctrl->move(tx, ty);
	    }
    }
}

void PKTabControl::resize(uint32_t w, 
						   uint32_t h)
{
	PKCritLock lock(&this->mutex);

	PKControl::resize(w, h);

	int tw = w;
	int th = h;

#ifdef LINUX
    return;
#endif

#ifdef WIN32

	// Adjust the client width to the 
	// total control width & height 
	// using the TCM_ADJUSTRECT msg...

	RECT rect;

	rect.left   = 0;
	rect.right  = tw;
	rect.top    = 0;
	rect.bottom = th;

	SendMessage(this->hwnd, 
				TCM_ADJUSTRECT, 
				(WPARAM) FALSE, // false to shrink from window to client rect
				(LPARAM) (LPRECT) &rect);

	// Return adjusted width

	tw = (rect.right - rect.left) - 10;
	th = (rect.bottom - rect.top) - 10;

#endif

#ifdef MACOSX

	tw -= 20;
	th -= 50;

#endif

	// Move tabs

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
    		ctrl->resize(tw, th);
	    }
    }

#ifdef WIN32

	// Finally let's create the
	// background bush so we can
	// give it to the dialog when
	// it needs to draw us

    HMODULE hinstDll;
	bool    bThemeActive = false;

    // Check if the application is themed
    
	hinstDll = LoadLibrary(L"UxTheme.dll");

    if(hinstDll != NULL)
    {
        typedef BOOL (*ISAPPTHEMEDPROC)();

        ISAPPTHEMEDPROC pIsAppThemed;

        pIsAppThemed = (ISAPPTHEMEDPROC) GetProcAddress(hinstDll, "IsAppThemed");

        if(pIsAppThemed != NULL)
		{
            bThemeActive = (bool) pIsAppThemed();
		}

        FreeLibrary(hinstDll);
    }

	if(this->backBrush != NULL)
	{
		DeleteObject(this->backBrush);
		this->backBrush = NULL;
	}

	// Only create the brush if
	// the app is using themes

	if(bThemeActive)
	{
	    RECT rc;

		// Get tab control dimensions
		GetWindowRect(this->hwnd, &rc);

        // Get the tab control DC
        HDC hDC = GetDC(this->hwnd);

        // Create a compatible DC
        HDC hDCMem = CreateCompatibleDC(hDC);

        HBITMAP hBmp = CreateCompatibleBitmap(hDC, 
											  rc.right - rc.left, 
											  rc.bottom - rc.top);

        HBITMAP hBmpOld = (HBITMAP)SelectObject(hDCMem, hBmp);

        // Tell the tab control to paint in our DC

        SendMessage(this->hwnd, WM_PRINTCLIENT, (WPARAM)(hDCMem), 
           (LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));

        // Create a pattern brush from 
		// the bitmap selected in our DC

		this->backBrush = CreatePatternBrush(hBmp);

        // Restore the bitmap
        SelectObject(hDCMem, hBmpOld);

        // Cleanup
        DeleteObject(hBmp);
        DeleteDC(hDCMem);
        ReleaseDC(this->hwnd, hDC);		
	}

#endif
}

void PKTabControl::show()
{
	PKCritLock lock(&this->mutex);

	PKControl::show();

#ifdef LINUX
    return;
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
		    if(i == this->currentTab)
		    {
			    ctrl->show();
		    }
		    else
		    {
			    ctrl->hide();
		    }
        }
	}
}

void PKTabControl::hide()
{
	PKCritLock lock(&this->mutex);

	PKControl::hide();

#ifdef LINUX
    return;
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
    		ctrl->hide();
    	}
    }
}

void PKTabControl::enable()
{
	PKCritLock lock(&this->mutex);

	PKControl::enable();

#ifdef LINUX
    return;
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
    		ctrl->enable();
	    }
    }
}

void PKTabControl::disable()
{
	PKCritLock lock(&this->mutex);

	PKControl::disable();

#ifdef LINUX
    return;
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
    		ctrl->disable();
	    }
    }
}

#ifdef WIN32

HBRUSH PKTabControl::getBkgBrush()
{
	return this->backBrush;
}

#endif
