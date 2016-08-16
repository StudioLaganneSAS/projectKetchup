//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKLayout.h"

//
// Properties
//

std::string PKLayout::MARGIN_INT32_PROPERTY  = "margin";
std::string PKLayout::SPACING_INT32_PROPERTY = "spacing";
std::string PKLayout::TYPE_INT32_PROPERTY    = "type";


//
// PKLayout
// 

PKLayout::PKLayout()
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
	PKOBJECT_ADD_INT32_PROPERTY(type, PK_LAYOUT_HORIZONTAL);
}

PKLayout::~PKLayout()
{
}

void PKLayout::build()
{
#ifdef LINUX

	PKVariant *t = this->get(TYPE_INT32_PROPERTY);
	PKLayoutType type = (PKLayoutType) PKVALUE_INT32(t);
  
	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

    if(!this->noWidget)
    {
        if(type == PK_LAYOUT_HORIZONTAL)
        {
            this->widget = gtk_hbox_new(FALSE, spacing);
        }
        else
        {
            this->widget = gtk_vbox_new(FALSE, spacing);        
        }

        if(this->widget != NULL)
        {
            gtk_container_set_border_width(GTK_CONTAINER(this->widget), margin);
            gtk_box_set_spacing(GTK_BOX(this->widget), spacing);   
        }    
    }

#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->build();
		}
	}
	
#ifdef LINUX
    
    // Add the children
    
    if(!this->noWidget)
    {
	    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	    {
		    PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		    if(child)
		    {
		        GtkWidget *w  = child->getWindowHandle();
                gboolean fill = FALSE;
                
                if(type == PK_LAYOUT_HORIZONTAL)
                {
                    fill = ((child->getLayoutPolicyX() == PK_LAYOUT_MAX) ? TRUE : FALSE);
                }           

                if(type == PK_LAYOUT_VERTICAL)
                {
                    fill = ((child->getLayoutPolicyY() == PK_LAYOUT_MAX) ? TRUE : FALSE);
                }           

                gtk_box_pack_start(GTK_BOX(this->widget), w, fill, fill, 0);		    
            }
        }    
    }    

#endif	
}

void PKLayout::initialize(PKDialog *dialog)
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

void PKLayout::destroy()
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

PKWindowHandle PKLayout::getWindowHandle()
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



void PKLayout::setLayoutType(PKLayoutType type)
{
	PKVariantInt32 t(type);
	this->set(TYPE_INT32_PROPERTY, &t);
}

PKLayoutType PKLayout::getLayoutType()
{
	PKVariant *type = this->get(TYPE_INT32_PROPERTY);
	return (PKLayoutType) PKVALUE_INT32(type);
}	

void PKLayout::setMargin(uint32_t margin)
{
	PKVariantInt32 m(margin);
	this->set(MARGIN_INT32_PROPERTY, &m);
}

void PKLayout::setSpacing(uint32_t spacing)
{
	PKVariantInt32 s(spacing);
	this->set(SPACING_INT32_PROPERTY, &s);
}

uint32_t PKLayout::getMinimumWidth()
{
	PKLayoutType type = this->getLayoutType();

	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	if(type == PK_LAYOUT_HORIZONTAL)
	{
		// We return the sum of the width
		// of the children, plus the margin

		uint32_t result = 2 * margin;

		unsigned int count = this->getChildrenCount();

		for(unsigned int i=0; i < count; i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child != NULL)
			{
                PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                bool hidden  = PKVALUE_BOOL(h);

                if(!hidden)
                {
    				result += child->getMinimumWidth();
                }
			}
		}

		if(count > 0)
		{
			result += ((count - 1) * spacing);
		}

		return result;
	}
	else
	{
		// We return the max of our children
		// minimum width as our own width

		uint32_t result = 0;

		unsigned int count = this->getChildrenCount();

		for(unsigned int i=0; i < count; i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child != NULL)
			{
                PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                bool hidden  = PKVALUE_BOOL(h);

                if(!hidden)
                {
				    uint32_t w = child->getMinimumWidth();

				    if(w > result)
				    {
					    result = w;
				    }
                }
			}
		}

		return (2*margin + result);
	}
}

uint32_t PKLayout::getMinimumHeight()
{
	PKLayoutType type = this->getLayoutType();

	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	if(type == PK_LAYOUT_VERTICAL)
	{
		// We return the sum of the height
		// of the children, plus the margin

		uint32_t result = 2 * margin;

		unsigned int count = this->getChildrenCount();

		for(unsigned int i=0; i < count; i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child != NULL)
			{
                PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                bool hidden  = PKVALUE_BOOL(h);

                if(!hidden)
                {
    				result += child->getMinimumHeight();
                }
			}
		}

		if(count > 0)
		{
			result += ((count - 1) * spacing);
		}

		return result;
	}
	else
	{
		// We return the max of our children
		// minimum height as our own width

		uint32_t result = 0;

		unsigned int count = this->getChildrenCount();

		for(unsigned int i=0; i < count; i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child != NULL)
			{
                PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                bool hidden  = PKVALUE_BOOL(h);

                if(!hidden)
                {
				    uint32_t h = child->getMinimumHeight();

				    if(h > result)
				    {
					    result = h;
				    }
                }
			}
		}

		return (2*margin + result);
	}
}

