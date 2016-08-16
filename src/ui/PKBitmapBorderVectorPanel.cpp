//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKBitmapBorderVectorPanel.h"
#include "PKVectorEngine.h"
#include "PKImageLoader.h"
#include "PKStr.h"

//
// PROPERTIES
//

std::string PKBitmapBorderVectorPanel::BACKCOLOR_COLOR_PROPERTY     = "backColor";
std::string PKBitmapBorderVectorPanel::BORDER_WIDTH_UINT32_PROPERTY = "borderWidth";

std::string PKBitmapBorderVectorPanel::TOPLEFT_IMAGE_WSTRING_PROPERTY      = "topLeftImage";
std::string PKBitmapBorderVectorPanel::TOPRIGHT_IMAGE_WSTRING_PROPERTY     = "topRightImage";
std::string PKBitmapBorderVectorPanel::BOTTOMLEFT_IMAGE_WSTRING_PROPERTY   = "bottomLeftImage";
std::string PKBitmapBorderVectorPanel::BOTTOMRIGHT_IMAGE_WSTRING_PROPERTY  = "bottomRightImage";

std::string PKBitmapBorderVectorPanel::LEFTSTART_IMAGE_WSTRING_PROPERTY    = "leftStartImage";
std::string PKBitmapBorderVectorPanel::LEFTMIDDLE_IMAGE_WSTRING_PROPERTY   = "leftMiddleImage";
std::string PKBitmapBorderVectorPanel::LEFTEND_IMAGE_WSTRING_PROPERTY      = "leftEndImage";

std::string PKBitmapBorderVectorPanel::RIGHTSTART_IMAGE_WSTRING_PROPERTY   = "rightStartImage";
std::string PKBitmapBorderVectorPanel::RIGHTMIDDLE_IMAGE_WSTRING_PROPERTY  = "rightMiddleImage";
std::string PKBitmapBorderVectorPanel::RIGHTEND_IMAGE_WSTRING_PROPERTY     = "rightEndImage";

std::string PKBitmapBorderVectorPanel::TOPSTART_IMAGE_WSTRING_PROPERTY     = "topStartImage";
std::string PKBitmapBorderVectorPanel::TOPMIDDLE_IMAGE_WSTRING_PROPERTY    = "topMiddleImage";
std::string PKBitmapBorderVectorPanel::TOPEND_IMAGE_WSTRING_PROPERTY       = "topEndImage";

std::string PKBitmapBorderVectorPanel::BOTTOMSTART_IMAGE_WSTRING_PROPERTY  = "bottomStartImage";
std::string PKBitmapBorderVectorPanel::BOTTOMMIDDLE_IMAGE_WSTRING_PROPERTY = "bottomMiddleImage";
std::string PKBitmapBorderVectorPanel::BOTTOMEND_IMAGE_WSTRING_PROPERTY    = "bottomEndImage";


//
// PKBitmapBorderVectorPanel
//

