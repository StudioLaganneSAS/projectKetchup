//
// PKVectorControl
//

#include "PKVectorControl.h"
#include "PKVectorEngine.h"
#include "PKVectorView.h"
#include "PKVectorControlRoot.h"
#include "PKDialog.h"

//
// Properties
//

std::string PKVectorControl::ALPHA_DOUBLE_PROPERTY = "alpha";
std::string PKVectorControl::SHARED_VECTOR_ENGINE_BOOL_PROPERTY = "sharedVectorEngine";
std::string PKVectorControl::DOUBLE_BUFFER_BOOL_PROPERTY = "doubleBuffer";

//
// PKVectorControl
//

PKVectorControl::PKVectorControl()
{
	PKOBJECT_ADD_DOUBLE_PROPERTY(alpha, 1.0);
	PKOBJECT_ADD_BOOL_PROPERTY(sharedVectorEngine, false);
	PKOBJECT_ADD_BOOL_PROPERTY(doubleBuffer, false);

#ifdef WIN32
	this->hwnd = NULL;

	this->mdc = NULL;
	this->bmp = NULL;
#endif
	
	this->controlRoot = NULL;
}

PKVectorControl::~PKVectorControl()
{
#ifdef WIN32

	if(this->bmp != NULL)
	{
		DeleteObject(this->bmp);
		this->bmp = NULL;
	}

	if(this->mdc != NULL)
	{
		DeleteDC(this->mdc);
		this->mdc = NULL;
	}

#endif
}

void PKVectorControl::build()
{
	PKVariant *s = this->get(PKVectorControl::SHARED_VECTOR_ENGINE_BOOL_PROPERTY);
	bool shared  = PKVALUE_BOOL(s);

	if(!shared)
	{
		PKCustomControl::build();
	}
	else
	{
#ifdef LINUX
        this->setNoWidget(true);
#endif
	}
	
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(root)
		{
			root->build();
		}
	}
}

void PKVectorControl::initialize(PKDialog *dialog)
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(root)
		{
			root->initialize(dialog);
		}
	}
	
    PKCustomControl::initialize(dialog);
}

void PKVectorControl::destroy()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(root)
		{
			root->destroy();
            root = NULL;
		}
	}

    this->clearChildren();

    PKCustomControl::destroy();
}

void PKVectorControl::setControlRoot(PKVectorControlRoot *r)
{
	this->controlRoot = r;
}

//
// Main draw method
//

void PKVectorControl::drawVector(PKVectorEngine *engine,
								 int32_t x, int32_t y)
{
	// Subclasses will implement
#ifdef _DEBUG
	// Draw something ?
#endif
}

//
// Main draw method (Internal)
//

void PKVectorControl::drawVectorInternal(PKVectorEngine *engine,
										 int32_t x, int32_t y)
{
	// Setup vector engine

	PKVariant *a = this->get(PKVectorControl::ALPHA_DOUBLE_PROPERTY);
	double alpha   = PKVALUE_DOUBLE(a);
	
	PKVariant *s = this->get(PKVectorControl::SHARED_VECTOR_ENGINE_BOOL_PROPERTY);
	bool shared  = PKVALUE_BOOL(s);

	if(!shared)
	{
		engine->setGlobalAlpha(alpha);
	}

	// Draw self
	this->drawVector(engine, x, y);

	// Draw children
	
	uint32_t count = this->getChildrenCount();	

	for(uint32_t i=0; i<count; i++)
	{
		PKVectorView *view = dynamic_cast<PKVectorView *>(this->getChildAt(i));

		if(view)
		{
			view->drawVector(engine, x, y);
		}
	}
}

void PKVectorControl::move(int32_t x, 
		                   int32_t y)
{
	PKCustomControl::move(x, y);

	if(this->getChildrenCount() == 1)
	{
	    PKControl *ctrl = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(ctrl)
        {
			ctrl->move(x, y);
        }
	}
}

