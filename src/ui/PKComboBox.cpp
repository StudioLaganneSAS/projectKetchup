//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKComboBox.h"
#include "PKStr.h"
#include "PKDialog.h"

#ifdef MACOSX
#define CB_MAC_MENU_BASE 0x0FF0
#endif

//
// Properties
// 

std::string PKComboBox::ITEMS_WSTRINGLIST_PROPERTY = "items";
std::string PKComboBox::CURRENTITEM_INT32_PROPERTY = "currentItem";


//
// PKComboBox
//

PKComboBox::PKComboBox()
{
#ifdef MACOSX
	this->menu = NULL;
#endif
	
#ifdef LINUX
	this->numItems = 0;
	this->recurse  = false;
	this->handler_id = 0;
#endif

    std::vector<std::wstring> list;
    PKOBJECT_ADD_WSTRINGLIST_PROPERTY(items, list);
	PKOBJECT_ADD_INT32_PROPERTY(currentItem, -1);
}

PKComboBox::~PKComboBox()
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
	
	if(this->menu != NULL)
	{
		this->menu->release();
	}

#endif
}

void PKComboBox::build()
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

    PKVariant *list = this->get(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

#ifdef LINUX
    
    this->widget = gtk_combo_box_new_text();
    
    if(this->widget != NULL)
    {
        for(uint32_t i=0; i<items.size(); i++)
        {
            std::string t = PKStr::wStringToUTF8string(items[i]);
            gtk_combo_box_insert_text(GTK_COMBO_BOX(this->widget), i, t.c_str());
        }

        // Select

	    gtk_combo_box_set_active(GTK_COMBO_BOX(this->widget), 0);

        // Connect
     
        this->handler_id = g_signal_connect(G_OBJECT(this->widget), "changed", 
                                            G_CALLBACK(PKComboBox::gtk_pkcombobox_changed), this);
    }
    
#endif

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(L"COMBOBOX", L"",
							  WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST, 
							  0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

	if(this->hwnd)
	{
		SendMessage(this->hwnd, WM_SETFONT, 
				   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	

        SendMessage(this->hwnd, CB_RESETCONTENT, 0, 0);

        for(unsigned int i=0; i < items.size(); i++)
        {
            std::wstring text = items[i];
            SendMessage(this->hwnd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) text.c_str());
        }

        SendMessage(this->hwnd, CB_SETCURSEL, 0, 0);
    }

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;

	this->menu = new PKPopupMenu();

	OSStatus err;
	
	err = CreatePopupButtonControl(window, &rect, NULL, 
								  -12345, 
								  false, 0, teFlushDefault, 
								  normal, &this->controlRef);
	
	MenuRef menuID = (MenuRef) this->menu->getHandle();
	
	if(err == noErr)
	{
		err = SetControlData(this->controlRef, 
					   kControlEntireControl,
					   kControlPopupButtonMenuRefTag,
					   sizeof(MenuRef),
					   (void *) &menuID);
					   
						 
		err = SetControlID(this->controlRef, &this->macId);
	}

    if(this->menu != NULL)
    {
        this->menu->clear();

        for(unsigned int i=0; i < items.size(); i++)
        {
            std::wstring text = items[i];

	        this->menu->insertItem(CB_MAC_MENU_BASE + i,
						           text);

	        SetControl32BitMinimum(this->controlRef, 1);
            SetControl32BitMaximum(this->controlRef, i+1);
        }
    }

#endif

    PKControl::build();
}

void PKComboBox::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKComboBox::CURRENTITEM_INT32_PROPERTY)
    {		
		int index = PKVALUE_INT32(prop->getValue());

		if(this->getSelectedItem() != index)
		{
#ifdef WIN32
			SendMessage(this->hwnd, CB_SETCURSEL, index, 0);
#endif
		
#ifdef LINUX
			if(this->widget != NULL)
			{
				g_signal_handler_disconnect(G_OBJECT(this->widget),
											this->handler_id);	
				
				gtk_combo_box_set_active(GTK_COMBO_BOX(this->widget), index);
				
				this->handler_id = g_signal_connect(G_OBJECT(this->widget), "changed", 
													G_CALLBACK(PKComboBox::gtk_pkcombobox_changed), this);
			}
#endif
		
#ifdef MACOSX
			if(this->controlRef != NULL)
			{
				SetControl32BitValue(this->controlRef, index + 1);
			}
#endif		
		}
	}
	
	if(prop->getName() == PKComboBox::ITEMS_WSTRINGLIST_PROPERTY)
    {
        PKVariant *list = this->get(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY);
        std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

        // Remember the current selection

        int current = this->getSelectedItem();

        // Clear the list first

#ifdef WIN32
    	SendMessage(this->hwnd, CB_RESETCONTENT, 0, 0);
#endif

#ifdef LINUX
    if(this->widget != NULL)
    {
        for(uint32_t i=0; i<this->numItems; i++)
        {
            gtk_combo_box_remove_text(GTK_COMBO_BOX(this->widget), 0);
        }
    }
#endif

#ifdef MACOSX
        if(this->menu != NULL)
        {
	        this->menu->clear();
        }

#endif
        // Then add items

        for(unsigned int i=0; i < items.size(); i++)
        {
            std::wstring text = items[i];
#ifdef WIN32

            SendMessage(this->hwnd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) text.c_str());

#endif

#ifdef LINUX
            if(this->widget != NULL)
            {
                std::string t = PKStr::wStringToUTF8string(text);
                gtk_combo_box_insert_text(GTK_COMBO_BOX(this->widget), i, t.c_str());
            }
            
            this->numItems = items.size();
#endif

#ifdef MACOSX

	        if(this->menu != NULL)
	        {
		        this->menu->insertItem(CB_MAC_MENU_BASE + i,
							           text);
	        }

	        SetControl32BitMinimum(this->controlRef, 1);
            SetControl32BitMaximum(this->controlRef, i+1);

#endif
        }

        // Restore selection

        this->selectItem(current);
    }

    PKControl::selfPropertyChanged(prop, oldValue);
}

