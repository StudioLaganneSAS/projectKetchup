//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKBitmapButton.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKDialog.h"

//f
// PROPERTIES
//

std::string PKBitmapButton::NORMAL_IMAGE_WSTRING_PROPERTY   = "normalImage";
std::string PKBitmapButton::HOVER_IMAGE_WSTRING_PROPERTY    = "hoverImage";
std::string PKBitmapButton::PUSHED_IMAGE_WSTRING_PROPERTY   = "pushedImage";
std::string PKBitmapButton::DISABLED_IMAGE_WSTRING_PROPERTY = "disabledImage";

//
// PKBitmapButton
//

PKBitmapButton::PKBitmapButton()
{
	PKOBJECT_ADD_WSTRING_PROPERTY(normalImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(hoverImage,    L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(pushedImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(disabledImage, L"IMG_NOT_SET");

	this->width     = 0;
	this->height    = 0;

	this->normal_bitmap   = NULL;
	this->hover_bitmap    = NULL;
	this->pushed_bitmap   = NULL;
	this->disabled_bitmap = NULL;

	this->built = false;
    this->hover = false;
    this->down  = false;
}

PKBitmapButton::~PKBitmapButton()
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
}

PKImage *PKBitmapButton::loadBitmap(std::string propertyName)
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


void PKBitmapButton::build()
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

    this->normal_bitmap = this->loadBitmap(PKBitmapButton::NORMAL_IMAGE_WSTRING_PROPERTY);

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    this->premultiplyAlpha(this->normal_bitmap);

    // Load the other images if present

    this->hover_bitmap = this->loadBitmap(PKBitmapButton::HOVER_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->hover_bitmap);

    this->pushed_bitmap = this->loadBitmap(PKBitmapButton::PUSHED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->pushed_bitmap);

    this->disabled_bitmap = this->loadBitmap(PKBitmapButton::DISABLED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->disabled_bitmap);

    // Set our size

    this->width  = this->normal_bitmap->getWidth();
    this->height = this->normal_bitmap->getHeight();

    this->resize(width, height);

    // Then create the control

	PKDoubleBufferedControl::build();

#ifdef LINUX
    
    if(this->widget != NULL && this->normal_bitmap != NULL)
    {
        uint32_t mmw = this->normal_bitmap->getWidth();
        uint32_t mmh = this->normal_bitmap->getHeight();
    
        gtk_widget_set_size_request(this->widget, mmw, mmh);
    }

#endif // LINUX


    this->built = true;
}

void PKBitmapButton::mouseMove(int x,
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

void PKBitmapButton::mouseLeave()
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

void PKBitmapButton::mouseDown(PKButtonType button,
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

void PKBitmapButton::mouseDblClick(PKButtonType button,
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

void PKBitmapButton::mouseUp(PKButtonType button,
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

#ifdef WIN32
                HWND topLevel = GetAncestor(this->hwnd, GA_ROOT);
                PostMessage(topLevel, WM_BITMAP_BUTTON_FIRE, (WPARAM) this->hwnd, 0);
#endif
			
#ifdef LINUX

                if(this->dialog != NULL)
                {
                    PKVariant *i   = this->get(PKControl::ID_STRING_PROPERTY);
                    std::string id = PKVALUE_STRING(i);
                    
                    dialog->buttonClicked(id);
                }
#endif			
				
#ifdef MACOSX

				// Send a message to the parent
				// window so it knows the button
				// was clicked
				
				OSErr	 err;
				EventRef newEvent;
				
				if(CreateEvent(NULL, kEventClassPKButton, kEventPKButtonTrack,
							   0, kEventAttributeNone, &newEvent) != noErr)
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
				
				EventTargetRef target = GetWindowEventTarget(GetWindowFromPort(parentView->getWindowHandle()));
				
				err = SetEventParameter(newEvent, kEventParamPostTarget,
										typeEventTargetRef, sizeof(target), &target);
				
				void *param = (void *) this->getInternalControlId();
				
				err = SetEventParameter(newEvent, kEventParamUserData,
										typeVoidPtr, sizeof(param), &param);
				
				if (err == noErr) 
				{
					err = PostEventToQueue(GetMainEventQueue(), newEvent, kEventPriorityStandard);
					ReleaseEvent(newEvent);
				}
				
#endif
				
            }
        }

        this->down = false;
        this->invalidate();
    }
}


uint32_t PKBitmapButton::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

	return this->width;
}

uint32_t PKBitmapButton::getMinimumHeight()
{
    if(!this->built)
    {
        this->build();
    }

	return this->height;
}

void PKBitmapButton::draw32bpp(uint8_t *buffer)
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


