//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAP_LABEL_H
#define PK_BITMAP_LABEL_H

#include "PKFont.h"

typedef enum 
{
	PK_BITMAP_ALIGN_LEFT,
	PK_BITMAP_ALIGN_CENTER,
	PK_BITMAP_ALIGN_RIGHT,

} PKBitmapLabelAlign;

//
// PKBitmapLabel
// 

class PKBitmapLabel 
{
public:

	PKBitmapLabel();
	PKBitmapLabel(PKBitmapLabel *copy);
	virtual ~PKBitmapLabel();

	void   setText(std::string text);

	void	           setAlign(PKBitmapLabelAlign align);
	PKBitmapLabelAlign getAlign();

	void		 setWText(std::wstring text);
	std::wstring getWText();

	void    setFont(PKFont *font);
	PKFont *getFont();

	void    setShadowFont(PKFont *font);
	PKFont *getShadowFont();

	void    setDisabledFont(PKFont *font);
	PKFont *getDisabledFont();

	void setClipped(bool clipped);
	bool getClipped();

    void setSize(uint32_t w, uint32_t h);
    void setEnabled(bool e);

	uint32_t getMinimumSize();

	bool draw(PKImage *bitmap,
			  int x, 
              int y);

	bool alphaDraw(unsigned char alpha,
				   PKImage *bitmap,
				   int x, 
                   int y);
private:

	std::wstring wtext;
	PKFont      *font;
	PKFont      *shadowFont;
	PKFont      *disabledFont;
	PKBitmapLabelAlign align;

	bool clipped;
    uint32_t w;
    uint32_t h;
    bool enabled;
};

#endif // PK_BITMAP_LABEL_H
