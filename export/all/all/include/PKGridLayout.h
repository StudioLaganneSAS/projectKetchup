//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKGRIDLAYOUT_H
#define PKGRIDLAYOUT_H

#include "PKControl.h"

//
// PKGridLayout
//

class PKGridLayout : public PKControl
{
public:

	static std::string MARGIN_INT32_PROPERTY;
	static std::string SPACING_INT32_PROPERTY;
	static std::string COLS_UINT32_PROPERTY;

public:

	PKGridLayout();
protected:
	~PKGridLayout();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

	virtual PKWindowHandle getWindowHandle();

	void setMargin(uint32_t margin);
	void setSpacing(uint32_t spacing);

	void setCols(uint32_t cols);

	PKObject *getChildAtRowAndCol(int32_t row, int32_t col);

	int32_t getIndexForChild(PKObject *child);

	int32_t getColForChild(PKObject *child);
	int32_t getRowForChild(PKObject *child);

    int32_t getColAt(int x);
    int32_t getRowAt(int y);

	int32_t getXForChildAtPos(int32_t col, int32_t row);
	int32_t getYForChildAtPos(int32_t col, int32_t row);

	void insertNewChildAt(int32_t col, int32_t row, PKObject *child);

    void exchangeChildren(PKObject *child1, PKObject *child2);

	virtual uint32_t getMinimumWidth();
	virtual uint32_t getMinimumHeight();

	void relayout();

    void resize(uint32_t w, 
				uint32_t h);

	void move(int32_t x, 
			  int32_t y);

	void show();
	void hide();

	void enable();
	void disable();

#ifdef WIN32
    virtual bool relayWIN32Event(HWND   parent,
                                 UINT   msg, 
							     WPARAM wParam, 
							     LPARAM lParam);
#endif

    // From PKObject

	void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

	void computeLayout();

	std::vector<uint32_t> columnWidths;
	std::vector<uint32_t> rowHeights;

    int  cached_children_count;
    bool refresh;
};

#endif // PKGRIDLAYOUT_H
