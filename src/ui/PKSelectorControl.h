//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SELECTOR_CONTROL_H
#define PK_SELECTOR_CONTROL_H

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"

//
// PKSelectorControl
//

class PKSelectorControl : public PKDoubleBufferedControl
{
public:

    static std::string SELECTION_INT32_PROPERTY;
	static std::string NORMAL_IMAGE_WSTRING_PROPERTY;
	static std::string PUSHED_IMAGE_WSTRING_PROPERTY;
	static std::string DISABLED_IMAGE_WSTRING_PROPERTY;
    static std::string NUM_ITEMS_UINT32_PROPERTY;
    static std::string OFFSETS_STRINGLIST_PROPERTY;

public:
	PKSelectorControl();

protected:
	virtual ~PKSelectorControl();

public:
    virtual void build();

    // From PKCustomControl
	
    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

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

protected:

    virtual void draw32bpp(uint8_t *buffer);

private:

    PKImage *loadBitmap(std::string propertyName);

	uint32_t width;
	uint32_t height;
	PKImage *normal_bitmap;
	PKImage *pushed_bitmap;
	PKImage *disabled_bitmap;
	bool built;   
	int32_t downButton;
};

#endif // PK_SELECTOR_CONTROL_H
