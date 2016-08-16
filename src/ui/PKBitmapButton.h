//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAP_BUTTON_H
#define PK_BITMAP_BUTTON_H

#define WM_BITMAP_BUTTON_FIRE (WM_USER+110)

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"

#ifdef MACOSX

enum {
	kEventClassPKButton  = FOUR_CHAR_CODE('PKBT'),
	kEventPKButtonTrack  = 1,
};

#endif

//
// PKBitmapButton
//

class PKBitmapButton : public PKDoubleBufferedControl
{
public:

	static std::string NORMAL_IMAGE_WSTRING_PROPERTY;
	static std::string HOVER_IMAGE_WSTRING_PROPERTY;
	static std::string PUSHED_IMAGE_WSTRING_PROPERTY;
	static std::string DISABLED_IMAGE_WSTRING_PROPERTY;

public:
	PKBitmapButton();

protected:
	virtual ~PKBitmapButton();

public:
    virtual void build();

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
};

#endif // PK_BITMAP_BUTTON_H
