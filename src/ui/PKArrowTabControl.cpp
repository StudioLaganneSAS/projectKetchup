

#include "PKArrowTabControl.h"
#include "PKImageLoader.h"
#include "PKCursorTools.h"
#include "PKDialog.h"

#include "PKEdit.h"
#include "PKCheckBox.h"
#include "PKComboBox.h"
#include "PKButton.h"
#include "PKVectorEngine.h"

//
// Properties
//

std::string PKArrowTabControl::BKG_IMAGE_WSTRING_PROPERTY    = "bkgImage";
std::string PKArrowTabControl::ARROW_IMAGE_WSTRING_PROPERTY  = "arrowImage";
std::string PKArrowTabControl::ICONS_WSTRINGLIST_PROPERTY    = "icons";
std::string PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY   = "currentTab";
std::string PKArrowTabControl::TOOLTIPS_WSTRINGLIST_PROPERTY = "tooltips";

//
// PKArrowTabControl
//

PKArrowTabControl::PKArrowTabControl()
{
    this->built = false;

    std::vector<std::wstring> empty;
    PKOBJECT_ADD_WSTRINGLIST_PROPERTY(icons, empty);
    PKOBJECT_ADD_UINT32_PROPERTY(currentTab, 0);
    PKOBJECT_ADD_WSTRINGLIST_PROPERTY(tooltips, empty);
	PKOBJECT_ADD_WSTRING_PROPERTY(bkgImage, L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(arrowImage, L"IMG_NOT_SET");
    
#ifdef LINUX
    this->tab  = NULL;
    this->hand = gdk_cursor_new(GDK_HAND2);
#else
	PKVariantBool db(true);
	this->set(PKVectorControl::DOUBLE_BUFFER_BOOL_PROPERTY, &db);
#endif

    this->bkg = NULL;
    this->arrow = NULL;

	this->bar_height = 0;
}

PKArrowTabControl::~PKArrowTabControl()
{
    for(uint32_t i=0; i<this->icons.size(); i++)
    {
        if(this->icons[i])
        {
            delete this->icons[i];
        }
    }
	
    this->icons.clear();

#ifdef LINUX
    gdk_cursor_unref(this->hand);
#endif
	
	if(this->bkg != NULL)
	{
		delete this->bkg;
	}

	if(this->arrow != NULL)
	{
		delete this->arrow;
	}
}

void PKArrowTabControl::build()
{
    if(this->built)
    {
        return;
    }

#ifdef LINUX
    PKCustomContainer::build();
#else
    PKVectorControl::build();
#endif

#ifdef WIN32
	LONG_PTR style = GetWindowLongPtr(this->hwnd, GWL_STYLE);
	style = (style | WS_CLIPSIBLINGS);
	SetWindowLongPtr(this->hwnd, GWL_STYLE, style);
#endif
	
	PKVariant *b = this->get(PKArrowTabControl::BKG_IMAGE_WSTRING_PROPERTY);
	std::wstring bkgUrl = PKVALUE_WSTRING(b);
	
	PKImageURIType type;
	std::wstring   image;
	
	PKImage::processURI(bkgUrl, &type, &image);
	
	if(type == PKIMAGE_URI_RESOURCE)
	{
		this->bkg = PKImageLoader::loadBitmapFromResource(image);
	}
	
	if(type == PKIMAGE_URI_FILENAME)
	{
		this->bkg = PKImageLoader::loadBitmapFromFilename(image);
	}
	
	if(this->bkg != NULL)
	{
		this->bar_height = this->bkg->getHeight();
	}
	
	PKVariant *a = this->get(PKArrowTabControl::ARROW_IMAGE_WSTRING_PROPERTY);
	std::wstring arrowUrl = PKVALUE_WSTRING(a);
		
	PKImage::processURI(arrowUrl, &type, &image);
	
	if(type == PKIMAGE_URI_RESOURCE)
	{
		this->arrow = PKImageLoader::loadBitmapFromResource(image);
	}
	
	if(type == PKIMAGE_URI_FILENAME)
	{
		this->arrow = PKImageLoader::loadBitmapFromFilename(image);
	}

    PKVariant *i = this->get(PKArrowTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);
	
    // Create HICONS for the icons we have
	
    for(uint32_t i=0; i<icons.size(); i++)
    {
		PKImage *bitmap = NULL;
		
		PKImageURIType type;
		std::wstring   image;
		
		PKImage::processURI(icons[i], &type, &image);
		
		if(type == PKIMAGE_URI_RESOURCE)
		{
			bitmap = PKImageLoader::loadBitmapFromResource(image);
		}
		
		if(type == PKIMAGE_URI_FILENAME)
		{
			bitmap = PKImageLoader::loadBitmapFromFilename(image);
		}
		
		if(bitmap != NULL)
		{
			this->icons.push_back(bitmap);
		}
	}

#ifdef LINUX

    // Create HICONS for the icons we have
	
    for(uint32_t i=0; i<icons.size(); i++)
    {
		PKImage *bitmap = NULL;
		
		PKImageURIType type;
		std::wstring   image;
		
		PKImage::processURI(icons[i], &type, &image);
		
		if(type == PKIMAGE_URI_RESOURCE)
		{
			bitmap = PKImageLoader::loadBitmapFromResource(image);
		}
		
		if(type == PKIMAGE_URI_FILENAME)
		{
			bitmap = PKImageLoader::loadBitmapFromFilename(image);
		}
		
		if(bitmap != NULL)
		{
            // Invert R & B

            uint32_t width  = bitmap->getWidth();
            uint32_t height = bitmap->getWidth();
            
            uint8_t *pdata  = (uint8_t *) bitmap->getPixels();

            for(unsigned int j=0; j < height; j++)
            {
                for(unsigned int i=0; i < width; i++)
                {
                    uint8_t temp = pdata[0];
                     
	                pdata[0] = pdata[2];
	                pdata[2] = temp;

	                pdata +=4;
                }
            }

            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*) bitmap->getPixels(),
                                                    GDK_COLORSPACE_RGB,
                                                    TRUE, 8, width, height, 
                                                    width*4, 
                                                    NULL, NULL);
                                                    
            this->bitmaps.push_back(pixbuf);
		}
	}

    // Create a tab child and add children to it
    
    this->tab = gtk_notebook_new();
    
    if(this->tab != NULL)
	{
        gtk_notebook_set_show_tabs(GTK_NOTEBOOK(this->tab), FALSE);
        gtk_notebook_set_show_border(GTK_NOTEBOOK(this->tab), FALSE);
        
        gtk_widget_show(this->tab);
        gtk_layout_put(GTK_LAYOUT(this->layout), this->tab, 0, 24);
	}

