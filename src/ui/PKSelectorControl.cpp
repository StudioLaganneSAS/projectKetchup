//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKSelectorControl.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKDialog.h"
#include "PKStr.h"
#include "PKBitmapOps.h"
#include "PKDialog.h"

//
// PROPERTIES
//


std::string PKSelectorControl::SELECTION_INT32_PROPERTY        = "selection";
std::string PKSelectorControl::NORMAL_IMAGE_WSTRING_PROPERTY   = "normalImage";
std::string PKSelectorControl::PUSHED_IMAGE_WSTRING_PROPERTY   = "pushedImage";
std::string PKSelectorControl::DISABLED_IMAGE_WSTRING_PROPERTY = "disabledImage";
std::string PKSelectorControl::NUM_ITEMS_UINT32_PROPERTY       = "numItems";
std::string PKSelectorControl::OFFSETS_STRINGLIST_PROPERTY     = "offsets";

//
// PKSelectorControl
//

PKSelectorControl::PKSelectorControl()
{
    std::vector<std::string> list;

    PKOBJECT_ADD_INT32_PROPERTY(selection, -1);
    
    PKOBJECT_ADD_WSTRING_PROPERTY(normalImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(pushedImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(disabledImage, L"IMG_NOT_SET");

    PKOBJECT_ADD_UINT32_PROPERTY(numItems, 0);
    PKOBJECT_ADD_STRINGLIST_PROPERTY(offsets, list);

	this->width  = 0;
	this->height = 0;

	this->normal_bitmap   = NULL;
	this->pushed_bitmap   = NULL;
	this->disabled_bitmap = NULL;

    this->built = false;
}

PKSelectorControl::~PKSelectorControl()
{
	if(this->normal_bitmap   != NULL)
    {
        delete this->normal_bitmap;
    }

	if(this->pushed_bitmap   != NULL)
    {
        delete this->pushed_bitmap;
    }

	if(this->disabled_bitmap != NULL)
    {
        delete this->disabled_bitmap;
    }
}

PKImage *PKSelectorControl::loadBitmap(std::string propertyName)
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


void PKSelectorControl::build()
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

    this->normal_bitmap = this->loadBitmap(PKSelectorControl::NORMAL_IMAGE_WSTRING_PROPERTY);

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    this->premultiplyAlpha(this->normal_bitmap);

    // Load the other images if present

    this->pushed_bitmap = this->loadBitmap(PKSelectorControl::PUSHED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->pushed_bitmap);

    this->disabled_bitmap = this->loadBitmap(PKSelectorControl::DISABLED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->disabled_bitmap);

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

void PKSelectorControl::mouseMove(int x,
			                      int y,
			                      unsigned int mod)
{
    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        return;
    }

    // TODO
}

void PKSelectorControl::mouseLeave()
{
    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        return;
    }

    // TODO
}

void PKSelectorControl::mouseDown(PKButtonType button,
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
		PKVariant *l = this->get(PKSelectorControl::OFFSETS_STRINGLIST_PROPERTY);
		std::vector<std::string> offsets = PKVALUE_STRINGLIST(l);

		// See where we landed
		this->downButton = -1;

		uint32_t i;

		for(i=0; i<offsets.size(); i++)
		{
			if(x < PKStr::stringToInt32(offsets[i]))
			{
				break;
			}
		}

		this->downButton = i-1;
    }
}

void PKSelectorControl::mouseUp(PKButtonType button,
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
		PKVariant *l = this->get(PKSelectorControl::OFFSETS_STRINGLIST_PROPERTY);
		std::vector<std::string> offsets = PKVALUE_STRINGLIST(l);

		// See where we landed
		int32_t upButton = -1;
		uint32_t i;

		for(i=0; i<offsets.size(); i++)
		{
			if(x < PKStr::stringToInt32(offsets[i]))
			{
				break;
			}
		}

		upButton = i-1;

		if(upButton == this->downButton)
		{
			int32_t oldValue = PKVALUE_INT32(this->get(PKSelectorControl::SELECTION_INT32_PROPERTY));

			PKVariantInt32 sel(upButton);
			this->set(PKSelectorControl::SELECTION_INT32_PROPERTY, &sel);

            if(this->getDialog())
            {
                std::string id = PKVALUE_STRING(this->get(PKControl::ID_STRING_PROPERTY));
                this->getDialog()->selectorControlChanged(id, oldValue);
            }
		}
    }
}


uint32_t PKSelectorControl::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

	return this->width;
}

uint32_t PKSelectorControl::getMinimumHeight()
{
    if(!this->built)
    {
        this->build();
    }

	return this->height;
}

void PKSelectorControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKSelectorControl::SELECTION_INT32_PROPERTY)
	{
		this->invalidate();
	}

    if(prop->getName() == PKControl::ENABLED_BOOL_PROPERTY)
	{
		this->invalidate();
	}

    PKDoubleBufferedControl::selfPropertyChanged(prop, oldValue);
}

void PKSelectorControl::draw32bpp(uint8_t *buffer)
{
    if(this->normal_bitmap == NULL)
    {
        return;
    }

    PKVariant *n = this->get(PKSelectorControl::NUM_ITEMS_UINT32_PROPERTY);
    uint32_t numItems = PKVALUE_UINT32(n);

    PKVariant *s = this->get(PKSelectorControl::SELECTION_INT32_PROPERTY);
    int32_t sel  = PKVALUE_INT32(s);

    PKVariant *l = this->get(PKSelectorControl::OFFSETS_STRINGLIST_PROPERTY);
    std::vector<std::string> offsets = PKVALUE_STRINGLIST(l);

    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        if(this->disabled_bitmap)
        {
            this->disabled_bitmap->copy(0, 0, (char*) buffer, 
                                        0, 0, 
                                        this->width, 
                                        this->height);
            return;
        }
    }


    this->normal_bitmap->copy(0, 0, (char*) buffer, 
                              0, 0, 
                              this->width, 
                              this->height);

    if(sel >= 0 && sel < (int32_t) numItems && this->pushed_bitmap)
    {
        if(sel+1 <= (int32_t)offsets.size()-1)
        {
            int x1 = PKStr::stringToInt32(offsets[sel]);
            int x2 = PKStr::stringToInt32(offsets[sel+1]);

            PKBitmapOps::blit(buffer, this->width, this->height, x1, 0, 
                              this->pushed_bitmap, x1, 0, x2-x1, this->height);
        }
    }
}


