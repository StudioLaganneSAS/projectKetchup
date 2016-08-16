//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKTreeView.h"
#include "PKStr.h"
#include "PKDialog.h"

#ifdef WIN32

LRESULT CALLBACK PKTreeView::TreeViewWinProc(HWND hWnd, 
										     UINT msg, 
										     WPARAM wParam, 
										     LPARAM lParam)
{
	PKTreeView *tree = (PKTreeView *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(tree == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	PKDialog *dialog = tree->dialog;

	if(dialog == NULL)
	{
        return tree->lpOldProc(hWnd, msg, wParam, lParam);
	}

	switch(msg)
	{
	case WM_RBUTTONDOWN:
		{
			// Do a hit test to find the 
			// item that was actally hit

			TVHITTESTINFO info;
			memset(&info, 0, sizeof(info));
			info.pt.x = LOWORD(lParam);
			info.pt.y = HIWORD(lParam);
			info.flags = TVHT_ONITEM;

			HTREEITEM item = TreeView_HitTest(tree->hwnd, &info);

			if(item)
			{
				std::string id;
				for(uint32_t i=0; i<tree->items.size(); i++)
				{
					if(tree->items[i] == item)
					{
						POINT pt;
						pt.x = info.pt.x;
						pt.y = info.pt.y;

						ClientToScreen(tree->hwnd, &pt);
						ScreenToClient(dialog->getWindowHandle(), &pt);

						dialog->treeViewItemRightClicked(tree->ids[i],
														 pt.x,
														 pt.y);
						break;
					}
				}
			}
		}
		break;
	}

	return tree->lpOldProc(hWnd, msg, wParam, lParam);
}

#endif

//
// PKTreeView
//

PKTreeView::PKTreeView()
{
	this->built = false;
#ifdef WIN32
    this->lpOldProc = NULL;
#endif
}

PKTreeView::~PKTreeView()
{
#ifdef WIN32

    if(this->built && this->hwnd)
    {
    	SubclassWindow(this->hwnd, (WNDPROC) this->lpOldProc);
    }

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

void PKTreeView::build()
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
	
    if(this->built)
    {
        return;
    }

#ifdef WIN32

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
				  TVS_HASLINES | TVS_LINESATROOT | 
				  TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	this->hwnd = CreateWindowEx(WS_EX_STATICEDGE,
                                WC_TREEVIEW, L"",
							    style, 
							    0, 0, 100, 80,
							    parentView->getWindowHandle(), 
							    (HMENU) this->controlId, 
							    hInst, 0);

	if(this->hwnd)
	{
		// Set Font
		SendMessage(this->hwnd, WM_SETFONT, 
				   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	

		// Set subclassing

        this->lpOldProc = SubclassWindow(this->hwnd, (WNDPROC) TreeViewWinProc);
    	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID) (PKTreeView *) this);
    }

#endif
}

void PKTreeView::addItem(std::wstring text,
						 std::string  id,
						 std::string  parentId)
{
#ifdef WIN32

	if(this->hwnd)
	{
		TVITEM		   tvi; 
		TVINSERTSTRUCT tvins;

		memset(&tvi, 0, sizeof(tvi));
		memset(&tvins, 0, sizeof(tvins));

		HTREEITEM hParent = NULL;
			
		if(parentId == "")
		{
			hParent = (HTREEITEM)TVI_ROOT; 
		}
		else
		{
			for(uint32_t i=0; i<this->ids.size(); i++)
			{
				if(this->ids[i] == parentId)
				{
					hParent = this->items[i];
					break;
				}
			}
		}

		tvi.mask = TVIF_TEXT | TVIF_PARAM; 

		// Set the text of the item. 
		tvi.pszText = (LPWSTR) text.c_str(); 
		tvi.cchTextMax = text.length();

		uint32_t uid = this->items.size();

		tvi.lParam = (LPARAM) uid; // custom id ? 
		tvins.item = tvi; 
		tvins.hParent = hParent; 

		// Add the item to the tree-view control. 

		HTREEITEM newItem = (HTREEITEM)SendMessage(this->hwnd, 
									   TVM_INSERTITEM, 
									   0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

		// Record new item handle, along with its ID
		this->items.push_back(newItem);
		this->ids.push_back(id);
	}

#endif
}

void PKTreeView::clearItems()
{
#ifdef WIN32

	if(this->hwnd)
	{
		TreeView_DeleteAllItems(this->hwnd);
		this->ids.clear();
		this->items.clear();
	}

#endif
}


void PKTreeView::expandRoot()
{
#ifdef WIN32

	if(this->hwnd && this->items.size())
	{
		TreeView_Expand(this->hwnd, 
						this->items[0],
						TVE_EXPAND);
	}

#endif
}

void PKTreeView::expandAll()
{
#ifdef WIN32

	if(this->hwnd && this->items.size())
	{
		for(uint32_t i=0; i < this->items.size(); i++)
		{
			TreeView_Expand(this->hwnd, 
							this->items[i],
							TVE_EXPAND);
		}
	}

#endif
}

#ifdef WIN32

std::string PKTreeView::getIdForItem(HTREEITEM item)
{
	for(uint32_t i = 0; i < this->items.size(); i++)
	{
		if(this->items[i] == item)
		{
			return this->ids[i];
		}
	}

	return "";
}

#endif

std::string PKTreeView::getSelectedItemId()
{
#ifdef WIN32

	if(this->hwnd)
	{
		HTREEITEM item = TreeView_GetSelection(this->hwnd);

		if(item)
		{
			return this->getIdForItem(item);
		}
	}

#endif

	return "";
}

void PKTreeView::setSelectedItem(std::string id)
{
#ifdef WIN32

    if(this->hwnd)
    {
        for(uint32_t i=0; i < this->ids.size(); i++)
        {
            if(this->ids[i] == id)
            {
                TreeView_SelectItem(this->hwnd, this->items[i]);
                break;
            }
        }
    }

#endif
}


void PKTreeView::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	PKControl::selfPropertyChanged(prop, oldValue);
}


uint32_t PKTreeView::getMinimumWidth()
{

    if(this->getLayoutPolicyX() != PK_LAYOUT_FIXED)
    {
		return 0;
    }

    return this->getW();
}

uint32_t PKTreeView::getMinimumHeight()
{

    if(this->getLayoutPolicyY() != PK_LAYOUT_FIXED)
    {
        return 0;
    }

    return this->getH();
}
