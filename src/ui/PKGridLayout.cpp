//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKGridLayout.h"

//
// Properties
//

std::string PKGridLayout::MARGIN_INT32_PROPERTY  = "margin";
std::string PKGridLayout::SPACING_INT32_PROPERTY = "spacing";
std::string PKGridLayout::COLS_UINT32_PROPERTY   = "cols";


//
// PKGridLayout
// 

PKGridLayout::PKGridLayout()
{
#ifdef LINUX
	PKOBJECT_ADD_INT32_PROPERTY(margin, 6);
	PKOBJECT_ADD_INT32_PROPERTY(spacing, 4);
#endif
#ifdef WIN32
	PKOBJECT_ADD_INT32_PROPERTY(margin, 6);
	PKOBJECT_ADD_INT32_PROPERTY(spacing, 4);
#endif
#ifdef MACOSX
	PKOBJECT_ADD_INT32_PROPERTY(margin, 10);
	PKOBJECT_ADD_INT32_PROPERTY(spacing, 10);
#endif
	PKOBJECT_ADD_UINT32_PROPERTY(cols, 2);

    this->refresh = true;
    this->cached_children_count = 0;
}

PKGridLayout::~PKGridLayout()
{
}

void PKGridLayout::build()
{
#ifdef LINUX

	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);
    
    uint32_t rows = 1;
    uint32_t cnt  = this->getChildrenCount();
    
    if(cols != 0)
	{
	    rows = (cnt / cols)  + 1;
	}   
	
	if(!this->noWidget) 
	{
        this->widget = gtk_table_new(rows, cols, FALSE);
        
        if(this->widget != NULL)
	    {
	        gtk_container_set_border_width(GTK_CONTAINER(this->widget), margin);
	    }    
	}           

#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->build();

#ifdef LINUX
            if(this->widget != NULL && !this->noWidget)
	        {
	            int r = i / cols;
	            int c = i % cols;
	        
	            gtk_table_attach(GTK_TABLE(this->widget), child->getWindowHandle(),
	                                       c, c+1, r, r+1, GTK_FILL, GTK_FILL, spacing/2, spacing/2);
	        }    
#endif
		}
	}
}

void PKGridLayout::initialize(PKDialog *dialog)
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->initialize(dialog);
		}
	}
	
	PKControl::initialize(dialog);
}

void PKGridLayout::destroy()
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

PKWindowHandle PKGridLayout::getWindowHandle()
{
#ifdef LINUX
    return this->widget;
#endif

	// We forward to our parent

	PKObject *parent = this->getParent();

	if(parent)
	{
		PKView *parentView = dynamic_cast<PKView*>(parent);

		if(parentView)
		{
			return parentView->getWindowHandle();
		}
	}

	return NULL;
}

void PKGridLayout::setMargin(uint32_t margin)
{
	PKVariantInt32 m(margin);
	this->set(MARGIN_INT32_PROPERTY, &m);
}

void PKGridLayout::setSpacing(uint32_t spacing)
{
	PKVariantInt32 s(spacing);
	this->set(SPACING_INT32_PROPERTY, &s);
}

void PKGridLayout::setCols(uint32_t cols)
{
	PKVariantUInt32 c(cols);
	this->set(COLS_UINT32_PROPERTY, &c);
}

PKObject *PKGridLayout::getChildAtRowAndCol(int32_t row, int32_t col)
{
	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	int offset = cols*row + col;

	if(offset >= this->getChildrenCount())
	{
		return NULL;
	}

	return this->getChildAt(offset);
}

int32_t PKGridLayout::getIndexForChild(PKObject *child)
{
	uint32_t i;
	
	for(i=0; i<this->getChildrenCount(); i++)
	{
		if(this->getChildAt(i) == child)
		{
			return i;
		}
	}
	
	return -1;
}


int32_t PKGridLayout::getColForChild(PKObject *child)
{
	bool found = false;

	uint32_t i;
	
	for(i=0; i<this->getChildrenCount(); i++)
	{
		if(this->getChildAt(i) == child)
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		PKVariant *c = this->get(COLS_UINT32_PROPERTY);
		uint32_t cols = PKVALUE_UINT32(c);

		return (i % cols);
	}

	// Not found
	return -1;
}

int32_t PKGridLayout::getRowForChild(PKObject *child)
{
	bool found = false;

	uint32_t i;

	for(i=0; i<this->getChildrenCount(); i++)
	{
		if(this->getChildAt(i) == child)
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		PKVariant *c = this->get(COLS_UINT32_PROPERTY);
		uint32_t cols = PKVALUE_UINT32(c);

		return (i / cols);
	}

	// Not found
	return -1;
}

