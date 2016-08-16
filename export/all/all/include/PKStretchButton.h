//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_STRETCH_BUTTON_H
#define PK_STRETCH_BUTTON_H

#define WM_STRETCH_BITMAP_BUTTON_FIRE (WM_USER+120)

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"

class PKFont;

#ifdef MACOSX

enum {
	kEventClassPKStretchButton  = FOUR_CHAR_CODE('PKSB'),
	kEventPKStretchButtonTrack  = 1,
};

#endif

//
// PKStretchButton
//

class PKStretchButton : public PKDoubleBufferedControl
{
public:

	static std::string NORMAL_IMAGE_WSTRING_PROPERTY;
	static std::string HOVER_IMAGE_WSTRING_PROPERTY;
	static std::string PUSHED_IMAGE_WSTRING_PROPERTY;
	static std::string DISABLED_IMAGE_WSTRING_PROPERTY;
    static std::string STRETCH_OFFSET_UINT32_PROPERTY;
    static std::string TEXT_WSTRING_PROPERTY;
    static std::string FONTNAME_STRING_PROPERTY;
    static std::string FONTSIZE_UINT32_PROPERTY;
    static std::string RELATIVEFONTSIZE_STRING_PROPERTY;
    static std::string COLOR_COLOR_PROPERTY;
    static std::string SHADOW_COLOR_COLOR_PROPERTY;
    static std::string DISABLED_COLOR_COLOR_PROPERTY;
	static std::string BOLD_BOOL_PROPERTY;
	static std::string ITALICS_BOOL_PROPERTY;
    static std::string SHADOW_OFFSET_POINT_PROPERTY;

public:
	PKStretchButton();

protected:
	virtual ~PKStretchButton();

public:
    virtual void build();

    // From PKControl

    void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

    void resize(uint32_t w, 
		        uint32_t h);

    // From PKCustomControl
	
    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

    // From PKCustomControl
	
    void mouseMove(int x,
				   int y,
				   unsigned int mod);

	void mouseLeave();

    void mouseDown(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod);

	void mouseUp(PKButtonType button,
				 int x,
				 int y,
				 unsigned int mod);

	void mouseDblClick(PKButtonType button,
					   int x,
					   int y,
					   unsigned int mod);

protected:

    virtual void draw32bpp(uint8_t *buffer);

private:

    PKImage *loadBitmap(std::string propertyName);

	uint32_t width;
	uint32_t height;
	PKImage *normal_bitmap;
	PKImage *hover_bitmap;
	PKImage *pushed_bitmap;
	PKImage *disabled_bitmap;
	bool built;   
    bool hover;
    bool down;
    PKFont *font;
    PKFont *shadowFont;
    PKFont *disabledFont;
};

#endif // PK_STRETCH_BUTTON
