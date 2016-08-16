//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKStretchButton.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKFont.h"
#include "PKBitmapOps.h"
#include "PKStr.h"
#include "PKDialog.h"

//
// PROPERTIES
//

std::string PKStretchButton::NORMAL_IMAGE_WSTRING_PROPERTY    = "normalImage";
std::string PKStretchButton::HOVER_IMAGE_WSTRING_PROPERTY     = "hoverImage";
std::string PKStretchButton::PUSHED_IMAGE_WSTRING_PROPERTY    = "pushedImage";
std::string PKStretchButton::DISABLED_IMAGE_WSTRING_PROPERTY  = "disabledImage";
std::string PKStretchButton::STRETCH_OFFSET_UINT32_PROPERTY   = "stretchOffset";
std::string PKStretchButton::TEXT_WSTRING_PROPERTY            = "text";
std::string PKStretchButton::FONTNAME_STRING_PROPERTY         = "fontName";
std::string PKStretchButton::FONTSIZE_UINT32_PROPERTY         = "fontSize";
std::string PKStretchButton::RELATIVEFONTSIZE_STRING_PROPERTY = "relativeFontSize";
std::string PKStretchButton::COLOR_COLOR_PROPERTY             = "color";
std::string PKStretchButton::SHADOW_COLOR_COLOR_PROPERTY      = "shadowColor";
std::string PKStretchButton::DISABLED_COLOR_COLOR_PROPERTY    = "disabledColor";
std::string PKStretchButton::BOLD_BOOL_PROPERTY               = "bold";
std::string PKStretchButton::ITALICS_BOOL_PROPERTY            = "italics";
std::string PKStretchButton::SHADOW_OFFSET_POINT_PROPERTY     = "shadowOffset";

//
// PKStretchButton
//

