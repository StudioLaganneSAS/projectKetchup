//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKBitmapProgressBar.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKBitmapOps.h"

//
// PROPERTIES
//

std::string PKBitmapProgressBar::EMPTY_IMAGE_WSTRING_PROPERTY     = "emptyImage";
std::string PKBitmapProgressBar::FILLED_IMAGE_WSTRING_PROPERTY    = "filledImage";
std::string PKBitmapProgressBar::DISABLED_IMAGE_WSTRING_PROPERTY  = "disabledImage";
std::string PKBitmapProgressBar::VALUE_INT32_PROPERTY             = "value";
std::string PKBitmapProgressBar::RANGE_INT32RANGE_PROPERTY        = "range";

//
// PKBitmapProgressBar
//

PKBitmapProgressBar::PKBitmapProgressBar()
{
    PKInt32Range r = PKInt32RangeCreate(0, 100);

    PKOBJECT_ADD_INT32_PROPERTY(value, 0);
    PKOBJECT_ADD_INT32RANGE_PROPERTY(range, r);

	PKOBJECT_ADD_WSTRING_PROPERTY(emptyImage,    L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(filledImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(disabledImage, L"IMG_NOT_SET");

	this->height    = 0;

	this->empty_bitmap    = NULL;
	this->filled_bitmap   = NULL;
	this->disabled_bitmap = NULL;

	this->built = false;
}

PKBitmapProgressBar::~PKBitmapProgressBar()
{
	if(this->empty_bitmap   != NULL)
    {
        delete this->empty_bitmap;
    }

	if(this->filled_bitmap  != NULL)
    {
        delete this->filled_bitmap;
    }

	if(this->disabled_bitmap != NULL)
    {
        delete this->disabled_bitmap;
    }
}

PKImage *PKBitmapProgressBar::loadBitmap(std::string propertyName)
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


void PKBitmapProgressBar::build()
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
	
	if(this->empty_bitmap != NULL)
	{
		delete this->empty_bitmap;
		this->empty_bitmap = NULL;
	}

    this->empty_bitmap = this->loadBitmap(PKBitmapProgressBar::EMPTY_IMAGE_WSTRING_PROPERTY);

    if(this->empty_bitmap == NULL)
    {
        return;
    }

    this->premultiplyAlpha(this->empty_bitmap);

    // Load the other images if present

    this->filled_bitmap = this->loadBitmap(PKBitmapProgressBar::FILLED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->filled_bitmap);

    if(this->filled_bitmap == NULL)
    {
        return;
    }

	this->disabled_bitmap = this->loadBitmap(PKBitmapProgressBar::DISABLED_IMAGE_WSTRING_PROPERTY);
	this->premultiplyAlpha(this->disabled_bitmap);

	// Set our size

    this->height = this->empty_bitmap->getHeight();
    this->resize(100, height);

    // Then create the control

	PKDoubleBufferedControl::build();

#ifdef LINUX
    
    if(this->widget != NULL && this->empty_bitmap != NULL)
    {
        int32_t mmw = -1;
        int32_t mmh = this->empty_bitmap->getHeight();
    
        gtk_widget_set_size_request(this->widget, mmw, mmh);
    }

#endif // LINUX

    this->built = true;
}

uint32_t PKBitmapProgressBar::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

	return 0;
}

uint32_t PKBitmapProgressBar::getMinimumHeight()
{
    if(!this->built)
    {
        this->build();
    }

	return this->height;
}

// From PKObject

void PKBitmapProgressBar::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKBitmapProgressBar::VALUE_INT32_PROPERTY)
	{
		this->invalidate();
	}

	if(prop->getName() == PKBitmapProgressBar::RANGE_INT32RANGE_PROPERTY)
	{
		this->invalidate();
	}

	PKDoubleBufferedControl::selfPropertyChanged(prop, oldValue);
}

void PKBitmapProgressBar::draw32bpp(uint8_t *buffer)
{
	// Draw current frame

    if(this->empty_bitmap == NULL)
    {
        return;
    }

    if(this->filled_bitmap == NULL)
    {
        return;
    }

	PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
		if(this->disabled_bitmap)
		{
			std::vector<unsigned int> offsets;
			offsets.push_back(this->disabled_bitmap->getWidth()/2);

			PKBitmapOps::fillStretchWidth(buffer, this->getW(), this->getH(), this->disabled_bitmap, offsets);

			return;
		}
    }

	// Fill Stretch Width

    PKVariant *v  = this->get(PKBitmapProgressBar::VALUE_INT32_PROPERTY);
    int32_t value = PKVALUE_INT32(v);

    PKVariant *r = this->get(PKBitmapProgressBar::RANGE_INT32RANGE_PROPERTY);
    PKInt32Range range = PKVALUE_INT32RANGE(r);

	if(range.max == range.min)
	{
		return;
	}

	int w = this->getW();
	int h = this->getH();

	if(value <= range.min)
	{
		std::vector<unsigned int> offsets;
		offsets.push_back(this->empty_bitmap->getWidth()/2);

		PKBitmapOps::fillStretchWidth(buffer, this->getW(), this->getH(), this->empty_bitmap, offsets);
	}
	else if(value >= range.max)
	{
		std::vector<unsigned int> offsets;
		offsets.push_back(this->filled_bitmap->getWidth()/2);

		PKBitmapOps::fillStretchWidth(buffer, this->getW(), this->getH(), this->filled_bitmap, offsets);
	}
	else
	{
		int split = (value - range.min) * this->getW() / (range.max - range.min);
		// Must mix the two
		PKBitmapOps::fillWidthWithStart(buffer, split, this->getW(), this->getH(), this->filled_bitmap, this->filled_bitmap->getWidth()/2);
		PKBitmapOps::fillWidthWithEnd(buffer, split, this->getW() - split, this->getW(), this->getH(), this->empty_bitmap, this->empty_bitmap->getWidth()/2);
	}
}


