//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKStretchToggleButton.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKFont.h"
#include "PKBitmapOps.h"
#include "PKDialog.h"
#include "PKStr.h"

//
// PROPERTIES
//

std::string PKStretchToggleButton::NORMAL_IMAGE_WSTRING_PROPERTY         = "normalImage";
std::string PKStretchToggleButton::HOVER_IMAGE_WSTRING_PROPERTY          = "hoverImage";
std::string PKStretchToggleButton::PUSHED_IMAGE_WSTRING_PROPERTY         = "pushedImage";
std::string PKStretchToggleButton::DISABLED_IMAGE_WSTRING_PROPERTY       = "disabledImage";
std::string PKStretchToggleButton::TOGGLED_NORMAL_IMAGE_WSTRING_PROPERTY = "toggledNormalImage";
std::string PKStretchToggleButton::TOGGLED_HOVER_IMAGE_WSTRING_PROPERTY  = "toggledHoverImage";
std::string PKStretchToggleButton::TOGGLED_PUSHED_IMAGE_WSTRING_PROPERTY = "toggledPushedImage";
std::string PKStretchToggleButton::TOGGLED_BOOL_PROPERTY                 = "toggled";
std::string PKStretchToggleButton::STRETCH_OFFSET_UINT32_PROPERTY        = "stretchOffset";
std::string PKStretchToggleButton::TEXT_OFFSET_UINT32_PROPERTY           = "textOffset";
std::string PKStretchToggleButton::TEXT_WSTRING_PROPERTY                 = "text";
std::string PKStretchToggleButton::FONTNAME_STRING_PROPERTY              = "fontName";
std::string PKStretchToggleButton::FONTSIZE_UINT32_PROPERTY              = "fontSize";
std::string PKStretchToggleButton::RELATIVEFONTSIZE_STRING_PROPERTY      = "relativeFontSize";
std::string PKStretchToggleButton::COLOR_COLOR_PROPERTY                  = "color";
std::string PKStretchToggleButton::SHADOW_COLOR_COLOR_PROPERTY           = "shadowColor";
std::string PKStretchToggleButton::DISABLED_COLOR_COLOR_PROPERTY         = "disabledColor";
std::string PKStretchToggleButton::BOLD_BOOL_PROPERTY                    = "bold";
std::string PKStretchToggleButton::ITALICS_BOOL_PROPERTY                 = "italics";
std::string PKStretchToggleButton::SHADOW_OFFSET_POINT_PROPERTY          = "shadowOffset";
std::string PKStretchToggleButton::PREVENT_TOGGLE_OFF_BOOL_PROPERTY      = "preventToggleOff";

//
// PKStretchButton
//