#endif
	
#ifdef LINUX		
    // Build children

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
		    gtk_notebook_append_page(GTK_NOTEBOOK(this->tab), 
		                             child->getWindowHandle(),
		                             NULL);
		}
	}
#endif

    // Force switch to first tab
    this->switchToTab(0);

    // All done!
    this->built = true;
}

void PKArrowTabControl::initialize(PKDialog *dialog)
{
#ifdef LINUX
    PKCustomContainer::initialize(dialog);
#else
    PKVectorControl::initialize(dialog);
#endif

#ifdef LINUX
        GtkRequisition req;
        gtk_widget_size_request(this->tab, &req);            
        gtk_widget_set_size_request(this->widget, req.width, req.height+this->bar_height);        
#endif

	int icon_w = 0;
	
	if(this->arrow != NULL)
	{
		icon_w = this->arrow->getWidth();
	}
	

    // Add tooltips

    PKVariant *t = this->get(PKArrowTabControl::TOOLTIPS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> tooltips = PKVALUE_WSTRINGLIST(t);

    for(uint32_t i=0; i<tooltips.size(); i++)
    {
        PKRect rect;
        rect.x = i*icon_w;
        rect.y = 0;
        rect.w = icon_w;
        rect.h = icon_w;

        this->addTooltip(rect, tooltips[i]);
    }
}

void PKArrowTabControl::destroy()
{
#ifdef LINUX
    PKCustomContainer::destroy();
#else
    PKVectorControl::destroy();
#endif
}

PKWindowHandle PKArrowTabControl::getWindowHandle()
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

void PKArrowTabControlHasFocusChildrenWalk(PKObject *child, void *context)
{
    PKArrowTabControl *icon = (PKArrowTabControl *) context;

    if(icon && icon->getDialog())
    {
        PKControl *ctrl = dynamic_cast<PKControl*>(child);

        if(ctrl != NULL)
        {
            PKControl *focus = icon->getDialog()->getFocusedControl();

            if(focus)
            {
                if(focus == ctrl)
                {
                    icon->hasFocusChild = true;
                }
            }
        }
    }
}

void PKArrowTabControlSetFocusChildrenWalk(PKObject *child, void *context)
{
    PKArrowTabControl *icon = (PKArrowTabControl *) context;

    if(icon && icon->getDialog())
    {
        if(icon->focusSet)
        {
            return;
        }

        PKControl *ctrl = dynamic_cast<PKControl*>(child);
        
        PKCheckBox *box = NULL;
        PKEdit    *edit = NULL;
        PKComboBox  *cb = NULL;
        PKButton    *bt = NULL;

        edit = dynamic_cast<PKEdit*>(child);
#ifndef MACOSX
        box = dynamic_cast<PKCheckBox*>(child);
#endif
        cb = dynamic_cast<PKComboBox*>(child);
        bt = dynamic_cast<PKButton*>(child);

        if(ctrl != NULL && (box != NULL || edit != NULL || cb != NULL || bt != NULL))
        {
            PKVariant *h = ctrl->get(PKControl::HIDDEN_BOOL_PROPERTY);
            bool hidden  = PKVALUE_BOOL(h);

            PKVariant *e   = ctrl->get(PKControl::ENABLED_BOOL_PROPERTY);
            bool disabledd = !PKVALUE_BOOL(e);

            if(!hidden && !disabledd && ctrl->isShowing())
            {
                PKVariant *id = ctrl->get(PKControl::ID_STRING_PROPERTY);
                std::string cid = PKVALUE_STRING(id);
                
                icon->getDialog()->setFocusToControl(cid);
                icon->focusSet = true;
            }
        }
    }
}

void PKArrowTabControl::switchToTab(uint32_t tab)
{
    if(tab >= this->getChildrenCount())
    {
        return;
    }

    // See if one of our children has the docus
    // because if so, we must reset it to the 
    // first child of the new tab

    this->hasFocusChild = false;
    this->walkChildren(PKArrowTabControlHasFocusChildrenWalk, this);

#ifdef LINUX

    if(this->tab != NULL)
    {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(this->tab), tab);

        // Change our property, this will redraw

        PKVariantUInt32 sel(tab);
        this->set(PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY, &sel);
    }
    
    return;

#endif

    for(uint32_t i=0; i<this->getChildrenCount(); i++)
    {
        PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

        if(child)
        {
            if(i==tab)
            {
                child->hide();
                child->show();
            }
            else
            {
                child->hide();
            }
        }
    }   

    // Reset focus if needed

    if(this->hasFocusChild)
    {
        this->focusSet = false;
        this->walkChildren(PKArrowTabControlSetFocusChildrenWalk, this);
    }

    // Change our property, this will redraw

    PKVariantUInt32 sel(tab);
    this->set(PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY, &sel);
}

