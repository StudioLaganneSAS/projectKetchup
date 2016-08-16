//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_TOOLBAR_SEGMENTED_ITEM_H
#define PK_TOOLBAR_SEGMENTED_ITEM_H

#include "PKUI.h"
#include "PKObject.h"
#include "PKImage.h"

class PKDialog;

//
// PKToolbarItem
//

class PKToolbarSegmentedItem : public PKObject
{
public:
	
	//
	// PROPERTIES
	//
	
	static std::string ID_STRING_PROPERTY;
	static std::string TEXT_WSTRING_PROPERTY;
	static std::string NORMAL_IMAGE_WSTRING_PROPERTY;
	static std::string DOWN_IMAGE_WSTRING_PROPERTY;
	static std::string HOVER_IMAGE_WSTRING_PROPERTY;
	static std::string NUM_BUTTONS_UINT32_PROPERTY;
	static std::string OFFSETS_STRINGLIST_PROPERTY;
	static std::string HELP_WSTRINGLIST_PROPERTY;
    static std::string SELECTION_INT32_PROPERTY;

public:	
	
	PKToolbarSegmentedItem();

protected:
	virtual ~PKToolbarSegmentedItem();
	
public:
    uint32_t getId();
	void     setId(uint32_t id);
	
private:

	uint32_t id;
	
#ifdef MACOSX
public:
	
	PKDialog *dialog;
	
	PKImage *normalImage;
	PKImage *downImage;
	PKImage *hoverImage;
	
	void loadImages();
	
private:
    PKImage *loadBitmap(std::string propertyName);

#endif
};

#endif // PK_TOOLBAR_SEGMENTED_ITEM_H