PKBitmapBorderVectorPanel::PKBitmapBorderVectorPanel()
{
    PKColor back = PKColorCreate(255, 255, 255, 255);

    PKOBJECT_ADD_COLOR_PROPERTY(backColor, back);
	PKOBJECT_ADD_UINT32_PROPERTY(borderWidth, 0);
	
	PKOBJECT_ADD_WSTRING_PROPERTY(topLeftImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(topRightImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(bottomLeftImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(bottomRightImage, L"");

	PKOBJECT_ADD_WSTRING_PROPERTY(topStartImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(topMiddleImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(topEndImage, L"");
	
	PKOBJECT_ADD_WSTRING_PROPERTY(leftStartImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(leftMiddleImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(leftEndImage, L"");

	PKOBJECT_ADD_WSTRING_PROPERTY(rightStartImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(rightMiddleImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(rightEndImage, L"");

	PKOBJECT_ADD_WSTRING_PROPERTY(bottomStartImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(bottomMiddleImage, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(bottomEndImage, L"");
	
	this->topLeftImage     = NULL;
	this->topRightImage    = NULL;
	this->bottomLeftImage  = NULL;
	this->bottomRightImage = NULL;

	this->topStartImage  = NULL;
	this->topMiddleImage = NULL;
	this->topEndImage    = NULL;

	this->leftStartImage  = NULL;
	this->leftMiddleImage = NULL;
	this->leftEndImage    = NULL;

	this->rightStartImage  = NULL;
	this->rightMiddleImage = NULL;
	this->rightEndImage    = NULL;

	this->bottomStartImage  = NULL;
	this->bottomMiddleImage = NULL;
	this->bottomEndImage    = NULL;
	
	this->built = false;
}

PKBitmapBorderVectorPanel::~PKBitmapBorderVectorPanel()
{
}

void PKBitmapBorderVectorPanel::build()
{
	PKVectorControl::build();
	
	// Load images
	
	this->topLeftImage      = this->loadBitmap(PKBitmapBorderVectorPanel::TOPLEFT_IMAGE_WSTRING_PROPERTY);
	this->topRightImage     = this->loadBitmap(PKBitmapBorderVectorPanel::TOPRIGHT_IMAGE_WSTRING_PROPERTY);
	this->bottomLeftImage   = this->loadBitmap(PKBitmapBorderVectorPanel::BOTTOMLEFT_IMAGE_WSTRING_PROPERTY);
	this->bottomRightImage  = this->loadBitmap(PKBitmapBorderVectorPanel::BOTTOMRIGHT_IMAGE_WSTRING_PROPERTY);	

	this->topStartImage     = this->loadBitmap(PKBitmapBorderVectorPanel::TOPSTART_IMAGE_WSTRING_PROPERTY);
	this->topMiddleImage    = this->loadBitmap(PKBitmapBorderVectorPanel::TOPMIDDLE_IMAGE_WSTRING_PROPERTY);
	this->topEndImage       = this->loadBitmap(PKBitmapBorderVectorPanel::TOPEND_IMAGE_WSTRING_PROPERTY);
	
	this->leftStartImage    = this->loadBitmap(PKBitmapBorderVectorPanel::LEFTSTART_IMAGE_WSTRING_PROPERTY);
	this->leftMiddleImage   = this->loadBitmap(PKBitmapBorderVectorPanel::LEFTMIDDLE_IMAGE_WSTRING_PROPERTY);
	this->leftEndImage      = this->loadBitmap(PKBitmapBorderVectorPanel::LEFTEND_IMAGE_WSTRING_PROPERTY);
	
	this->rightStartImage   = this->loadBitmap(PKBitmapBorderVectorPanel::RIGHTSTART_IMAGE_WSTRING_PROPERTY);
	this->rightMiddleImage  = this->loadBitmap(PKBitmapBorderVectorPanel::RIGHTMIDDLE_IMAGE_WSTRING_PROPERTY);
	this->rightEndImage     = this->loadBitmap(PKBitmapBorderVectorPanel::RIGHTEND_IMAGE_WSTRING_PROPERTY);
	
	this->bottomStartImage  = this->loadBitmap(PKBitmapBorderVectorPanel::BOTTOMSTART_IMAGE_WSTRING_PROPERTY);
	this->bottomMiddleImage = this->loadBitmap(PKBitmapBorderVectorPanel::BOTTOMMIDDLE_IMAGE_WSTRING_PROPERTY);
	this->bottomEndImage    = this->loadBitmap(PKBitmapBorderVectorPanel::BOTTOMEND_IMAGE_WSTRING_PROPERTY);	
	
	this->built = true;

	if(this->topLeftImage      == NULL ||
	   this->topRightImage     == NULL ||
	   this->bottomLeftImage   == NULL ||
	   this->bottomRightImage  == NULL ||
	   this->topStartImage     == NULL ||
	   this->topMiddleImage    == NULL ||
	   this->topEndImage       == NULL ||
	   this->leftStartImage    == NULL ||
	   this->leftMiddleImage   == NULL ||
	   this->leftEndImage      == NULL ||
   	   this->rightStartImage   == NULL ||
	   this->rightMiddleImage  == NULL ||
	   this->rightEndImage     == NULL ||
	   this->bottomStartImage  == NULL ||
	   this->bottomMiddleImage == NULL ||
	   this->bottomEndImage    == NULL)
	{
		this->built = false;
	}
}

PKImage *PKBitmapBorderVectorPanel::loadBitmap(std::string propertyName)
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


void PKBitmapBorderVectorPanel::drawVector(PKVectorEngine *engine,
		                                   int32_t x, int32_t y)
{
	if(!this->built)
	{
		return;
	}
	
	PKVariant *b = this->get(PKBitmapBorderVectorPanel::BACKCOLOR_COLOR_PROPERTY);
    PKColor back = PKVALUE_COLOR(b);

	PKVariant *bwi = this->get(PKBitmapBorderVectorPanel::BORDER_WIDTH_UINT32_PROPERTY);
    uint32_t bw    = PKVALUE_UINT32(bwi);

	if(engine == NULL)
	{
		return;
	}
	
	int w = this->getW();
	int h = this->getH();
	
	// Draw background
	
	int endx = x+w-bw+1;
	int endy = y+h-bw+1;
	
	engine->fillRect(x+bw-1, y+bw-1, 
					 w-2*bw+2, h-2*bw+2, 
					 back);

	// Draw images - corners
	
	engine->drawImage(this->topLeftImage, x, y);
	engine->drawImage(this->topRightImage, endx, y);
	engine->drawImage(this->bottomLeftImage, x, endy);
	engine->drawImage(this->bottomRightImage, endx, endy);
		
	// Draw top & bottom
	
	int tew = this->topEndImage->getWidth();
	int tsw = this->topStartImage->getWidth();
	int tmw = this->topMiddleImage->getWidth();
 
	engine->drawImage(this->topStartImage, x+bw, y);
	engine->drawImage(this->topEndImage, endx-tew, y);

	engine->drawImage(this->bottomStartImage, x+bw, endy);
	engine->drawImage(this->bottomEndImage, endx-tew, endy);
	
	int topsize = (w - 2*bw - tew - tsw);
	int tcount  = topsize / tmw;
	int tmod    = topsize % tmw;
	
	int32_t i=0;
	
	for(i=0; i<tcount; i++)
	{
		engine->drawImage(this->topMiddleImage, x+bw+tsw+(i*tmw), y);		
		engine->drawImage(this->bottomMiddleImage, x+bw+tsw+(i*tmw), endy);		
	}
	
	engine->drawImageSubImage(this->topMiddleImage, x+bw+tsw+(i*tmw), y, tmod+1);	
	engine->drawImageSubImage(this->bottomMiddleImage, x+bw+tsw+(i*tmw), endy, tmod+1);	
	
	// Draw left & right
	
	int leh = this->leftEndImage->getHeight();
	int lsh = this->leftStartImage->getHeight();
	int lmh = this->leftMiddleImage->getHeight();

	engine->drawImage(this->leftStartImage, x, y+bw);
	engine->drawImage(this->leftEndImage, x, endy-leh);

	engine->drawImage(this->rightStartImage, endx, y+bw);
	engine->drawImage(this->rightEndImage, endx, endy-leh);
	
	int leftsize = (h - 2*bw - leh - lsh);
	int lcount  = leftsize / lmh;
	int lmod    = leftsize % lmh;
	
	i=0;
	
	for(i=0; i<lcount; i++)
	{
		engine->drawImage(this->leftMiddleImage, x, y+bw+lsh+(i*lmh));		
		engine->drawImage(this->rightMiddleImage, endx, y+bw+lsh+(i*lmh));		
	}
	
	engine->drawImageSubImage(this->leftMiddleImage, x, y+bw+lsh+(i*lmh), bw, lmod+1);	
	engine->drawImageSubImage(this->rightMiddleImage, endx, y+bw+lsh+(i*lmh), bw, lmod+1);	
}

// From PKVectorControl

void PKBitmapBorderVectorPanel::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	PKVectorControl::selfPropertyChanged(prop, oldValue);
}
