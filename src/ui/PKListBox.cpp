//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKListBox.h"
#include "PKStr.h"
#include "PKDialog.h"

//
// Properties
//

std::string PKListBox::ITEMS_WSTRINGLIST_PROPERTY   = "items";
std::string PKListBox::ALWAYS_SCROLL_BOOL_PROPERTY  = "alwaysScroll";
std::string PKListBox::SELECTED_ITEM_INT32_PROPERTY = "selectedItem";

//
// MACOSX
//

#ifdef MACOSX

std::vector<CFStringRef> PKListBox::items;

OSStatus PKDataBrowserItemDataCallback(ControlRef browser,
									   DataBrowserItemID item,
									   DataBrowserPropertyID property,
									   DataBrowserItemDataRef itemData,
									   Boolean setValue)
{
	SetDataBrowserItemDataText(itemData, PKListBox::items[item-1]);
	return noErr;
}

#endif // MACOSX

//
// PKListBox
//

PKListBox::PKListBox()
{
    std::vector<std::wstring> list;

    PKOBJECT_ADD_WSTRINGLIST_PROPERTY(items, list);
    PKOBJECT_ADD_BOOL_PROPERTY(alwaysScroll, false);
    PKOBJECT_ADD_INT32_PROPERTY(selectedItem, -1);

#ifdef LINUX
    this->store = NULL;
    this->tree  = NULL;
#endif

}

PKListBox::~PKListBox()
{
#ifdef WIN32

	if(this->hwnd != NULL)
	{
		DestroyWindow(this->hwnd);
	}

#endif

#ifdef LINUX
    if(this->store != NULL)
	{
	    g_object_unref(G_OBJECT(this->store));
	    this->store = NULL;
	}
    
#endif


#ifdef MACOSX

	if(this->controlRef != NULL)
	{
		DisposeControl(this->controlRef);
	}

#endif
}

void PKListBox::build()
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

	this->store  = gtk_list_store_new(1, G_TYPE_STRING);
	this->widget = gtk_scrolled_window_new(NULL, NULL); 
	
    this->tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    
    if(this->widget != NULL && tree != NULL)
	{
	    // Setup scroll view
	
        GtkPolicyType hp = GTK_POLICY_AUTOMATIC;
        GtkPolicyType vp = GTK_POLICY_AUTOMATIC;

        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(this->widget), hp, vp);	    
	
	    // Setup tree view
	
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();

        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes ("", renderer,
                                                                              "text", 0, NULL);

        gtk_tree_view_append_column(GTK_TREE_VIEW(this->tree), column);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(this->tree), FALSE);
        gtk_widget_show(this->tree);
                                                     
       // Add to scrolled view
       
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(this->widget), 
                                              this->tree);    
        
        // Connect
       
       GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(this->tree));
               
       g_signal_connect(G_OBJECT(selection), "changed", 
                         G_CALLBACK(PKListBox::gtk_pklistbox_selection_changed), this);
 	}    

#endif
	