int32_t PKGridLayout::getColAt(int x)
{
	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return -1;
	}

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	this->computeLayout();
    int w = this->getW();

    if(x < margin)
    {
        return -1;
    }

    if(x > (w - margin))
    {
        return -1;
    }

    int offset = (x - margin);

    for(int32_t n=0; n<numCols; n++)
    {
        if(offset < this->columnWidths[n])
        {
            return n;
        }

        offset -= this->columnWidths[n];

        if(offset < spacing)
        {
            return -1;
        }

        offset -= spacing;
    }

    return -1;
}

int32_t PKGridLayout::getRowAt(int y)
{
	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return -1;
	}

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	this->computeLayout();
    int h = this->getH();

    if(y < margin)
    {
        return -1;
    }

    if(y > (h - margin))
    {
        return -1;
    }

    int offset = (y - margin);

    for(int32_t n=0; n<numRows; n++)
    {
        if(offset < this->rowHeights[n])
        {
            return n;
        }

        offset -= this->rowHeights[n];

        if(offset < spacing)
        {
            return -1;
        }

        offset -= spacing;
    }

    return -1;
}

int32_t PKGridLayout::getXForChildAtPos(int32_t col, int32_t row)
{
	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return -1;
	}

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	if(col > numCols)
	{
		return -1;
	}

	this->computeLayout();

	uint32_t result = margin;

	for(uint32_t i=0; i < col; i++)
	{
		result += this->columnWidths[i];
	}

	result += (col*spacing);

	return this->getX() + result;
}

int32_t PKGridLayout::getYForChildAtPos(int32_t col, int32_t row)
{
	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return -1;
	}

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	if(row > numRows)
	{
		return -1;
	}

	this->computeLayout();

	uint32_t result = margin;

	for(uint32_t i=0; i < row; i++)
	{
		result += this->rowHeights[i];
	}

	result += (row*spacing);

	return this->getY() + result;
}

void PKGridLayout::insertNewChildAt(int32_t col, int32_t row, PKObject *child)
{
	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return;
	}

	if(child == NULL)
	{
		return;
	}

	int offset = row*cols + col;
	this->insertChildAt(offset, child);
}

void PKGridLayout::exchangeChildren(PKObject *child1, PKObject *child2)
{
    int off1 = this->getIndexOfChild(child1);
    int off2 = this->getIndexOfChild(child2);

    if(off1 != -1 && off2 != -1)
    {
        this->replaceChildAt(off1, child2);
        this->replaceChildAt(off2, child1);
    }
}

uint32_t PKGridLayout::getMinimumWidth()
{
	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return 0;
	}

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	this->computeLayout();

	// We return the sum of the largest
	// of the children in each column
	// plus the margin & spacing

	uint32_t result = 2 * margin;

	for(uint32_t i=0; i < numCols; i++)
	{
		result += this->columnWidths[i];
	}

	result += ((numCols-1)*spacing);

	return result;
}

uint32_t PKGridLayout::getMinimumHeight()
{
	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return 0;
	}

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	this->computeLayout();

	// We return the sum of the highest
	// of the children in each row
	// plus the margin & spacing

	uint32_t result = 2 * margin;

	for(uint32_t i=0; i < numRows; i++)
	{
		result += this->rowHeights[i];
	}

	result += ((numRows-1)*spacing);

	return result;
}

void PKGridLayout::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    this->refresh = true;
    PKControl::selfPropertyChanged(prop, oldValue);
}

void PKGridLayout::computeLayout()
{
    int32_t childCount = this->getChildrenCount();

    if(childCount == this->cached_children_count && !this->refresh)
    {
        // no need to update
        return;
    }

    this->refresh = false;
    this->cached_children_count = this->getChildrenCount();

	this->rowHeights.clear();
	this->columnWidths.clear();

	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return;
	}

	// Widths

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	for(uint32_t i=0; i < numCols; i++)
	{
		this->columnWidths.push_back(0);
	}

	for(uint32_t i=0; i < numCols; i++)
	{
		for(uint32_t j=0; j < numRows; j++)
		{
			uint32_t index = j*numCols+i;

			if(index < count)
			{
				PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(index));

				if(child != NULL)
				{
                    PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                    bool hidden  = PKVALUE_BOOL(h);

                    if(!hidden)
                    {
					    uint32_t mw = child->getMinimumWidth();
    					
					    if(mw > this->columnWidths[i])
					    {
						    this->columnWidths[i] = mw;
					    }
                    }
				}
			}
		}
	}

	// Heights

	for(uint32_t i=0; i < numRows; i++)
	{
		this->rowHeights.push_back(0);
	}

	for(uint32_t i=0; i < numRows; i++)
	{
		for(uint32_t j=0; j < numCols; j++)
		{
			uint32_t index = i*numCols+j;

			if(index < count)
			{
				PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(index));

				if(child != NULL)
				{
                    PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                    bool hidden  = PKVALUE_BOOL(h);

                    if(!hidden)
                    {
					    uint32_t mh = child->getMinimumHeight();
    					
					    if(mh > this->rowHeights[i])
					    {
						    this->rowHeights[i] = mh;
					    }
                    }
				}
			}
		}
	}
}

