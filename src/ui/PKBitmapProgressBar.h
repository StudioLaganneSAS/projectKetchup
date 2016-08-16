//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAP_PROGRESSBAR_H
#define PK_BITMAP_PROGRESSBAR_H

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"

//
// PKBitmapProgressBar
//

class PKBitmapProgressBar : public PKDoubleBufferedControl
{
public:

	static std::string EMPTY_IMAGE_WSTRING_PROPERTY;
	static std::string FILLED_IMAGE_WSTRING_PROPERTY;
	static std::string DISABLED_IMAGE_WSTRING_PROPERTY;
    static std::string VALUE_INT32_PROPERTY;
    static std::string RANGE_INT32RANGE_PROPERTY;

public:
	PKBitmapProgressBar();

protected:
	virtual ~PKBitmapProgressBar();

public:
    virtual void build();

    // From PKCustomControl
	
    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

protected:

    virtual void draw32bpp(uint8_t *buffer);

private:

    PKImage *loadBitmap(std::string propertyName);

	uint32_t height;
	PKImage *empty_bitmap;
	PKImage *filled_bitmap;
	PKImage *disabled_bitmap;
	bool built;   
};

#endif // PK_BITMAP_PROGRESSBAR_H
