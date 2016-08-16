//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKBitmapLabel.h"
#include "PKStr.h"

//
// PKBitmapLabel
//

PKBitmapLabel::PKBitmapLabel()
{
	this->wtext      = L"";

	this->align      = PK_BITMAP_ALIGN_LEFT;
	this->clipped    = true;

	this->font         = NULL;
	this->shadowFont   = NULL;
	this->disabledFont = NULL;

    this->w = 0;
    this->h = 0;
    this->enabled = true;
}

PKBitmapLabel::PKBitmapLabel(PKBitmapLabel *copy)
{
	this->wtext      = copy->wtext;

	this->align      = PK_BITMAP_ALIGN_LEFT;
	this->clipped    = true;

	this->font         = NULL;
	this->shadowFont   = NULL;
	this->disabledFont = NULL;

	// Copy

	this->align   = copy->align;
	this->clipped = copy->clipped;

    this->w = copy->w;
    this->h = copy->h;
    this->enabled = copy->enabled;

	if(copy->font != NULL)
	{
		this->font = new PKFont(copy->font);
	}

	if(copy->shadowFont != NULL)
	{
		this->shadowFont = new PKFont(copy->shadowFont);
	}

	if(copy->disabledFont != NULL)
	{
		this->disabledFont = new PKFont(copy->disabledFont);
	}
}

PKBitmapLabel::~PKBitmapLabel()
{
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

void PKBitmapLabel::setClipped(bool clipped)
{
	this->clipped = clipped;
}

bool PKBitmapLabel::getClipped()
{
	return this->clipped;
}

void PKBitmapLabel::setAlign(PKBitmapLabelAlign align)
{
	this->align = align;
}

PKBitmapLabelAlign PKBitmapLabel::getAlign()
{
	return this->align;
}

uint32_t PKBitmapLabel::getMinimumSize()
{
	if(this->getClipped())
	{
		return this->w;
	}
	else
	{
		int width  = this->font->getTextWidth(this->wtext);
		return width;
	}
}

void PKBitmapLabel::setSize(uint32_t w, uint32_t h)
{
    this->w = w;
    this->h = h;
}

void PKBitmapLabel::setEnabled(bool e)
{
    this->enabled = e;
}

void PKBitmapLabel::setText(std::string text)
{
	this->wtext = PKStr::stringToWString(text);

	if(this->font == NULL)
	{
		return;
	}

	if(!this->getClipped())
	{
		//
		// We need to set our size according to 
		// the current text being assigned to us...
		//

		int height = this->font->getCharHeight();
		int width  = this->font->getTextWidth(this->wtext);

		this->setSize(width, height);
	}
}

void PKBitmapLabel::setWText(std::wstring wtext)
{
	this->wtext = wtext;

	if(this->font == NULL)
	{
		return;
	}

	if(!this->getClipped())
	{
		//
		// We need to set our size according to 
		// the current text being assigned to us...
		//

		int height = this->font->getCharHeight();
		int width  = this->font->getTextWidth(this->wtext);

		this->setSize(width, height);
	}
}

std::wstring PKBitmapLabel::getWText()
{
	return this->wtext;
}

void PKBitmapLabel::setFont(PKFont *font)
{
	this->font = font;
}

PKFont *PKBitmapLabel::getFont()
{
	return this->font;
}

void PKBitmapLabel::setShadowFont(PKFont *font)
{
	this->shadowFont = font;
}

PKFont *PKBitmapLabel::getShadowFont()
{
	return this->shadowFont;
}

void PKBitmapLabel::setDisabledFont(PKFont *font)
{
	this->disabledFont = font;
}

PKFont *PKBitmapLabel::getDisabledFont()
{
	return this->disabledFont;
}

bool PKBitmapLabel::draw(PKImage *bitmap,
			             int x, int y)
{
	return this->alphaDraw(255, bitmap, x, y);
}

bool PKBitmapLabel::alphaDraw(unsigned char alpha,
					          PKImage *bitmap,
					          int x, int y)
{
	if(this->font == NULL)
	{
		return false;
	}

	PKFont *f = NULL;

	if(this->enabled)
	{
		f = this->font;
	}
	else
	{
		f = this->disabledFont;

		if(f == NULL)
		{
			f = this->font;
		}
	}

	if(f == NULL)
	{
		return false;
	}

	unsigned int dotSize = 0;
	
	dotSize	= f->getCharWidth(L'.');

	unsigned int i = 0;
	int xOffset = 0;
	int startX  = 0;
	int textW   = 0;

	textW = f->getTextWidth(this->wtext);

	switch(this->align)
	{
	case PK_BITMAP_ALIGN_RIGHT:
		{
			startX = x + this->w - textW;
		}
		break;

	case PK_BITMAP_ALIGN_CENTER:
		{
			startX = x + ((w - textW) / 2);

			if(startX < x)
			{
				startX = x;
			}
		}
		break;

	case PK_BITMAP_ALIGN_LEFT:
	default:
		{
			startX = x;
		}
		break;
	}

	if(this->wtext != L"")
	{
		while((i < this->wtext.size()) && 
			((int)(xOffset + f->getCharWidth(this->wtext[i])) <= w))
		{
			if(this->shadowFont != NULL)
			{
				int offset = this->font->getCharHeight() / 5;

				this->shadowFont->drawChar(this->wtext[i],
										   bitmap, startX + xOffset + offset,
										   y + offset, alpha);
			}

			f->drawChar(this->wtext[i],
						   bitmap, startX + xOffset,
						   y, alpha);
		
			xOffset += f->getCharWidth(this->wtext[i]);

			i++;
		}

		//
		// If we didn't print all the letters
		// we still have the "..." to print...
		//

		if(i != this->wtext.size())
		{
			for(int j=0; j < 3; j++)
			{
				if(this->shadowFont != NULL)
				{
					int offset = this->font->getCharHeight() / 5;
	
					this->shadowFont->drawChar(L'.', bitmap,
												startX + xOffset + offset,
												y + offset, alpha);
				}

				f->drawChar(L'.', bitmap,
							   startX + xOffset,
							   y, alpha);
		
				xOffset += dotSize;
			}
		}	
	}

	return true;
}