PKStretchButton::PKStretchButton()
{
	this->built = false;
    this->hover = false;
    this->down  = false;

	PKColor c = PKColorCreate(0, 0, 0, 255);
    PKColor d = PKColorCreate(128, 128, 128, 255);
    PKColor s = PKColorCreate(0, 0, 0, 0);
    PKPoint p = PKPointCreate(0, 0);

    PKOBJECT_ADD_WSTRING_PROPERTY(normalImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(hoverImage,    L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(pushedImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(disabledImage, L"IMG_NOT_SET");
    PKOBJECT_ADD_UINT32_PROPERTY(stretchOffset, 0);
    PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
    PKOBJECT_ADD_STRING_PROPERTY(fontName, "");
    PKOBJECT_ADD_UINT32_PROPERTY(fontSize, 0);
    PKOBJECT_ADD_COLOR_PROPERTY(color, c);
    PKOBJECT_ADD_COLOR_PROPERTY(disabledColor, d);
    PKOBJECT_ADD_COLOR_PROPERTY(shadowColor, s);
    PKOBJECT_ADD_BOOL_PROPERTY(bold, false);
    PKOBJECT_ADD_BOOL_PROPERTY(italics, false);
    PKOBJECT_ADD_POINT_PROPERTY(shadowOffset, p);
    PKOBJECT_ADD_STRING_PROPERTY(relativeFontSize, "NOT_SET");

	this->width     = 0;
	this->height    = 0;

	this->normal_bitmap   = NULL;
	this->hover_bitmap    = NULL;
	this->pushed_bitmap   = NULL;
	this->disabled_bitmap = NULL;


    this->font = NULL;

    this->shadowFont   = NULL;
    this->disabledFont = NULL;
}

PKStretchButton::~PKStretchButton()
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

    if(this->font != NULL)
    {
        delete this->font;
    }

    if(this->shadowFont != NULL)
    {
        delete this->shadowFont;
    }

    if(this->disabledFont != NULL)
    {
        delete this->disabledFont;
    }
}

PKImage *PKStretchButton::loadBitmap(std::string propertyName)
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


void PKStretchButton::build()
{
    if(this->built)
    {
        return;
    }

    PKVariant *n = this->get(PKStretchButton::FONTNAME_STRING_PROPERTY);
    std::string fontName = PKVALUE_STRING(n);

    PKVariant *s = this->get(PKStretchButton::FONTSIZE_UINT32_PROPERTY);
    uint32_t fontSize = PKVALUE_UINT32(s);

    PKVariant *rs = this->get(PKStretchButton::RELATIVEFONTSIZE_STRING_PROPERTY);
    std::string relativeSize = PKVALUE_STRING(rs);

    PKVariant *b = this->get(PKStretchButton::BOLD_BOOL_PROPERTY);
    bool bold = PKVALUE_BOOL(b);

	PKVariant *i = this->get(PKStretchButton::ITALICS_BOOL_PROPERTY);
    bool italics = PKVALUE_BOOL(i);

    PKVariant *c = this->get(PKStretchButton::COLOR_COLOR_PROPERTY);
    PKColor fontColor = PKVALUE_COLOR(c);

    PKVariant *d = this->get(PKStretchButton::DISABLED_COLOR_COLOR_PROPERTY);
    PKColor disabledColor = PKVALUE_COLOR(d);

    PKVariant *sh = this->get(PKStretchButton::SHADOW_COLOR_COLOR_PROPERTY);
    PKColor shadowColor = PKVALUE_COLOR(sh);

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

    this->normal_bitmap = this->loadBitmap(PKStretchButton::NORMAL_IMAGE_WSTRING_PROPERTY);

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    this->premultiplyAlpha(this->normal_bitmap);

    // Load the other images if present

    this->hover_bitmap = this->loadBitmap(PKStretchButton::HOVER_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->hover_bitmap);

    this->pushed_bitmap = this->loadBitmap(PKStretchButton::PUSHED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->pushed_bitmap);

    this->disabled_bitmap = this->loadBitmap(PKStretchButton::DISABLED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->disabled_bitmap);

    // Load the font and text

    if(fontName == "" && 
       fontSize == 0  && 
       relativeSize == "NOT_SET" &&
      !bold && !italics)
    {
        // Default font
        this->font = new PKFont();
        this->disabledFont = new PKFont();

        if(shadowColor.a != 0)
        {
            this->shadowFont = new PKFont();
        }
    }
    else
    {
        if(relativeSize != "NOT_SET")
        {
            int32_t diff = PKStr::stringToInt32(relativeSize);
            fontSize     = PKFont::getSystemFontSize() + diff;
        }

        this->font = new PKFont(fontName, fontSize, bold, italics);
        this->disabledFont = new PKFont(fontName, fontSize, bold, italics);

        if(shadowColor.a != 0)
        {
            this->shadowFont = new PKFont(fontName, fontSize, bold, italics);
        }
    }    

    if(this->font == NULL)
    {
        return;
    }

    this->font->setColor(fontColor.r, fontColor.g, fontColor.b);

    if(this->disabledFont != NULL)
    {
        this->disabledFont->setColor(disabledColor.r, disabledColor.g, disabledColor.b);
    }

    if(this->shadowFont != NULL)
    {
        this->shadowFont->setColor(shadowColor.r, shadowColor.g, shadowColor.b);
    }

    // Compute our size

    this->built = true;

    uint32_t minWidth  = this->getMinimumWidth();
    uint32_t minHeight = this->getMinimumHeight();

    // Set our size

    this->width  = minWidth;
    this->height = minHeight;

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

#endif // LINUX
	
}

// From PKControl

void PKStretchButton::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(!this->built)
    {
		return;
	}

	if(prop->getName() == PKStretchButton::TEXT_WSTRING_PROPERTY)
	{
		if(this->getLayoutPolicyX() != PK_LAYOUT_FIXED)
		{
			// Calculate our new size
			int w = this->getMinimumWidth();
			int h = this->getMinimumHeight();
			
			this->resize(w, h);
		}
	}
	
    this->invalidate();

    PKDoubleBufferedControl::selfPropertyChanged(prop, oldValue);
}

// From PKCustomControl

void PKStretchButton::mouseMove(int x,
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

void PKStretchButton::mouseLeave()
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

void PKStretchButton::mouseDown(PKButtonType button,
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

void PKStretchButton::mouseDblClick(PKButtonType button,
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

void PKStretchButton::mouseUp(PKButtonType button,
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
                PostMessage(topLevel, WM_STRETCH_BITMAP_BUTTON_FIRE, (WPARAM) this->hwnd, 0);
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
				
				if(CreateEvent(NULL, kEventClassPKStretchButton, kEventPKStretchButtonTrack,
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


uint32_t PKStretchButton::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

    if(this->getLayoutPolicyX() != PK_LAYOUT_FIXED)
    {
        if(this->normal_bitmap && this->font)
        {
	        PKVariant *t = this->get(PKStretchButton::TEXT_WSTRING_PROPERTY);
	        std::wstring text = PKVALUE_WSTRING(t);

            return (this->normal_bitmap->getWidth() + this->font->getTextWidth(text));
        }
    }
    else
    {
        return this->getW();
    }

	return 0;
}

uint32_t PKStretchButton::getMinimumHeight()
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

void PKStretchButton::resize(uint32_t w, 
			                 uint32_t h)
{
    PKDoubleBufferedControl::resize(w, h);
    this->width  = w;
    this->height = h;
}

void PKStretchButton::draw32bpp(uint8_t *buffer)
{
	// Draw current frame

    if(this->normal_bitmap == NULL || this->font == NULL)
    {
        return;
    }

    PKVariant *t = this->get(PKStretchButton::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

    PKVariant *o    = this->get(PKStretchButton::STRETCH_OFFSET_UINT32_PROPERTY);
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

            PKFont *font = this->disabledFont;

            if(font == NULL)
            {
                font = this->font;
            }

            int h = this->font->getCharHeight();
            int x = offset;
            int y = (this->height - h)/2 - h/4;

            font->drawText(text, buffer, x, y, this->width, this->height);

            return;
        }
    }

    // Draw background

    if(this->down && this->pushed_bitmap)
    {
        PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                      this->pushed_bitmap, offsets);
    }
    else
    {
        if(this->hover && this->hover_bitmap)
        {
            PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                          this->hover_bitmap, offsets);
        }
        else
        {
            PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                          this->normal_bitmap, offsets);
        }
    }

    // Draw text

    if(this->font != NULL)
    {
        int h = this->font->getCharHeight();
        int w = this->font->getTextWidth(text);
        int x = (this->width - w)/2;
        int y = (this->height - h)/2 - h/4;

        if(this->shadowFont)
        {
            PKVariant *o = this->get(PKStretchButton::SHADOW_OFFSET_POINT_PROPERTY);
            PKPoint p = PKVALUE_POINT(o);
            
            this->shadowFont->drawText(text, buffer, x+p.x, y+p.y, this->width, this->height);
        }

        this->font->drawText(text, buffer, x, y, this->width, this->height);
    }
}


