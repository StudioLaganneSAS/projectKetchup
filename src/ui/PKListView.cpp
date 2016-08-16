//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKListView.h"
#include "PKStr.h"

//
// Properties
//

//
// PKListView
//

PKListView::PKListView()
{

}

PKListView::~PKListView()
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

void PKListView::build()
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

	this->hwnd = CreateWindow(WC_LISTVIEW, L"",
							  WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | WS_BORDER | LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS, 
							  0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

	if(this->hwnd)
	{
        ListView_SetExtendedListViewStyle(this->hwnd, LVS_EX_DOUBLEBUFFER | 
                                                      LVS_EX_FULLROWSELECT | 
                                                      //LVS_EX_BORDERSELECT |
                                                      LVS_EX_GRIDLINES | 
                                                      LVS_EX_LABELTIP ); // checkboxes ?

		//SendMessage(this->hwnd, WM_SETFONT, 
		//		   (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);	
	
        // TEST

        // background color
        // ListView_SetBkColor(this->hwnd, CLR_NONE);
        //ListView_SetBkColor(this->hwnd, RGB(128,128,128));

        // border color
        ListView_SetOutlineColor(this->hwnd, RGB(150, 150, 180));

        // columns
        
        LVCOLUMN lvc; 
        int iCol; 
 
        for (iCol = 0; iCol < 1; iCol++) 
        { 
            lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

            lvc.iSubItem = iCol;
            lvc.pszText  = L"A column";	
            lvc.cx       = 70;     // width of column in pixels

            // Use LVCFMT_IMAGE and iImage for image list icon

            if ( iCol < 2 )
                lvc.fmt = LVCFMT_LEFT;  // left-aligned column
            else
                lvc.fmt = LVCFMT_RIGHT; // right-aligned column		                         

            ListView_InsertColumn(this->hwnd, iCol, &lvc); 
            //ListView_SetColumnWidth(this->hwnd, iCol, 80); // Does this work ? YES
        } 
    
        // Items

        LVITEM lvI;

        for(int index = 0; index < 3; index++)
        {
            lvI.mask = LVIF_TEXT | LVIF_PARAM; 
            lvI.state = 0; 
            lvI.stateMask = 0; 
            lvI.iItem = index;
            lvI.iImage = 0; // Image list for item icons
            lvI.iSubItem = 0;
            lvI.lParam = (LPARAM) this;


            if(index == 0) lvI.pszText = L"Major";
            if(index == 1) lvI.pszText = L"Minor";
            if(index == 2) lvI.pszText = L"Major 7th";
            
            ListView_InsertItem(this->hwnd, &lvI);
/*
            lvI.mask = LVIF_TEXT; 
            lvI.state = 0; 
            lvI.stateMask = 0; 
            lvI.iItem = index;
            lvI.iSubItem = 1;
            lvI.pszText = L"A subitem";
            ListView_SetItem(this->hwnd, &lvI);

            lvI.mask = LVIF_TEXT; 
            lvI.state = 0; 
            lvI.stateMask = 0; 
            lvI.iItem = index;
            lvI.iSubItem = 2;
            lvI.pszText = L"15";
            ListView_SetItem(this->hwnd, &lvI);*/
        }

        // checkboxes
        // ListView_SetCheckState(this->hwnd, 1, TRUE);
    }

#endif

#ifdef MACOSX
    // TODO
#endif
}


void PKListView::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    // TODO

	PKControl::selfPropertyChanged(prop, oldValue);
}


uint32_t PKListView::getMinimumWidth()
{

#ifdef WIN32
    // TODO
#endif

#ifdef MACOSX
    // TODO
#endif

    return 70;
}

uint32_t PKListView::getMinimumHeight()
{

#ifdef WIN32
    // TODO
#endif

#ifdef MACOSX
    // TODO
#endif

    return 100;
}
