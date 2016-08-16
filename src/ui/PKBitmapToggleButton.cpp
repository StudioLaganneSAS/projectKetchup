//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKBitmapToggleButton.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKDialog.h"

//
// PROPERTIES
//

std::string PKBitmapToggleButton::TOGGLED_BOOL_PROPERTY           = "toggled";

std::string PKBitmapToggleButton::NORMAL_IMAGE_WSTRING_PROPERTY   = "normalImage";
std::string PKBitmapToggleButton::HOVER_IMAGE_WSTRING_PROPERTY    = "hoverImage";
std::string PKBitmapToggleButton::PUSHED_IMAGE_WSTRING_PROPERTY   = "pushedImage";
std::string PKBitmapToggleButton::DISABLED_IMAGE_WSTRING_PROPERTY = "disabledImage";

std::string PKBitmapToggleButton::TOGGLED_NORMAL_IMAGE_WSTRING_PROPERTY   = "toggledNormalImage";
std::string PKBitmapToggleButton::TOGGLED_HOVER_IMAGE_WSTRING_PROPERTY    = "toggledHoverImage";
std::string PKBitmapToggleButton::TOGGLED_PUSHED_IMAGE_WSTRING_PROPERTY   = "toggledPushedImage";

std::string PKBitmapToggleButton::PREVENT_TOGGLE_OFF_BOOL_PROPERTY = "preventToggleOff";


//
// PKBitmapButton
//

PKBitmapToggleButton::PKBitmapToggleButton()
{
    PKOBJECT_ADD_BOOL_PROPERTY(toggled, false);
    
    PKOBJECT_ADD_WSTRING_PROPERTY(normalImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(hoverImage,    L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(pushedImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(disabledImage, L"IMG_NOT_SET");

    PKOBJECT_ADD_WSTRING_PROPERTY(toggledNormalImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(toggledHoverImage,    L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(toggledPushedImage,   L"IMG_NOT_SET");

	PKOBJECT_ADD_BOOL_PROPERTY(preventToggleOff, false);

	this->width  = 0;
	this->height = 0;

	this->normal_bitmap   = NULL;
	this->hover_bitmap    = NULL;
	this->pushed_bitmap   = NULL;
	this->disabled_bitmap = NULL;

	this->toggled_normal_bitmap   = NULL;
	this->toggled_hover_bitmap    = NULL;
	this->toggled_pushed_bitmap   = NULL;

    this->built = false;
    this->hover = false;
    this->down  = false;
}

PKBitmapToggleButton::~PKBitmapToggleButton()
{
	if(this->normal_bitmap   != NULL)
    {
        delete this->normal_bitmap;
    }

	if(this->hover_bitmap    != NULL)
    {
        delete this->hover_bitmap;
    }

	if(this->pushed_bitmap   != NULL)
    {
        delete this->pushed_bitmap;
    }

	if(this->disabled_bitmap != NULL)
    {
        delete this->disabled_bitmap;
    }

	if(this->toggled_normal_bitmap   != NULL)
    {
        delete this->toggled_normal_bitmap;
    }

	if(this->toggled_hover_bitmap    != NULL)
    {
        delete this->toggled_hover_bitmap;
    }

	if(this->toggled_pushed_bitmap   != NULL)
    {
        delete this->toggled_pushed_bitmap;
    }
}

PKImage *PKBitmapToggleButton::loadBitmap(std::string propertyName)
{
    PKImage *bitmap = NULL;

    PKVariant *img = this->get(propertyName);
	std::wstring URI = PKVALUE_WSTRING(img);
	
	PKImageURIType type;
	std::wstring   image;
	
	PKImage::processURI(URI, &type, &image);

	if(type == PKIMAGE_URI_RESOURCE)
	{
    	bitmap = PKImageLoader::loadBitmapFromResource(image);
	}

	if(type == PKIMAGE_URI_FILENAME)
	{
    	bitmap = PKImageLoader::loadBitmapFromFilename(image);
	}

    return bitmap;
}


void PKBitmapToggleButton::build()
{
    if(this->built)
    {
        return;
    }

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

    // Load the normal image
	
	if(this->normal_bitmap != NULL)
	{
		delete this->normal_bitmap;
		this->normal_bitmap = NULL;
	}

    this->normal_bitmap = this->loadBitmap(PKBitmapToggleButton::NORMAL_IMAGE_WSTRING_PROPERTY);

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    this->premultiplyAlpha(this->normal_bitmap);

    // Load the other images if present

    this->hover_bitmap = this->loadBitmap(PKBitmapToggleButton::HOVER_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->hover_bitmap);

    this->pushed_bitmap = this->loadBitmap(PKBitmapToggleButton::PUSHED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->pushed_bitmap);

    this->disabled_bitmap = this->loadBitmap(PKBitmapToggleButton::DISABLED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->disabled_bitmap);

    this->toggled_normal_bitmap = this->loadBitmap(PKBitmapToggleButton::TOGGLED_NORMAL_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->toggled_normal_bitmap);

    this->toggled_hover_bitmap = this->loadBitmap(PKBitmapToggleButton::TOGGLED_HOVER_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->toggled_hover_bitmap);

    this->toggled_pushed_bitmap = this->loadBitmap(PKBitmapToggleButton::TOGGLED_PUSHED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->toggled_pushed_bitmap);


    // Set our size

    this->width  = this->normal_bitmap->getWidth();
    this->height = this->normal_bitmap->getHeight();

    this->resize(width, height);

    // Then create the control

	PKDoubleBufferedControl::build();
	
#ifdef LINUX

    if(this->widget != NULL)
    {
        gtk_widget_set_size_request(this->widget, 
                                    this->width, 
                                    this->height);
    }
    	
#endif

    this->built = true;
}

// From PKObject
void PKBitmapToggleButton::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKBitmapToggleButton::TOGGLED_BOOL_PROPERTY)
	{
		this->invalidate();
	}
	
	PKDoubleBufferedControl::selfPropertyChanged(prop, oldValue);
}

void PKBitmapToggleButton::mouseMove(int x,
			                   int y,
			                   unsigned int mod)
{
    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        return;
    }

    this->hover = true;
    this->invalidate();
}

void PKBitmapToggleButton::mouseLeave()
{
    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        return;
    }

    this->hover = false;
    this->invalidate();
}

void PKBitmapToggleButton::mouseDown(PKButtonType button,
					           int x,
					           int y,
					           unsigned int mod)
{
    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        return;
    }

    if(button == PK_BUTTON_LEFT)
    {
        this->down = true;
        this->invalidate();
    }
}

