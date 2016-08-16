//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKToolbarSegmentedItem.h"
#include "PKImageLoader.h"

//
// PROPERTIES
//

std::string PKToolbarSegmentedItem::ID_STRING_PROPERTY            = "id";
std::string PKToolbarSegmentedItem::TEXT_WSTRING_PROPERTY         = "text";
std::string PKToolbarSegmentedItem::NORMAL_IMAGE_WSTRING_PROPERTY = "normalImage";
std::string PKToolbarSegmentedItem::DOWN_IMAGE_WSTRING_PROPERTY   = "downImage";
std::string PKToolbarSegmentedItem::HOVER_IMAGE_WSTRING_PROPERTY  = "hoverImage";
std::string PKToolbarSegmentedItem::HELP_WSTRINGLIST_PROPERTY     = "help";
std::string PKToolbarSegmentedItem::NUM_BUTTONS_UINT32_PROPERTY   = "numButtons";
std::string PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY   = "offsets";
std::string PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY      = "selection";

//
// PKToolbarItem
//

PKToolbarSegmentedItem::PKToolbarSegmentedItem()
{
	std::vector<std::wstring> helpTags;
	std::vector<std::string>  offs;
	
	PKOBJECT_ADD_STRING_PROPERTY(id, "");

	PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(normalImage, L"NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(downImage, L"NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(hoverImage, L"NOT_SET");
	PKOBJECT_ADD_WSTRINGLIST_PROPERTY(help, helpTags);
	PKOBJECT_ADD_UINT32_PROPERTY(numButtons, 0);
	PKOBJECT_ADD_STRINGLIST_PROPERTY(offsets, offs);
    PKOBJECT_ADD_INT32_PROPERTY(selection, 0);
	
	this->id = 0;

#ifdef MACOSX
	this->normalImage = NULL;
	this->downImage   = NULL;
	this->hoverImage  = NULL;

	this->dialog = NULL;
#endif
}

PKToolbarSegmentedItem::~PKToolbarSegmentedItem()
{
	
}

uint32_t PKToolbarSegmentedItem::getId()
{
	return this->id;
}

void PKToolbarSegmentedItem::setId(uint32_t id)
{
	this->id = id;
}

#ifdef MACOSX

void PKToolbarSegmentedItem::loadImages()
{
	if(this->normalImage == NULL)
	{
		this->normalImage = this->loadBitmap(PKToolbarSegmentedItem::NORMAL_IMAGE_WSTRING_PROPERTY);
	}

	if(this->hoverImage == NULL)
	{
		this->hoverImage = this->loadBitmap(PKToolbarSegmentedItem::HOVER_IMAGE_WSTRING_PROPERTY);
	}

	if(this->downImage == NULL)
	{
		this->downImage = this->loadBitmap(PKToolbarSegmentedItem::DOWN_IMAGE_WSTRING_PROPERTY);
	}
}

PKImage *PKToolbarSegmentedItem::loadBitmap(std::string propertyName)
{
    PKImage *bitmap = NULL;
	
    PKVariant *img = this->get(propertyName);
	std::wstring URI = PKVALUE_WSTRING(img);
	
	PKImageURIType type;
	std::wstring   image;
	
	PKImage::processURI(URI, &type, &image);
	
	if(type == PKIMAGE_URI_RESOURCE)
	{
    	bitmap = PKImageLoader::loadBitmapFromResource(image);
	}
	
	if(type == PKIMAGE_URI_FILENAME)
	{
    	bitmap = PKImageLoader::loadBitmapFromFilename(image);
	}
	
    return bitmap;	
}

#endif