void PKLayout::resize(uint32_t w, 
					  uint32_t h)
{
    // Forward

    PKControl::resize(w,h);

#ifdef LINUX
    if(!this->noWidget)
    {
        // Layouting is implemented for us by GTK+
        return;
    }
#endif

    // Now proceed

	PKLayoutType type = this->getLayoutType();

	PKVariant *m = this->get(MARGIN_INT32_PROPERTY);
	int32_t margin = PKVALUE_INT32(m);

	PKVariant *s = this->get(SPACING_INT32_PROPERTY);
	int32_t spacing = PKVALUE_INT32(s);

	// We need to resize the elements
	// according to their sizing policy

	if(type == PK_LAYOUT_HORIZONTAL)
	{
		// Get the total minimum size 
		// of all our elements so we can know
		// how much to allocate to elements
		// that stretch (if any)

		int32_t minWidth  = (int32_t) this->getMinimumWidth();
		int32_t minHeight = (int32_t) this->getMinimumHeight() - 2*margin;

		unsigned int ccount = this->getChildrenCount();

		if(w < (uint32_t) minWidth)
		{
			return;
		}

		int32_t extra = (w - minWidth);
		int32_t count = 0;

		for(unsigned int i=0; i < ccount; i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child != NULL)
			{
                PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                bool hidden  = PKVALUE_BOOL(h);

                if(!hidden)
                {
				    PKLayoutPolicy lay = child->getLayoutPolicyX();

				    if(lay == PK_LAYOUT_MAX)
				    {
					    count++;
				    }
                }
			}
		}

		int32_t pad = extra;

		if(count >= 1)
		{
			pad = extra / count;
		}

		int x = this->getX() + margin;

		for(unsigned int i=0; i < this->getChildrenCount(); i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child == NULL)
			{
				continue; 
			}

            PKVariant *hd = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
            bool hidden  = PKVALUE_BOOL(hd);

            if(hidden)
            {
                continue;
            }

			PKLayoutPolicy lay = child->getLayoutPolicyX();

			int mw = child->getMinimumWidth();
			int mh = child->getMinimumHeight();

			PKLayoutPolicy ylay = child->getLayoutPolicyY();

			int ch = child->getH();

			if(ylay == PK_LAYOUT_MIN)
			{
				ch = mh;
			}

			int y = this->getY() + margin + (minHeight - ch) / 2;

            if(ylay == PK_LAYOUT_MAX)
			{
				ch = h - 2*margin;
                y = this->getY() + margin;
            }

			child->move(x, y);

			if(lay == PK_LAYOUT_MIN)
			{
				child->resize(mw, ch);
				x += mw;
			}
			else if(lay == PK_LAYOUT_FIXED)
			{
				child->resize(child->getW(), ch);
				x += child->getW();
			}
			else if(lay == PK_LAYOUT_MAX)
			{
				child->resize(mw + pad, ch);
				x += (mw + pad);
			}

			x += spacing;
		}
	}
	else
	{
		// Get the total minimum size 
		// of all our elements so we can know
		// how much to allocate to elements
		// that stretch (if any)

		int32_t minWidth  = (int32_t) this->getMinimumWidth()  - 2*margin;
		int32_t minHeight = (int32_t) this->getMinimumHeight();

		unsigned int ccount = this->getChildrenCount();

		if(h < (uint32_t) minHeight)
		{
			return;
		}

		int32_t extra = (h - minHeight);
		int32_t count = 0;

		for(unsigned int i=0; i < ccount; i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child != NULL)
			{
                PKVariant *h = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
                bool hidden  = PKVALUE_BOOL(h);

                if(!hidden)
                {
				    PKLayoutPolicy lay = child->getLayoutPolicyY();

				    if(lay == PK_LAYOUT_MAX)
				    {
					    count++;
				    }
                }
			}
		}

		int32_t pad = extra;

		if(count >= 1)
		{
			pad = extra / count;
		}

		int y = this->getY() + margin;

		for(unsigned int i=0; i < this->getChildrenCount(); i++)
		{
			PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

			if(child == NULL)
			{
				continue; 
			}

            PKVariant *hd = child->get(PKControl::HIDDEN_BOOL_PROPERTY);
            bool hidden  = PKVALUE_BOOL(hd);

            if(hidden)
            {
                continue;
            }

			PKLayoutPolicy lay = child->getLayoutPolicyY();

			int mw = child->getMinimumWidth();
			int mh = child->getMinimumHeight();

			PKLayoutPolicy xlay = child->getLayoutPolicyX();

			int cw = child->getW();

			int x = this->getX() + margin;
			
			if(xlay == PK_LAYOUT_MIN)
			{
				cw = mw;
				x = this->getX() + margin + (minWidth - cw) / 2;
			}

			if(xlay == PK_LAYOUT_MAX)
			{
				cw = w - 2*margin;
			}

			child->move(x, y);

			if(lay == PK_LAYOUT_MIN)
			{
				child->resize(cw, mh);
				y += mh;
			}
			else if(lay == PK_LAYOUT_FIXED)
			{
				child->resize(cw, child->getH());
				y += child->getH();
			}
			else if(lay == PK_LAYOUT_MAX)
			{
				child->resize(cw, mh + pad);
				y += (mh + pad);
			}

			y += spacing;
		}
	}
}	

void PKLayout::show()
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

void PKLayout::hide()
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

void PKLayout::enable()
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

void PKLayout::disable()
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

bool PKLayout::relayWIN32Event(HWND   parent,
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
