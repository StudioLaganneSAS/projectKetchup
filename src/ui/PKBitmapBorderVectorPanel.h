//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAP_BORDER_VECTOR_PANEL_H
#define PK_BITMAP_BORDER_VECTOR_PANEL_H

#include "PKVectorControl.h"
#include "PKImage.h"

//
// PKBitmapBorderVectorPanel
//

class PKBitmapBorderVectorPanel : public PKVectorControl
{
public:

    //
    // PROPERTIES
    //

    static std::string BACKCOLOR_COLOR_PROPERTY;
	static std::string BORDER_WIDTH_UINT32_PROPERTY;

	static std::string TOPLEFT_IMAGE_WSTRING_PROPERTY;
	static std::string TOPRIGHT_IMAGE_WSTRING_PROPERTY;
	static std::string BOTTOMLEFT_IMAGE_WSTRING_PROPERTY;
	static std::string BOTTOMRIGHT_IMAGE_WSTRING_PROPERTY;

	static std::string LEFTSTART_IMAGE_WSTRING_PROPERTY;
	static std::string LEFTMIDDLE_IMAGE_WSTRING_PROPERTY;
	static std::string LEFTEND_IMAGE_WSTRING_PROPERTY;
	
	static std::string RIGHTSTART_IMAGE_WSTRING_PROPERTY;
	static std::string RIGHTMIDDLE_IMAGE_WSTRING_PROPERTY;
	static std::string RIGHTEND_IMAGE_WSTRING_PROPERTY;

	static std::string TOPSTART_IMAGE_WSTRING_PROPERTY;
	static std::string TOPMIDDLE_IMAGE_WSTRING_PROPERTY;
	static std::string TOPEND_IMAGE_WSTRING_PROPERTY;

	static std::string BOTTOMSTART_IMAGE_WSTRING_PROPERTY;
	static std::string BOTTOMMIDDLE_IMAGE_WSTRING_PROPERTY;
	static std::string BOTTOMEND_IMAGE_WSTRING_PROPERTY;
	
public:
	PKBitmapBorderVectorPanel();

protected:
	virtual ~PKBitmapBorderVectorPanel();

public:
    virtual void build();

	// From PKVectorControl

	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

protected:

    virtual void drawVector(PKVectorEngine *engine,
		                    int32_t x, int32_t y);

private:

    PKImage *loadBitmap(std::string propertyName);
	
	PKImage *topLeftImage;
	PKImage *topRightImage;
	PKImage *bottomLeftImage;
	PKImage *bottomRightImage;

	PKImage *topStartImage;
	PKImage *topMiddleImage;
	PKImage *topEndImage;

	PKImage *leftStartImage;
	PKImage *leftMiddleImage;
	PKImage *leftEndImage;

	PKImage *rightStartImage;
	PKImage *rightMiddleImage;
	PKImage *rightEndImage;

	PKImage *bottomStartImage;
	PKImage *bottomMiddleImage;
	PKImage *bottomEndImage;
	
	bool built;
};

#endif // PK_BITMAP_BORDER_VECTOR_PANEL_H
