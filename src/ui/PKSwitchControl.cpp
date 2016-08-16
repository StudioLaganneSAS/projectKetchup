//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKSwitchControl.h"
#include "PKStr.h"
#include "PKCursorTools.h"
#include "PKDialog.h"

#pragma warning(disable : 4800)

//
// PKSwitchControl
//

PKSwitchControl::PKSwitchControl()
{
	this->currentTab = 0;
}

PKSwitchControl::~PKSwitchControl()
{
}

void PKSwitchControl::build()
{
    // Build children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->build();
		}
	}
}

void PKSwitchControl::initialize(PKDialog *dialog)
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

    if(this->getChildrenCount() > 0)
    {
        this->setCurrentTab(0);
    }
}

void PKSwitchControl::destroy()
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

PKWindowHandle PKSwitchControl::getWindowHandle()
{
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

void PKSwitchControl::clearTabs()
{
    this->clearChildren();
}

void PKSwitchControl::addTab(PKLayout *object)
{
	if(object == NULL)
	{
		return;
	}

    this->addChild(object);
}

void PKSwitchControl::setCurrentTab(unsigned int index)
{
    if(index >= this->getChildrenCount())
	{
		return;
	}

	this->currentTab = index;

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

uint32_t PKSwitchControl::getMinimumWidth()
{
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

	return minClientWidth;
}

uint32_t PKSwitchControl::getMinimumHeight()
{
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

	return minClientHeight;
}

void PKSwitchControl::move(int32_t x, 
						   int32_t y)
{
	PKControl::move(x, y);

	// Move children

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
    		ctrl->move(x, y);
	    }
    }
}

void PKSwitchControl::resize(uint32_t w, 
						     uint32_t h)
{
	PKControl::resize(w, h);

	// Move children

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
        PKObject *child = this->getChildAt(i);
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl)
        {
    		ctrl->resize(w, h);
	    }
    }
}

void PKSwitchControl::show()
{
	PKControl::show();

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

void PKSwitchControl::hide()
{
	PKControl::hide();

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

void PKSwitchControl::enable()
{
	PKControl::enable();

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

void PKSwitchControl::disable()
{
	PKControl::disable();

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

bool PKSwitchControl::relayWIN32Event(HWND   parent,
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