PKStretchToggleButton::PKStretchToggleButton()
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
    PKOBJECT_ADD_WSTRING_PROPERTY(toggledNormalImage,   L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(toggledHoverImage,    L"IMG_NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(toggledPushedImage,   L"IMG_NOT_SET");
    PKOBJECT_ADD_BOOL_PROPERTY(toggled, false);
    PKOBJECT_ADD_UINT32_PROPERTY(stretchOffset, 0);
    PKOBJECT_ADD_UINT32_PROPERTY(textOffset, 0);
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
	PKOBJECT_ADD_BOOL_PROPERTY(preventToggleOff, false);

	this->width     = 0;
	this->height    = 0;

	this->normal_bitmap   = NULL;
	this->hover_bitmap    = NULL;
	this->pushed_bitmap   = NULL;
	this->disabled_bitmap = NULL;
	
    this->toggled_normal_bitmap   = NULL;
	this->toggled_hover_bitmap    = NULL;
	this->toggled_pushed_bitmap   = NULL;

    this->font = NULL;

    this->shadowFont   = NULL;
    this->disabledFont = NULL;
}

PKStretchToggleButton::~PKStretchToggleButton()
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

PKImage *PKStretchToggleButton::loadBitmap(std::string propertyName)
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


void PKStretchToggleButton::build()
{
    if(this->built)
    {
        return;
    }

    PKVariant *n = this->get(PKStretchToggleButton::FONTNAME_STRING_PROPERTY);
    std::string fontName = PKVALUE_STRING(n);

    PKVariant *s = this->get(PKStretchToggleButton::FONTSIZE_UINT32_PROPERTY);
    uint32_t fontSize = PKVALUE_UINT32(s);

    PKVariant *rs = this->get(PKStretchToggleButton::RELATIVEFONTSIZE_STRING_PROPERTY);
    std::string relativeSize = PKVALUE_STRING(rs);

	PKVariant *b = this->get(PKStretchToggleButton::BOLD_BOOL_PROPERTY);
    bool bold = PKVALUE_BOOL(b);

	PKVariant *i = this->get(PKStretchToggleButton::ITALICS_BOOL_PROPERTY);
    bool italics = PKVALUE_BOOL(i);

    PKVariant *c = this->get(PKStretchToggleButton::COLOR_COLOR_PROPERTY);
    PKColor fontColor = PKVALUE_COLOR(c);

    PKVariant *d = this->get(PKStretchToggleButton::DISABLED_COLOR_COLOR_PROPERTY);
    PKColor disabledColor = PKVALUE_COLOR(d);

    PKVariant *sh = this->get(PKStretchToggleButton::SHADOW_COLOR_COLOR_PROPERTY);
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

    this->normal_bitmap = this->loadBitmap(PKStretchToggleButton::NORMAL_IMAGE_WSTRING_PROPERTY);

    if(this->normal_bitmap == NULL)
    {
        return;
    }

    this->premultiplyAlpha(this->normal_bitmap);

    // Load the other images if present

    this->hover_bitmap = this->loadBitmap(PKStretchToggleButton::HOVER_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->hover_bitmap);

    this->pushed_bitmap = this->loadBitmap(PKStretchToggleButton::PUSHED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->pushed_bitmap);

    this->disabled_bitmap = this->loadBitmap(PKStretchToggleButton::DISABLED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->disabled_bitmap);

    this->toggled_normal_bitmap = this->loadBitmap(PKStretchToggleButton::TOGGLED_NORMAL_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->toggled_normal_bitmap);

    this->toggled_hover_bitmap = this->loadBitmap(PKStretchToggleButton::TOGGLED_HOVER_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->toggled_hover_bitmap);

    this->toggled_pushed_bitmap = this->loadBitmap(PKStretchToggleButton::TOGGLED_PUSHED_IMAGE_WSTRING_PROPERTY);
    this->premultiplyAlpha(this->toggled_pushed_bitmap);

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

void PKStretchToggleButton::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    if(!this->built)
    {
		return;
	}
	
	if(prop->getName() == PKStretchToggleButton::TEXT_WSTRING_PROPERTY)
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

void PKStretchToggleButton::mouseMove(int x,
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

void PKStretchToggleButton::mouseLeave()
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

void PKStretchToggleButton::mouseDown(PKButtonType button,
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

void PKStretchToggleButton::mouseDblClick(PKButtonType button,
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

void PKStretchToggleButton::mouseUp(PKButtonType button,
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

                PKVariant *t = this->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY);
                bool toggled = PKVALUE_BOOL(t);

				bool preventOff = PKVALUE_BOOL(this->get(PKStretchToggleButton::PREVENT_TOGGLE_OFF_BOOL_PROPERTY));

				if(toggled && preventOff)
				{
					// Do nothing
				}
				else // toggle
				{
					toggled = !toggled;

					PKVariantBool newVal(toggled);
					this->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &newVal);

					if(this->dialog != NULL)
					{                    
						dialog->stretchToggleButtonToggled(this->getControlId(), this, !toggled);
					}				
				}
            }
        }

        this->down = false;
        this->invalidate();
    }
}


uint32_t PKStretchToggleButton::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

    if(this->getLayoutPolicyX() != PK_LAYOUT_FIXED)
    {
        if(this->normal_bitmap && this->font)
        {
	        PKVariant *t = this->get(PKStretchToggleButton::TEXT_WSTRING_PROPERTY);
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

uint32_t PKStretchToggleButton::getMinimumHeight()
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

void PKStretchToggleButton::resize(uint32_t w, 
			                       uint32_t h)
{
    PKDoubleBufferedControl::resize(w, h);
    this->width  = w;
    this->height = h;
}

void PKStretchToggleButton::draw32bpp(uint8_t *buffer)
{
	// Draw current frame

    if(this->normal_bitmap == NULL || 
       this->toggled_normal_bitmap == NULL ||
       this->font == NULL)
    {
        return;
    }

    PKVariant *t = this->get(PKStretchToggleButton::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

    PKVariant *toff  = this->get(PKStretchToggleButton::TEXT_OFFSET_UINT32_PROPERTY);
    uint32_t toffset = PKVALUE_UINT32(toff);

    PKVariant *o    = this->get(PKStretchToggleButton::STRETCH_OFFSET_UINT32_PROPERTY);
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

			int h = this->disabledFont->getCharHeight();
			int w = this->disabledFont->getTextWidth(text);
			int x = (this->width - w)/2;
			int y = (this->height - h)/2 - h/4;

            font->drawText(text, buffer, x, y+toffset, this->width, this->height);

            return;
        }
    }

    PKVariant *to = this->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY);
    bool toggled = PKVALUE_BOOL(to);

    // Draw background

    if(toggled)
    {
        if(this->down && this->toggled_pushed_bitmap)
        {
            PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                          this->toggled_pushed_bitmap, offsets);
        }
        else
        {
            if(this->hover && this->toggled_hover_bitmap)
            {
                PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                              this->toggled_hover_bitmap, offsets);
            }
            else
            {
                PKBitmapOps::fillStretchWidth(buffer, this->width, this->height,
                                              this->toggled_normal_bitmap, offsets);
            }
        }
    }
    else
    {
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
            PKVariant *o = this->get(PKStretchToggleButton::SHADOW_OFFSET_POINT_PROPERTY);
            PKPoint p = PKVALUE_POINT(o);
            
            this->shadowFont->drawText(text, buffer, x+p.x, y+p.y+toffset, this->width, this->height);
        }

        this->font->drawText(text, buffer, x, y+toffset, this->width, this->height);
    }
}


