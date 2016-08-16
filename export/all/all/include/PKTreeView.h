//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_TREEVIEW_H
#define PK_TREEVIEW_H

#include "PKControl.h"

#ifdef WIN32
#include <windowsx.h>
#endif

//
// PKListBox
//

class PKTreeView : public PKControl
{
public:

//
// PROPERTIES
//

public:
	PKTreeView();
protected:
	~PKTreeView();

public:
    virtual void build();

	// Utility methods

	void addItem(std::wstring text,
				 std::string  id,
				 std::string  parentId = "");

	void clearItems();

	void expandRoot();
	void expandAll();

	std::string getSelectedItemId();
    void setSelectedItem(std::string id);

	// From PKControl

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

public:

	bool built;

#ifdef WIN32
	std::vector<HTREEITEM> items;
	std::vector<std::string> ids;

public:

	std::string getIdForItem(HTREEITEM item);

	WNDPROC lpOldProc;

	static LRESULT CALLBACK TreeViewWinProc(HWND hwnd,
							 		        UINT uMsg,
								 	        WPARAM wParam,
                                            LPARAM lParam);
#endif
};

#endif // PK_TREEVIEW_H
