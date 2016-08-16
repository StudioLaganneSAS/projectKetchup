//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKImageVectorControl.h"
#include "PKVectorEngine.h"
#include "PKVectorControlRoot.h"
#include "PKImageLoader.h"

//
// PROPERTIES
//

std::string PKImageVectorControl::IMAGE_WSTRING_PROPERTY = "image";

//
// PKImageVectorControl
//

PKImageVectorControl::PKImageVectorControl()
{
	PKOBJECT_ADD_WSTRING_PROPERTY(image, L"");

    this->built = false;
    this->image = NULL;
}

PKImageVectorControl::~PKImageVectorControl()
{
}

void PKImageVectorControl::build()
{
    if(this->built)
    {
        return;
    }

	PKVectorControl::build();
	this->built = true;

    // Load image

    PKVariant *img = this->get(PKImageVectorControl::IMAGE_WSTRING_PROPERTY);
	std::wstring URI = PKVALUE_WSTRING(img);
	
	PKImageURIType type;
	std::wstring   imageStr;
	
	PKImage::processURI(URI, &type, &imageStr);

	if(type == PKIMAGE_URI_RESOURCE)
	{
    	this->image = PKImageLoader::loadBitmapFromResource(imageStr);
	}

	if(type == PKIMAGE_URI_FILENAME)
	{
    	this->image = PKImageLoader::loadBitmapFromFilename(imageStr);
	}
}

void PKImageVectorControl::drawVector(PKVectorEngine *engine,
		                              int32_t x, int32_t y)
{
	if(!this->built)
	{
		return;
	}
	
	if(engine == NULL)
	{
		return;
	}

	int w = this->getW();
	int h = this->getH();

#ifdef WIN32

	PKObject  *parent  = this->getParent();
	PKControl *parentC = dynamic_cast<PKControl *>(parent);

	if(parentC)
	{
		HBRUSH brush = parentC->getBackgroundBrush();
		engine->fillRect(x, y, w, h, brush);
	}

#endif	

    engine->drawImageStretched(this->image, x, y, w, h);
}

