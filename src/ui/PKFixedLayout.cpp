//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFixedLayout.h"

//
// PKFixedLayout
// 

PKFixedLayout::PKFixedLayout()
{
}

PKFixedLayout::~PKFixedLayout()
{
}

void PKFixedLayout::build()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->build();
		}
	}

#ifdef LINUX

    this->widget = gtk_fixed_new();
    
    if(this->widget != NULL)
    {
	    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	    {
		    PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		    if(child)
		    {
		        GtkWidget *w = child->getWindowHandle();
		        gtk_fixed_put(GTK_FIXED(this->widget), 
		                      w, child->getX(), child->getY());
		    }
	    }        
    }    

#endif
}

void PKFixedLayout::initialize(PKDialog *dialog)
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->initialize(dialog);
		}
	}
	
#ifdef LINUX
	PKControl::initialize(dialog);	
#endif
}

void PKFixedLayout::destroy()
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

PKWindowHandle PKFixedLayout::getWindowHandle()
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

uint32_t PKFixedLayout::getMinimumWidth()
{
    // Not used ?
    return 0;
}

uint32_t PKFixedLayout::getMinimumHeight()
{
    // Not used ?
    return 0;
}

void PKFixedLayout::move(int32_t x, 
		                 int32_t y)
{
    PKControl::move(x,y);

 	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            int x = 0; //this->getAbsoluteX();
            int y = 0; //this->getAbsoluteY();

            int cx = child->getX();
            int cy = child->getY();

            child->move(x+cx, y+cy);
		}
	}
}

void PKFixedLayout::resize(uint32_t w, 
					       uint32_t h)
{
    // Do nothing special
    PKControl::resize(w, h);
}	

void PKFixedLayout::show()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			child->show();
		}
	}
}

void PKFixedLayout::hide()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			child->hide();
		}
	}
}

void PKFixedLayout::enable()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child != NULL)
		{
			child->enable();
		}
	}
}

void PKFixedLayout::disable()
{
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

bool PKFixedLayout::relayWIN32Event(HWND   parent,
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