#ifdef WIN32

    PKVariant *it = this->get(PKListBox::ITEMS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> items = PKVALUE_WSTRINGLIST(it);

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT;

    PKVariant *as = this->get(PKListBox::ALWAYS_SCROLL_BOOL_PROPERTY);
    bool alwaysScroll = PKVALUE_BOOL(as);

    if(alwaysScroll)
    {
        style |= LBS_DISABLENOSCROLL;
    }

	this->hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                                L"LISTBOX", L"",
							    style, 
							    0, 0, 100, 80,
							    parentView->getWindowHandle(), 
							    (HMENU) this->controlId, 
							    hInst, 0);

	if(this->hwnd)
	{
		SendMessage(this->hwnd, WM_SETFONT, 
				   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	

        // Add items if any
    
        for(uint32_t i=0; i < items.size(); i++)
        {
            SendMessage(this->hwnd, LB_ADDSTRING, 0, (LPARAM) items[i].c_str());
        }

        PKVariant *id = this->get(PKListBox::SELECTED_ITEM_INT32_PROPERTY);
        int32_t index = PKVALUE_INT32(id);

        if(index >= 0)
        {
            SendMessage(this->hwnd, LB_SETCURSEL, index, 0);
        }
    }

#endif

#ifdef MACOSX

	WindowRef window = GetWindowFromPort(parentView->getWindowHandle());
	Rect rect;
	
	rect.left = rect.top = 0;
	rect.bottom = rect.right = 100;
	
	OSStatus err = CreateDataBrowserControl(window, &rect, kDataBrowserListView, &this->controlRef);
	
	if(err == noErr)
	{
		SetControlID(this->controlRef, &this->macId);
		
		// TODO: use properties to set this up
		
		SetDataBrowserHasScrollBars(this->controlRef, false, true);
		SetDataBrowserListViewHeaderBtnHeight(this->controlRef, 0);

		SetDataBrowserSelectionFlags(this->controlRef, 
									 kDataBrowserSelectOnlyOne  |
									 kDataBrowserResetSelection);
		
		SetDataBrowserTableViewHiliteStyle(this->controlRef, 
										   kDataBrowserTableViewFillHilite);
		
		
		Boolean f = true;
		SetControlData(this->controlRef, 0, kControlDataBrowserIncludesFrameAndFocusTag, sizeof(Boolean), &f);
		
		DataBrowserCallbacks callbacks;
		callbacks.version = kDataBrowserLatestCallbacks;
		err = InitDataBrowserCallbacks(&callbacks);
		
		callbacks.u.v1.itemDataCallback =
			NewDataBrowserItemDataUPP(PKDataBrowserItemDataCallback);
		err = SetDataBrowserCallbacks(this->controlRef, &callbacks);

		DataBrowserListViewColumnDesc desc;
		memset(&desc, 0, sizeof(desc));
		
		// TODO adjust these better !!
		
		desc.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;
		desc.headerBtnDesc.btnContentInfo.contentType = kControlContentTextOnly;
		desc.headerBtnDesc.btnFontStyle.flags = kControlUseFontMask | kControlUseJustMask;
		desc.headerBtnDesc.btnFontStyle.font = kControlFontViewSystemFont;
		desc.headerBtnDesc.titleOffset = 0; 
		desc.headerBtnDesc.initialOrder = kDataBrowserOrderUndefined; 
		desc.headerBtnDesc.minimumWidth = 0;
		desc.headerBtnDesc.maximumWidth = 500;
		
		desc.propertyDesc.propertyID = 'pklb';
		desc.propertyDesc.propertyType = kDataBrowserTextType;
		desc.propertyDesc.propertyFlags = kDataBrowserDefaultPropertyFlags + kDataBrowserListViewSelectionColumn;;
		
		err = AddDataBrowserListViewColumn(this->controlRef, &desc, 0);

		PKVariant *it = this->get(PKListBox::ITEMS_WSTRINGLIST_PROPERTY);
		std::vector<std::wstring> items = PKVALUE_WSTRINGLIST(it);
		
		if(items.size() > 0)
		{
			for(uint32_t i=0; i < items.size(); i++)
			{
				this->addItem(items[i]);
			}
		}
	}
	
#endif
}

void PKListBox::clearItems()
{
#ifdef WIN32
    if(this->hwnd)
    {
        SendMessage(this->hwnd, LB_RESETCONTENT, 0, 0);
    }
#endif

#ifdef LINUX
    if(this->store != NULL)
    {
        gtk_list_store_clear(this->store);
    }
#endif
	
#ifdef MACOSX
	
	if(this->controlRef)
	{
		RemoveDataBrowserItems(this->controlRef,
							   kDataBrowserNoItem,
							   0, NULL, 
							   kDataBrowserItemNoProperty);
	}
	
	this->ids.clear();
	
#endif
}

void PKListBox::addItem(std::wstring item)
{
#ifdef WIN32
    if(this->hwnd)
    {
        SendMessage(this->hwnd, LB_ADDSTRING, 0, (WPARAM) item.c_str());
    }
#endif

#ifdef LINUX

    if(this->tree != NULL && this->store != NULL)
    {
        std::string text = PKStr::wStringToUTF8string(item);

        GtkTreeIter iter;
        gtk_list_store_append(this->store, &iter);
        
        gtk_list_store_set (this->store, &iter,
                            0, text.c_str(), -1);
    }
    
#endif

#ifdef MACOSX
	
	if(this->controlRef)
	{
		DataBrowserItemID itemId;
		
		itemId = PKListBox::items.size() + 1;
		this->ids.push_back(itemId);
		PKListBox::items.push_back(PKStr::wStringToCFString(item));
		
		AddDataBrowserItems(this->controlRef, 
							kDataBrowserNoItem,
							1, &itemId,
							kDataBrowserItemNoProperty);
		
	}
	
#endif
}

