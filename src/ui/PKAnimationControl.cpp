//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKAnimationControl.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKTime.h"

//
// PROPERTIES
//

std::string PKAnimationControl::FRAMERATE_DOUBLE_PROPERTY = "frameRate";
std::string PKAnimationControl::IMAGE_WSTRING_PROPERTY    = "image";
std::string PKAnimationControl::NUMFRAMES_UINT32_PROPERTY = "numFrames";

//
// PKAnimationControl
//

PKAnimationControl::PKAnimationControl()
{
	PKOBJECT_ADD_DOUBLE_PROPERTY(frameRate, 20.0);
	PKOBJECT_ADD_WSTRING_PROPERTY(image, L"IMG_NOT_SET");
	PKOBJECT_ADD_UINT32_PROPERTY(numFrames, 0);	

	this->width     = 0;
	this->height    = 0;
	this->bitmap    = NULL;

	this->built = false;

    this->currentFrame = 0;

    this->thread = new PKThread(this);
}

PKAnimationControl::~PKAnimationControl()
{
    this->stop();
    delete this->thread;

    if(this->bitmap != NULL)
    {
        delete this->bitmap;
    }
}

void PKAnimationControl::build()
{
	PKObject *parent = this->getParent();

	if(parent == NULL)
	{
		return;
	}

	PKView *parentView = dynamic_cast<PKView*>(parent);

	if(parentView == NULL)
	{
		return;
	}

    if(this->built)
    {
        return;
    }

    // Load the image
	
	if(this->bitmap != NULL)
	{
		delete this->bitmap;
		this->bitmap = NULL;
	}

    PKVariant *img = this->get(PKAnimationControl::IMAGE_WSTRING_PROPERTY);
	std::wstring URI = PKVALUE_WSTRING(img);
	
	PKImageURIType type;
	std::wstring   image;
	
	PKImage::processURI(URI, &type, &image);

	if(type == PKIMAGE_URI_RESOURCE)
	{
    	this->bitmap = PKImageLoader::loadBitmapFromResource(image);
	}

	if(type == PKIMAGE_URI_FILENAME)
	{
    	this->bitmap = PKImageLoader::loadBitmapFromFilename(image);
	}

    if(this->bitmap == NULL)
    {
        return;
    }

#ifdef WIN32

	// alpha premultiply

    uint8_t *pdata = (uint8_t *) this->bitmap->getPixels();

	for(unsigned int j=0; j <  this->bitmap->getHeight(); j++)
	{
		for(unsigned int i=0; i < this->bitmap->getWidth(); i++)
		{
			if(pdata[3] != 255)
			{
				pdata[0] = (uint8_t) ((int)pdata[0]*(int)pdata[3]/255);
				pdata[1] = (uint8_t) ((int)pdata[1]*(int)pdata[3]/255);
				pdata[2] = (uint8_t) ((int)pdata[2]*(int)pdata[3]/255);
			}
		
			pdata +=4;
		}
	}

#endif

    // Set our size

    PKVariant *num = this->get(PKAnimationControl::NUMFRAMES_UINT32_PROPERTY);
    uint32_t numFrames = PKVALUE_UINT32(num);

    this->currentFrame = 0;

    if(numFrames == 0)
    {
        return;
    }

    this->width  = this->bitmap->getWidth() / numFrames;
    this->height = this->bitmap->getHeight();

    this->resize(width, height);

    // Then create the control

	PKDoubleBufferedControl::build();

#ifdef LINUX
    
    if(this->widget != NULL)
    {
        gtk_widget_set_size_request(this->widget, this->width, this->height);
    }

#endif // LINUX

    this->built = true;
}

// From PKObject
void PKAnimationControl::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    PKDoubleBufferedControl::selfPropertyChanged(prop, oldValue);
}

void PKAnimationControl::start()
{
    this->thread->start();
}

void PKAnimationControl::stop()
{
    this->thread->stop();
}

// PKThread
void PKAnimationControl::runThread(PKThread *thread)
{
    PKVariant *fr = this->get(PKAnimationControl::FRAMERATE_DOUBLE_PROPERTY);
    double frameRate = PKVALUE_DOUBLE(fr);

    PKVariant *nf = this->get(PKAnimationControl::NUMFRAMES_UINT32_PROPERTY);
    uint32_t numFrames = PKVALUE_UINT32(nf);

    if(frameRate <= 0)
    {
        return;
    }

    if(numFrames <= 0)
    {
        return;
    }

    uint32_t time = (uint32_t) ((double)1000.0 / frameRate);

    while(!thread->getExitFlag())
    {
        PKTime::sleep(time);

        if(this->currentFrame < (numFrames - 1))
        {
            this->currentFrame++;
        }
        else
        {
            this->currentFrame = 0;
        }

#ifdef LINUX
        if(!GTK_IS_WIDGET(this->widget))
        {
            continue;
        }
#endif

        this->invalidate();
    }
}

uint32_t PKAnimationControl::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

	return this->width;
}

uint32_t PKAnimationControl::getMinimumHeight()
{
    if(!this->built)
    {
        this->build();
    }

	return this->height;
}

void PKAnimationControl::draw32bpp(uint8_t *buffer)
{
	// Draw current frame

    if(this->bitmap == NULL)
    {
        return;
    }

    uint32_t x = this->currentFrame * this->width;

    this->bitmap->copy(x, 0, (char*)buffer, 
                       0, 0, 
                       this->width, 
                       this->height);
}