#ifdef LINUX
void PKArrowTabControl::drawLinux(GtkWidget   *widget,
                                 GdkDrawable *drawable,
                                 GdkGC       *gc,
                                 PKRect       rect)
{
    PKVariant *s = this->get(PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

    int w = rect.w;
    int h = rect.h;

    GdkGCValues values;
    gdk_gc_get_values(gc, &values);
    
    // Draw icons
/*
    for(uint32_t i=0; i < this->bitmaps.size(); i++)
    {
        gdk_draw_pixbuf(drawable, gc, 
                        this->bitmaps[i],
                        0, 0, 5+(i)*icon_w, 5, 16, 16,
                        GDK_RGB_DITHER_NONE, 0, 0);
    }
*/    
 
    // Revert dc setup changes                                             
/*
    gdk_gc_set_foreground(gc, &values.foreground); 
    
    gdk_gc_set_line_attributes(gc,
                               values.line_width,
                               values.line_style,
                               values.cap_style,
                               values.join_style);
*/
}

#endif

void PKArrowTabControl::drawVector(PKVectorEngine *engine,
								   int32_t x, int32_t y)
{
    PKVariant *s = this->get(PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);
	
    int w = this->getW();
    int h = this->getH();
	
#ifdef WIN32
	// Draw the full bkg
	LONG    bkgColor = GetSysColor(COLOR_BTNFACE);
	PKColor rgb      = PKColorCreate((bkgColor & 0xFF), ((bkgColor >> 8) & 0xFF), ((bkgColor >> 16) & 0xFF), 0xFF);
	engine->fillRect(-1, -1, w+1, this->bar_height+1, rgb);
#endif

	// Draw the top bar
	
	if(this->bkg)
	{
		std::vector <unsigned int> offsets;
		offsets.push_back(this->bkg->getWidth()/2);
		engine->fillStretchWidth(x, y, w, this->bkg, offsets);
	}
	
	// Draw the selection
	
	int iw = this->bar_height;

	if(this->arrow)
	{
		iw = this->arrow->getWidth();
		engine->drawImage(this->arrow, x+sel*iw, y);
	}
	/*
	// Draw a line on the left
	
#ifdef MACOSX
	PKColor line = PKColorCreate(128,128,128,255);
#else
	PKColor line = PKColorCreate(178,178,178,255);
#endif
	engine->drawLine(0, 0, 0, h, line, 1);
*/
    // Draw images 
	
    for(uint32_t i=0; i < this->icons.size(); i++)
    {
		int icw = this->icons[i]->getWidth();
		int ich = this->icons[i]->getHeight();
		
		int ix = iw*i + (iw - icw)/2;
		int iy = (iw - ich)/2;
		
		engine->drawImage(this->icons[i], x+ix, y+iy);
    }
}

void PKArrowTabControl::move(int32_t x, 
				             int32_t y)
{
#ifdef LINUX
    PKCustomContainer::move(x, y);
#else
    PKVectorControl::move(x, y);
#endif    

    for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->move(x, y+this->bar_height+1);
		}
	}
}