void PKGridLayout::move(int32_t x, int32_t y)
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	PKRect frame  = PKVALUE_RECT(fr);

	int32_t diffX = x - frame.x;
	int32_t diffY = y - frame.y;

	frame.x = x;
	frame.y = y;

	PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
	this->set(FRAME_RECT_PROPERTY, &newFrame);

#ifdef LINUX

	if(!this->noWidget) 
	{
        return;
    }

#endif

    for(uint32_t i=0; i < this->getChildrenCount(); i++)
    {
        PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

        if(child)
        {
            child->moveBy(diffX, diffY);
        }
    }
}

void PKGridLayout::relayout()
{
    int w = this->getMinimumWidth();
    int h = this->getMinimumHeight();
	this->resize(w, h);
}

void PKGridLayout::resize(uint32_t w, 
					      uint32_t h)
{
    // Forward

    PKControl::resize(w,h);

#ifdef LINUX
	if(!this->noWidget) 
	{
        return;
    }
#endif

    // Now proceed

	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	PKVariant *c = this->get(COLS_UINT32_PROPERTY);
	uint32_t cols = PKVALUE_UINT32(c);

	if(cols == 0)
	{
		return;
	}

	// We need to resize the elements
	// according to their sizing policy

	unsigned int count = this->getChildrenCount();

	uint32_t div = count / cols;
	uint32_t mod = count % cols;

	uint32_t numRows = div + (mod ? 1 : 0);
	uint32_t numCols = cols;

	for(uint32_t index=0; index<count; index++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(index));

		if(child != NULL)
		{
			uint32_t row = index / numCols;
			uint32_t col = index % numCols;

			PKLayoutPolicy px = child->getLayoutPolicyX();
			PKLayoutPolicy py = child->getLayoutPolicyY();

			uint32_t ww = 0;
			uint32_t hh = 0;

			if(px == PK_LAYOUT_MIN)
			{
				ww = child->getMinimumWidth();
			}
			if(px == PK_LAYOUT_FIXED)
			{
				ww = child->getW();
			}
			if(px == PK_LAYOUT_MAX)
			{
				ww = this->columnWidths[col];
			}

			if(py == PK_LAYOUT_MIN)
			{
				hh = child->getMinimumHeight();
			}
			if(py == PK_LAYOUT_FIXED)
			{
				hh = child->getH();
			}
			if(py == PK_LAYOUT_MAX)
			{
				hh = this->rowHeights[row];
			}

			int xPos = this->getX() + margin;

			for(uint32_t n=0; n < col; n++)
			{
				xPos += this->columnWidths[n];
				xPos += spacing;
			}

			int yPos = this->getY() + margin;

			for(uint32_t n=0; n < row; n++)
			{
				yPos += this->rowHeights[n];
				yPos += spacing;
			}

			// TODO : have proper options for cell alignment
			// so that items don't have to be centered (same for X)

			yPos += (this->rowHeights[row] - hh) / 2;

			child->move(xPos, yPos);
			child->resize(ww, hh);
		}
	}
}	

void PKGridLayout::show()
{
#ifdef LINUX
	if(!this->noWidget) 
	{
        PKControl::show();
        return;
    }
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			child->show();
		}
	}
}

void PKGridLayout::hide()
{
#ifdef LINUX
	if(!this->noWidget) 
	{
        PKControl::hide();
        return;
    }
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			child->hide();
		}
	}
}

void PKGridLayout::enable()
{
#ifdef LINUX
	if(!this->noWidget) 
	{
        PKControl::enable();
        return;
    }
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			child->enable();
		}
	}
}

void PKGridLayout::disable()
{
#ifdef LINUX
	if(!this->noWidget) 
	{
        PKControl::disable();
        return;
    }
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			child->disable();
		}
	}
}

#ifdef WIN32

bool PKGridLayout::relayWIN32Event(HWND   parent,
                               UINT   msg, 
						       WPARAM wParam, 
						       LPARAM lParam)
{
    bool hit = false;

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			hit = child->relayWIN32Event(parent, msg, wParam, lParam);

            if(hit)
            {
                return true;
            }
		}
	}

    return false;
}

#endif