void PKVectorControl::resize(uint32_t w, 
			                 uint32_t h)
{
	PKCustomControl::resize(w, h);

#ifdef WIN32

	if(this->bmp != NULL)
	{
		DeleteObject(this->bmp);
		this->bmp = NULL;
	}

	if(this->mdc != NULL)
	{
		DeleteDC(this->mdc);
		this->mdc = NULL;
	}

	if(this->hwnd)
	{
		HDC dc = GetDC(this->hwnd);

		this->mdc = CreateCompatibleDC(dc);
		this->bmp = CreateCompatibleBitmap(dc, w, h);
	}

#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->resize(w, h);
		}
	}
}

void PKVectorControl::show()
{
	PKCustomControl::show();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->show();
		}
	}
}

void PKVectorControl::hide()
{
	PKCustomControl::hide();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->hide();			
		}
	}
}

void PKVectorControl::enable()
{
	PKCustomControl::enable();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->enable();
		}
	}
}

void PKVectorControl::disable()
{
	PKCustomControl::disable();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->disable();
		}
	}
}

uint32_t PKVectorControl::getMinimumWidth()
{
    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    {
        return this->getW();
    }

    if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumWidth());
		}
	}

    return PKCustomControl::getMinimumWidth();
}

uint32_t PKVectorControl::getMinimumHeight()
{
    if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
    {
        return this->getH();
    }

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumHeight());
		}
	}

	return PKCustomControl::getMinimumHeight();
}
    
void PKVectorControl::invalidate(bool recurse)
{
	if(!this->dialog)
	{
		return;
	}

	if(!this->dialog->isShowing())
	{
		return;
	}

	PKVariant *s = this->get(PKVectorControl::SHARED_VECTOR_ENGINE_BOOL_PROPERTY);
	bool shared  = PKVALUE_BOOL(s);

	if(shared)
	{
		// We need to invalidate our root
		// vector control instead of us
		
		if(this->controlRoot)
		{
			PKRect client;

			client.x = this->getX() - this->controlRoot->getX(); 
			client.y = this->getY() - this->controlRoot->getY();
			client.w = this->getW();
			client.h = this->getH();
			
#ifdef MACOSX 
			client.x -= this->getScrollOriginX(); 
			client.y -= this->getScrollOriginY();			
#endif

			this->controlRoot->invalidateRect(client);
		}
	}
	else
	{
		PKCustomControl::invalidate(recurse);
	}
}

bool PKVectorControl::wantsKeyboardFocus()
{
	// reimplement
	return false;
}

// 
// From PKCustomControl
//

#ifdef WIN32

void PKVectorControl::drawWin32(PKDrawingDevice device, 
								PKRect          rect)
{
	PKVariant *d = this->get(PKVectorControlRoot::DOUBLE_BUFFER_BOOL_PROPERTY);
	bool db = PKVALUE_BOOL(d);

	uint32_t w = this->getW();
	uint32_t h = this->getH();

	if(db)
	{
		PKVectorEngine engine;

		HANDLE hold = SelectObject(mdc, bmp);

		engine.startWithDevice(mdc);
		engine.setDimensions(w, h);
		engine.clipToRect(rect);
		this->drawVectorInternal(&engine, 0, 0);
		engine.end();

		BitBlt(device, rect.x, rect.y, rect.w, rect.h, 
			   mdc, rect.x, rect.y, SRCCOPY);

		SelectObject(mdc, hold);
	}
	else
	{
		PKVectorEngine engine;
		engine.startWithDevice(device);
		engine.setDimensions(w, h);
		engine.clipToRect(rect);
		this->drawVectorInternal(&engine, 0, 0);
		engine.end();
	}
}

#endif

#ifdef LINUX

void PKVectorControl::drawLinux(GtkWidget   *widget,
                                GdkDrawable *drawable,
                                GdkGC       *gc,
                                PKRect       rect)
{
    GtkAllocation all;
    gtk_widget_get_allocation(widget, &all);

	PKVectorEngine engine;
	engine.startWithDevice(drawable);
	engine.setDimensions(all.width, all.height);
	this->drawVectorInternal(&engine, 0, 0);
	engine.end();
}

#endif

#ifdef MACOSX

void PKVectorControl::drawMacOSX(RgnHandle	 inLimitRgn,
								 CGContextRef inContext)
{
	PKVectorEngine engine;
	engine.startWithCGContext(inContext);
	engine.setDimensions(this->getW(), this->getH());
	this->drawVectorInternal(&engine, 0, 0);
	engine.end();
}

#endif