void PKComboBox::clearItems()
{
    PKVariant *list = this->get(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

	items.clear();

    PKVariantWStringList newList(items);
    this->set(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY, &newList);
}

void PKComboBox::addItem(std::wstring text)
{
    PKVariant *list = this->get(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

    items.push_back(text);

    PKVariantWStringList newList(items);
    this->set(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY, &newList);
}

void PKComboBox::selectItem(int index)
{
	PKVariantInt32 sel(index);
	this->set(PKComboBox::CURRENTITEM_INT32_PROPERTY, &sel);
}

int PKComboBox::getSelectedItem()
{
#ifdef WIN32
	return SendMessage(this->hwnd, CB_GETCURSEL, 0, 0);
#endif

#ifdef LINUX
    if(this->widget != NULL)
	{
        return gtk_combo_box_get_active(GTK_COMBO_BOX(this->widget));
	}        
#endif

#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		return GetControl32BitValue(this->controlRef)-1;
	}

	return -1;

#endif
}

std::wstring PKComboBox::getSelectedText()
{
	int selection = this->getSelectedItem();
	
	if(selection == -1)
	{
		return L"";
	}
	
	PKVariant *i = this->get(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY);
	std::vector<std::wstring> items = PKVALUE_WSTRINGLIST(i);
	
	if(selection >= 0 && selection < (int)items.size())
	{
		return items[selection];
	}
	
	return L"";
}

uint32_t PKComboBox::getMinimumWidth()
{
    PKVariant *list = this->get(PKComboBox::ITEMS_WSTRINGLIST_PROPERTY);
    std::vector <std::wstring> items = PKVALUE_WSTRINGLIST(list);

	if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
		return this->getW();
	}

    // Return accroding to our
	// current text & font

#ifdef WIN32

	int32_t wsize = 0x0;

	for(unsigned int i=0; i < items.size(); i++)
	{
		SIZE size;

		HDC dc = GetDC(this->hwnd);

		HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
		HFONT old  = (HFONT) SelectObject(dc, font);

		GetTextExtentPoint32(dc, 
							 items[i].c_str(), 
							 items[i].size(), 
							 &size);

		SelectObject(dc, old);

		ReleaseDC(this->hwnd, dc);

		if(size.cx > wsize)
		{
			wsize = size.cx;
		}
	}

	return (wsize + 25);

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect((ControlRef) this->controlRef, &rect, &dummy);
	return (rect.right - rect.left);
#endif

    return 0;
}

uint32_t PKComboBox::getMinimumHeight()
{
	// Return accroding to our
	// current text & font

	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

#ifdef WIN32

	DWORD result = SendMessage(this->hwnd, CB_GETITEMHEIGHT, 0, 0);
	return (result+8);

#endif

#ifdef MACOSX
	Rect rect;
	SInt16 dummy;

	GetBestControlRect((ControlRef) this->controlRef, &rect, &dummy);
	return (rect.bottom - rect.top);
#endif

    return 0;
}

#ifdef LINUX
    
void PKComboBox::gtk_pkcombobox_changed(GtkComboBox *widget,
                                        gpointer     user_data)
{
    PKComboBox *cb = (PKComboBox *) user_data;
    
    if(user_data == NULL)
    {
        return;
    }    
    
    cb->linuxComboBoxChanged();
}
                                        
                                   
void PKComboBox::linuxComboBoxChanged()
{
    if(this->recurse)
    {
        return;
    }    

    this->recurse = true;    
	
	int32_t oldValue = PKVALUE_INT32(this->get(PKComboBox::CURRENTITEM_INT32_PROPERTY));

    if(this->dialog != NULL)
	{
        PKVariant   *i = this->get(PKControl::ID_STRING_PROPERTY);
        std::string id = PKVALUE_STRING(i);

        this->dialog->comboBoxChanged(id);
    }    

    this->recurse = false;    
}
                                           
#endif


