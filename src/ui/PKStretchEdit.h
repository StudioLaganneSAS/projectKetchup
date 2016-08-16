//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_STRETCH_EDIT_H
#define PK_STRETCH_EDIT_H

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"
#include "PKEdit.h"

//
// PKStretchEdit
//

class PKStretchEdit : public PKDoubleBufferedControl
{
public:

	static std::string NORMAL_IMAGE_WSTRING_PROPERTY;
	static std::string DISABLED_IMAGE_WSTRING_PROPERTY;
    static std::string STRETCH_OFFSET_UINT32_PROPERTY;
    static std::string TEXT_WSTRING_PROPERTY;
    static std::string OFFSET_TOP_UINT32_PROPERTY;
    static std::string OFFSET_LEFT_UINT32_PROPERTY;
    static std::string OFFSET_RIGHT_UINT32_PROPERTY;
    static std::string OFFSET_BOTTOM_UINT32_PROPERTY;

public:
	PKStretchEdit();

protected:
	virtual ~PKStretchEdit();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);

    // From PKControl

    void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

    void resize(uint32_t w, 
		        uint32_t h);

    // From PKCustomControl
	
    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

    // from PKControl

	void move(int32_t x, 
    		  int32_t y);

    void show();
	void hide();

	void enable();
	void disable();

protected:

    virtual void draw32bpp(uint8_t *buffer);

private:

    PKImage *loadBitmap(std::string propertyName);

	uint32_t width;
	uint32_t height;
	PKImage *normal_bitmap;
	PKImage *disabled_bitmap;
	bool built;   

#ifdef WIN32
    PKEdit *edit;
#endif
	
#ifdef MACOSX
	HIViewRef textView;
#endif
};

#endif // PK_STRETCH_BUTTON