void PKListBox::setCurrentSelection(int32_t index)
{
#ifdef WIN32
    if(this->hwnd)
    {
        SendMessage(this->hwnd, LB_SETCURSEL, index, 0);
    }
#endif

#ifdef LINUX

    if(this->tree != NULL)
	{
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(this->tree));
        
        if(selection)
	    {
	        GtkTreePath *path = gtk_tree_path_new_from_indices(index, -1);
            
            if(path)
	        {
	            gtk_tree_selection_select_path(selection, path);
                gtk_tree_path_free(path);	        
	        }
	    }
	}

#endif

#ifdef MACOSX
	
	if(this->controlRef)
	{
		if((index < this->ids.size()) && index >= 0)
		{
			DataBrowserItemID item = this->ids[index];
		
			SetDataBrowserSelectedItems(this->controlRef,
										1, &item, kDataBrowserItemsAssign);
		}
		else
		{
			// Clear all items
			SetDataBrowserSelectedItems(this->controlRef,
										0, NULL, kDataBrowserItemsAssign);
		}
	}
	
#endif
}

int32_t PKListBox::getCurrentSelection()
{
#ifdef WIN32
	
	if(this->hwnd)
	{
		return SendMessage(this->hwnd, LB_GETCURSEL, 0, 0);
	}
	
#endif
	
#ifdef LINUX
    
    if(this->tree != NULL)
	{
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(this->tree));
        
        if(selection)
	    {
	        GtkTreeModel *model;
	        GtkTreeIter   iter;
	        
	        if(gtk_tree_selection_get_selected(selection, &model, &iter))
    	    {
                GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
                
                if(path != NULL)
        	    {
                    gint *indexes = gtk_tree_path_get_indices(path);
 
                    int sel = -1;
 
                    if(indexes)
            	    {
            	        sel = *indexes;
            	    }
            	    
                    gtk_tree_path_free(path);
                    
        	        return sel;
        	    }
    	    }
	    }
	}
    
#endif
	
#ifdef MACOSX
	
	if(this->controlRef)
	{
		DataBrowserItemID first;
		DataBrowserItemID last;
		
		GetDataBrowserSelectionAnchor(this->controlRef,
									  &first, &last);

		for(uint32_t i=0; i < this->ids.size(); i++)
		{
			if(this->ids[i] == first)
			{
				return i;
			}
		}
	}
	
#endif
	
	return -1;
}


void PKListBox::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKListBox::SELECTED_ITEM_INT32_PROPERTY)
	{
		int32_t newValue = PKVALUE_INT32(this->get(PKListBox::SELECTED_ITEM_INT32_PROPERTY));

		if(newValue != this->getCurrentSelection())
		{
			this->setCurrentSelection(newValue);
		}
	}

	PKControl::selfPropertyChanged(prop, oldValue);
}


uint32_t PKListBox::getMinimumWidth()
{

    if(this->getLayoutPolicyX() != PK_LAYOUT_FIXED)
    {
		return 0;
    }

    return this->getW();
}

uint32_t PKListBox::getMinimumHeight()
{

    if(this->getLayoutPolicyY() != PK_LAYOUT_FIXED)
    {
        return 0;
    }

    return this->getH();
}
#ifdef LINUX
    
gboolean PKListBox::gtk_pklistbox_selection_changed(GtkTreeSelection *treeselection,
                                                    gpointer     user_data)
{
    PKListBox *lb = (PKListBox *) user_data;
    
    if(lb == NULL)
    {
        return true;
    }
    
    lb->linuxSelectionChanged();

    return true;
}
                                                        

void PKListBox::linuxSelectionChanged()
{
    if(this->dialog != NULL)
    {
        PKVariant   *i = this->get(PKControl::ID_STRING_PROPERTY);
        std::string id = PKVALUE_STRING(i);
    
		int32_t oldValue = PKVALUE_INT32(obj->get(PKListBox::SELECTED_ITEM_INT32_PROPERTY));

		PKVariantInt32 newV(this->getCurrentSelection());
		this->set(PKListBox::SELECTED_ITEM_INT32_PROPERTY, &newV);

        dialog->listBoxSelectionChanged(id, oldValue);
    }        
}

#endif