void PKBitmapToggleButton::mouseDblClick(PKButtonType button,
				                   int x,
				                   int y,
				                   unsigned int mod)
{
    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        return;
    }

    if(button == PK_BUTTON_LEFT)
    {
        this->down = true;
        this->invalidate();
    }
}

void PKBitmapToggleButton::mouseUp(PKButtonType button,
					                         int x,
					                         int y,
					                         unsigned int mod)
{
    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        return;
    }

    if(button == PK_BUTTON_LEFT)
    {
        if(this->down)
        {
            if(x >= 0 && y >= 0 &&
               x <= (int) this->width &&
               y <= (int) this->height)
            {
                // It's a HIT !

                PKVariant *t = this->get(PKBitmapToggleButton::TOGGLED_BOOL_PROPERTY);
                bool toggled = PKVALUE_BOOL(t);

				bool preventOff = PKVALUE_BOOL(this->get(PKBitmapToggleButton::PREVENT_TOGGLE_OFF_BOOL_PROPERTY));

				if(toggled && preventOff)
				{
					// Do nothing
				}
				else // toggle
				{
					toggled = !toggled;

					PKVariantBool newVal(toggled);
					this->set(PKBitmapToggleButton::TOGGLED_BOOL_PROPERTY, &newVal);

					if(this->dialog != NULL)
					{
						PKVariant *i   = this->get(PKControl::ID_STRING_PROPERTY);
						std::string id = PKVALUE_STRING(i);
			            
						dialog->bitmapToggleButtonToggled(id, this, !toggled);
					}	
				}
            }
        }

        this->down = false;
        this->invalidate();
    }
}


uint32_t PKBitmapToggleButton::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

	return this->width;
}

uint32_t PKBitmapToggleButton::getMinimumHeight()
{
    if(!this->built)
    {
        this->build();
    }

	return this->height;
}

void PKBitmapToggleButton::draw32bpp(uint8_t *buffer)
{
	// Draw current frame

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        if(this->disabled_bitmap)
        {
            this->disabled_bitmap->copy(0, 0, (char*) buffer, 
                                        0, 0, this->width, this->height);
            return;
        }
    }

    PKVariant *t = this->get(PKBitmapToggleButton::TOGGLED_BOOL_PROPERTY);
    bool toggled = PKVALUE_BOOL(t);

    if(toggled)
    {
        if(this->down && this->toggled_pushed_bitmap)
        {
            this->toggled_pushed_bitmap->copy(0, 0, (char*) buffer, 
                                              0, 0, this->width, this->height);
        }
        else
        {
            if(this->hover && this->toggled_hover_bitmap)
            {
                this->toggled_hover_bitmap->copy(0, 0, (char*) buffer, 
                                                 0, 0, this->width, this->height);
            }
            else
            {
                this->toggled_normal_bitmap->copy(0, 0, (char*) buffer, 
                                                  0, 0, this->width, this->height);
            }
        }
    }
    else
    {
        if(this->down && this->pushed_bitmap)
        {
            this->pushed_bitmap->copy(0, 0, (char*) buffer, 
                                      0, 0, this->width, this->height);
        }
        else
        {
            if(this->hover && this->hover_bitmap)
            {
                this->hover_bitmap->copy(0, 0, (char*) buffer, 
                                          0, 0, this->width, this->height);
            }
            else
            {
                this->normal_bitmap->copy(0, 0, (char*) buffer, 
                                          0, 0, this->width, this->height);
            }
        }
    }
}


