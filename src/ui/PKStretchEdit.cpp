//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKStretchEdit.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKBitmapOps.h"

//
// PROPERTIES
//

std::string PKStretchEdit::NORMAL_IMAGE_WSTRING_PROPERTY   = "normalImage";
std::string PKStretchEdit::DISABLED_IMAGE_WSTRING_PROPERTY = "disabledImage";
std::string PKStretchEdit::STRETCH_OFFSET_UINT32_PROPERTY  = "stretchOffset";
std::string PKStretchEdit::TEXT_WSTRING_PROPERTY           = "text";
std::string PKStretchEdit::OFFSET_TOP_UINT32_PROPERTY      = "offsetTop";
std::string PKStretchEdit::OFFSET_LEFT_UINT32_PROPERTY     = "offsetLeft";
std::string PKStretchEdit::OFFSET_RIGHT_UINT32_PROPERTY    = "offsetRight";
std::string PKStretchEdit::OFFSET_BOTTOM_UINT32_PROPERTY   = "offsetBottom";

//
// PKStretchEdit
//

PKStretchEdit::PKStretchEdit()
{
	this->built = false;

    PKOBJECT_ADD_WSTRING_PROPERTY(normalImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(disabledImage, L"IMG_NOT_SET");
    PKOBJECT_ADD_UINT32_PROPERTY(stretchOffset, 0);
    PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
    PKOBJECT_ADD_UINT32_PROPERTY(offsetTop, 0);
    PKOBJECT_ADD_UINT32_PROPERTY(offsetLeft, 0);
    PKOBJECT_ADD_UINT32_PROPERTY(offsetRight, 0);
    PKOBJECT_ADD_UINT32_PROPERTY(offsetBottom, 0);

	this->width     = 0;
	this->height    = 0;

	this->normal_bitmap   = NULL;
	this->disabled_bitmap = NULL;

#ifdef WIN32
    this->edit = NULL;
#endif
	
#ifdef MACOSX
	this->textView = NULL;
#endif
}

PKStretchEdit::~PKStretchEdit()
{
	if(this->normal_bitmap   != NULL)
    {
        delete this->normal_bitmap;
    }

	if(this->disabled_bitmap != NULL)
    {
        delete this->disabled_bitmap;
    }
}

PKImage *PKStretchEdit::loadBitmap(std::string propertyName)
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


void PKStretchEdit::build()
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

    this->normal_bitmap = this->loadBitmap(PKStretchEdit::NORMAL_IMAGE_WSTRING_PROPERTY);

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    this->premultiplyAlpha(this->normal_bitmap);

    // Load the other images if present

    this->disabled_bitmap = this->loadBitmap(PKStretchEdit::DISABLED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->disabled_bitmap);

    // Compute our size

    this->built = true;

    uint32_t minWidth  = this->getMinimumWidth();
    uint32_t minHeight = this->getMinimumHeight();

    // Set our size

    this->width  = minWidth;
    this->height = minHeight;

    // Then create the control

	PKDoubleBufferedControl::build();
    
    // Now create the edit control

#ifdef WIN32
    this->edit = new PKEdit();
    this->addChild(this->edit);

    PKVariantBool border(false);
    this->edit->set(PKEdit::BORDER_BOOL_PROPERTY, &border);
    PKVariant *t = this->get(PKStretchEdit::TEXT_WSTRING_PROPERTY);
    this->edit->set(PKEdit::TEXT_WSTRING_PROPERTY, t);

    this->edit->build();
    this->resize(width, height);
#endif
	
#ifdef MACOSX
	
	OSStatus err = HITextViewCreate(NULL, 0, 
									kTXNDontDrawCaretWhenInactiveMask | kTXNSingleLineOnlyMask,
									&this->textView);
	
	if(err == noErr && this->textView)
	{
		HIViewRef mContent;
		
		HIViewFindByID(HIViewGetRoot(GetWindowFromPort(parentView->getWindowHandle())),
					   kHIViewWindowContentID, &mContent);
		
		HIViewAddSubview(mContent, this->textView);
		HIViewSetVisible(this->textView, true);
	}
	
#endif
		
	// TODO: Implement right area bitmap for clearing the text
	// and also shadow text for when textfield is empty
}

void PKStretchEdit::initialize(PKDialog *dialog)
{
    PKDoubleBufferedControl::initialize(dialog);

#ifdef WIN32
    if(this->edit)
    {
        this->edit->initialize(dialog);
    }
#endif
}


// From PKControl

void PKStretchEdit::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(!this->built)
    {
        return;
    }

    if(prop->getName() == PKStretchEdit::TEXT_WSTRING_PROPERTY)
    {
#ifdef WIN32
        if(this->edit)
        {
            this->edit->set(PKEdit::TEXT_WSTRING_PROPERTY, prop->getValue());
        }
#endif
		
#ifdef MACOSX
		// TODO
#endif
    }

    PKDoubleBufferedControl::selfPropertyChanged(prop, oldValue);
}

