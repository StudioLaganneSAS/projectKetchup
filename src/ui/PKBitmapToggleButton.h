//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAP_TOGGLE_BUTTON_H
#define PK_BITMAP_TOGGLE_BUTTON_H

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"

//
// PKBitmapButton
//

class PKBitmapToggleButton : public PKDoubleBufferedControl
{
public:

    static std::string TOGGLED_BOOL_PROPERTY;
	static std::string NORMAL_IMAGE_WSTRING_PROPERTY;
	static std::string HOVER_IMAGE_WSTRING_PROPERTY;
	static std::string PUSHED_IMAGE_WSTRING_PROPERTY;
	static std::string DISABLED_IMAGE_WSTRING_PROPERTY;
	static std::string TOGGLED_NORMAL_IMAGE_WSTRING_PROPERTY;
	static std::string TOGGLED_HOVER_IMAGE_WSTRING_PROPERTY;
	static std::string TOGGLED_PUSHED_IMAGE_WSTRING_PROPERTY;
	static std::string PREVENT_TOGGLE_OFF_BOOL_PROPERTY;

public:
	PKBitmapToggleButton();

protected:
	virtual ~PKBitmapToggleButton();

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
	
	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

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
	PKImage *toggled_normal_bitmap;
	PKImage *toggled_hover_bitmap;
	PKImage *toggled_pushed_bitmap;
	bool built;   
    bool hover;
    bool down;
};

#endif // PK_BITMAP_TOGGLE_BUTTON_H