void PKArrowTabControl::resize(uint32_t w, 
					           uint32_t h)
{
#ifdef LINUX
    PKCustomContainer::resize(w, h);
#endif
#ifdef MACOSX
    PKVectorControl::resize(w, h);
#endif    
#ifdef WIN32
    PKVectorControl::resize(w, this->bar_height);
#endif    

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->resize(w, h-this->bar_height);
		}
	}
}

void PKArrowTabControl::relayout()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            uint32_t w = child->getW();
            uint32_t h = child->getH();

			child->resize(w, h);
		}
	}

    this->invalidate();
}


void PKArrowTabControl::show()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->show();
		}
	}
}

void PKArrowTabControl::hide()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->hide();
		}
	}
}

void PKArrowTabControl::enable()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->enable();
		}
	}
}

void PKArrowTabControl::disable()
{
	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
			child->disable();
		}
	}
}

uint32_t PKArrowTabControl::getMinimumWidth()
{
    PKVariant *i = this->get(PKArrowTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);

	int icon_w = 0;
	
	if(this->arrow != NULL)
	{
		icon_w = this->arrow->getWidth();
	}

    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    {
        return this->getW();
    }

    uint32_t iconW = (icons.size()*icon_w-1);

    uint32_t maxW = 0;

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            uint32_t childW = child->getMinimumWidth();
            if(childW > maxW) maxW = childW;
		}
	}

    return pk_max(maxW, iconW);
}

uint32_t PKArrowTabControl::getMinimumHeight()
{
    if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
    {
        return this->getH();
    }

    uint32_t maxH = 0;

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

		if(child)
		{
            uint32_t childH = child->getMinimumHeight();
            if(childH > maxH) maxH = childH;
		}
	}

    return maxH + this->bar_height + 3;
}

void PKArrowTabControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(prop->getName() == PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY)
    {
        // Just redraw
        this->invalidate();
    }

#ifdef LINUX
    PKCustomContainer::selfPropertyChanged(prop, oldValue);
#else
    PKVectorControl::selfPropertyChanged(prop, oldValue);
#endif
}

void PKArrowTabControl::mouseDown(PKButtonType button,
					             int x,
					             int y,
					             unsigned int mod)
{
    if(y > this->bar_height)
    {
        return;
    }

	int icon_w = 16;
	
	if(this->arrow != NULL)
	{
		icon_w = this->arrow->getWidth();
	}

    PKVariant *i = this->get(PKArrowTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);

    PKVariant *s = this->get(PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

    uint32_t numItems = icons.size();

    uint32_t index = x / icon_w;

    if(index != sel && index < numItems)
    {
        this->switchToTab(index);
    }
}

void PKArrowTabControl::mouseMove(int x,
					             int y,
					             unsigned int mod)
{
    if(y > this->bar_height)
    {
#ifdef LINUX
        gdk_window_set_cursor(GTK_LAYOUT(this->layout)->bin_window, NULL);    
#endif
    
        PKCursorTools::setCursor(PK_CURSOR_ARROW);
        return;
    }

    PKVariant *i = this->get(PKArrowTabControl::ICONS_WSTRINGLIST_PROPERTY);
    std::vector<std::wstring> icons = PKVALUE_WSTRINGLIST(i);

    PKVariant *s = this->get(PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY);
    uint32_t sel = PKVALUE_UINT32(s);

	int icon_w = 16;
	
	if(this->arrow != NULL)
	{
		icon_w = this->arrow->getWidth();
	}

    uint32_t numItems = icons.size();

    uint32_t index = x / icon_w;

    if(index != sel && index < numItems)
    {
        PKCursorTools::setCursor(PK_CURSOR_HAND);
#ifdef LINUX
        gdk_window_set_cursor(GTK_LAYOUT(this->layout)->bin_window, this->hand);    
#endif
    }
    else
    {
        PKCursorTools::setCursor(PK_CURSOR_ARROW);
#ifdef LINUX
        gdk_window_set_cursor(GTK_LAYOUT(this->layout)->bin_window, NULL);    
#endif
    }
}