void PKStretchEdit::move(int32_t x, 
		                 int32_t y)
{
    PKDoubleBufferedControl::move(x, y);

	PKVariant *offx = this->get(PKStretchEdit::OFFSET_LEFT_UINT32_PROPERTY);
	uint32_t   xx   = PKVALUE_UINT32(offx);
	
	PKVariant *offy = this->get(PKStretchEdit::OFFSET_TOP_UINT32_PROPERTY);
	uint32_t   yy   = PKVALUE_UINT32(offy);

#ifdef WIN32
    if(this->edit)
    {
        this->edit->move(xx, yy); 
	}
#endif

#ifdef MACOSX
	if(this->textView)
	{
		MoveControl(this->textView, x + xx, y + yy);		
	}
#endif
}

void PKStretchEdit::show()
{
    PKDoubleBufferedControl::show();

#ifdef WIN32
    if(this->edit)
    {
        this->edit->show();
    }
#endif
	
#ifdef MACOSX
	if(this->textView)
	{
		ShowControl(this->textView);
	}
#endif
}

void PKStretchEdit::hide()
{
    PKDoubleBufferedControl::hide();

#ifdef WIN32
    if(this->edit)
    {
        this->edit->hide();
    }
#endif

#ifdef MACOSX
	if(this->textView)
	{
		HideControl(this->textView);
	}
#endif
}

void PKStretchEdit::enable()
{
    PKDoubleBufferedControl::enable();

#ifdef WIN32
    if(this->edit)
    {
        this->edit->enable();
    }
#endif

#ifdef MACOSX
	if(this->textView)
	{
		EnableControl(this->textView);
	}
#endif
}

void PKStretchEdit::disable()
{
    PKDoubleBufferedControl::disable();

#ifdef WIN32
    if(this->edit)
    {
        this->edit->disable();
    }
#endif

#ifdef MACOSX
	if(this->textView)
	{
		DisableControl(this->textView);
	}
#endif
}

uint32_t PKStretchEdit::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

    if(this->getLayoutPolicyX() != PK_LAYOUT_FIXED)
    {
        if(this->normal_bitmap)
        {
	        PKVariant *t = this->get(PKStretchEdit::TEXT_WSTRING_PROPERTY);
	        std::wstring text = PKVALUE_WSTRING(t);

            return (this->normal_bitmap->getWidth() + 100);
        }
    }
    else
    {
        return this->getW();
    }

	return 0;
}

uint32_t PKStretchEdit::getMinimumHeight()
{
    if(!this->built)
    {
        this->build();
    }

    if(this->normal_bitmap)
    {
        return (this->normal_bitmap->getHeight());
    }

    return 0;
}

void PKStretchEdit::resize(uint32_t w, 
			               uint32_t h)
{
    PKDoubleBufferedControl::resize(w, h);

    this->width  = w;
    this->height = h;

	PKVariant *offx = this->get(PKStretchEdit::OFFSET_LEFT_UINT32_PROPERTY);
	uint32_t   xx   = PKVALUE_UINT32(offx);
	
	PKVariant *offy = this->get(PKStretchEdit::OFFSET_TOP_UINT32_PROPERTY);
	uint32_t   yy   = PKVALUE_UINT32(offy);
    
	PKVariant *diffx = this->get(PKStretchEdit::OFFSET_RIGHT_UINT32_PROPERTY);
	uint32_t   dx    = PKVALUE_UINT32(diffx);
	
	PKVariant *diffy = this->get(PKStretchEdit::OFFSET_BOTTOM_UINT32_PROPERTY);
	uint32_t   dy    = PKVALUE_UINT32(diffy);
	
#ifdef WIN32
    if(this->edit)
    {
        this->edit->resize(w-xx-dx, h-yy-dy); 
    }
#endif

#ifdef MACOSX
	if(this->textView)
	{
		SizeControl(this->textView, w-xx-dx, h-yy-dy);		
	}
#endif
}

void PKStretchEdit::draw32bpp(uint8_t *buffer)
{
	// Draw current frame

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    PKVariant *o    = this->get(PKStretchEdit::STRETCH_OFFSET_UINT32_PROPERTY);
    uint32_t offset = PKVALUE_UINT32(o);

    std::vector<unsigned int> offsets;
    offsets.push_back(offset);

    PKVariant *e = this->get(PKControl::ENABLED_BOOL_PROPERTY);
    bool enabled = PKVALUE_BOOL(e);

    if(!enabled)
    {
        if(this->disabled_bitmap)
        {
            PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                          this->disabled_bitmap, offsets);
            return;
        }
    }

    // Draw background

    PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                  this->normal_bitmap, offsets);
}


