//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKImage.h"
#include "PKStr.h"
#include "PKFile.h"


#include "PKPNGLoader.h"
#include "PKJPGLoader.h"
#include "PKGIFLoader.h"

#ifdef LINUX
#include <stdlib.h>
#endif

//
// Utility
//

void PKImage::processURI(std::wstring    URI,
						 PKImageURIType *type,
						 std::wstring   *img)
{
	std::vector <std::wstring> fields;
	
	if(type == NULL || img == NULL)
	{
		return;
	}
	
	*type = PKIMAGE_URI_UNKNOWN;
	
	PKStr::explode(URI, L'|', &fields);
	
	if(fields.size() == 2)
	{
		*img = fields[1];
		
		if(PKStr::toLower(fields[0]) == L"res")
		{
			*type = PKIMAGE_URI_RESOURCE;
		}

		if(PKStr::toLower(fields[0]) == L"file")
		{
			*type = PKIMAGE_URI_FILENAME;
		}

		if(PKStr::toLower(fields[0]) == L"mem")
		{
			*type = PKIMAGE_URI_MPOINTER;
		}
	}
}


// 
// PKImage
//

PKImage::PKImage()
{
	this->pngLoader = NULL;
	this->jpgLoader = NULL;
    this->gifLoader = NULL;

	this->pixels    = NULL;
	this->width     = 0;
	this->height    = 0;
}

PKImage::PKImage(PKImage *copy)
{
	this->pngLoader = NULL;
	this->jpgLoader = NULL;
	this->gifLoader = NULL;

	this->pixels    = NULL;
	this->width     = 0;
	this->height    = 0;

    if(copy == NULL)
    {
        return;
    }
    
    int w = copy->getWidth();
    int h = copy->getHeight();
    
    void *src = copy->getPixels();
    
    if(src != NULL)
    {
        if(this->create(w, h))
        {
            void *dst = this->getPixels();
            
            if(dst != NULL)
            {
                memcpy(dst, src, w*h*4);
            }
        }
    }
}

PKImage::~PKImage()
{
	if(this->pngLoader != NULL)
	{
		delete this->pngLoader;
	}

	if(this->jpgLoader != NULL)
	{
		delete this->jpgLoader;
	}

	if(this->gifLoader != NULL)
	{
		delete this->gifLoader;
	}

    if(this->pixels != NULL)
	{
		free(this->pixels);
		this->pixels = NULL;
	}

	this->pngLoader = NULL;
	this->jpgLoader = NULL;
	this->gifLoader = NULL;
}

bool PKImage::create(unsigned int w,
					  unsigned int h)
{
	if(this->pixels    != NULL || 
	   this->pngLoader != NULL ||
	   this->jpgLoader != NULL ||
       this->gifLoader != NULL)
	{
		return false;
	}

	this->pixels = (char *) malloc(w*h*4);
	
	if(this->pixels != NULL)
	{
		memset(this->pixels, 0, w*h*4);
	}

	this->width    = w;
	this->height   = h;

	return true;
}

bool PKImage::destroy()
{
	if(this->pixels != NULL)
	{
		free(this->pixels);
		this->pixels = NULL;
	}

	this->width    = 0;
	this->height   = 0;

	return true;
}

bool PKImage::load(std::wstring filename)
{
    // See if the file looks good

    PKFile *file = new PKFile();
    uint64_t size = 0;

    if(file->open(filename, PKFILE_MODE_READ) == PKFILE_OK)
    {
        size = file->getFileSize();
        file->close();
    }
	
	delete file;

    if(size == 0)
    {
        // File is empty, don't risk
        // trying to load it...

        return false;
    }

    // Try PNG

	this->pngLoader = new PKPNGLoader();

	if(this->pngLoader == NULL)
	{
		return false;
	}

	if(this->pngLoader->load(filename) == true) 
	{
		return true;
	}

	delete this->pngLoader;
	this->pngLoader = NULL;

    // Try GIF

    this->gifLoader = new PKGIFLoader();

	if(this->gifLoader == NULL)
	{
		return false;
	}

	if(this->gifLoader->load(filename) == true) 
	{
		return true;
	}

	delete this->gifLoader;
	this->gifLoader = NULL;

    // Try JPG

    this->jpgLoader = new PKJPGLoader();

	if(this->jpgLoader == NULL)
	{
		return false;
	}

	if(this->jpgLoader->load(filename) == true) 
	{
		return true;
	}

	delete this->jpgLoader;
	this->jpgLoader = NULL;

    // Fail

	return false;
}

bool PKImage::load(char *buffer, int size)
{
    if(size == 0)
    {
        // File is empty, don't risk
        // trying to load it...

        return false;
    }
	
	// Try PNG

	this->pngLoader = new PKPNGLoader();

	if(this->pngLoader->load(buffer, size) == true) 
	{
		return true;
	}
	else
	{
		delete this->pngLoader;
		this->pngLoader = NULL;
	}

    // Try GIF

	this->gifLoader = new PKGIFLoader();

	if(this->gifLoader->load(buffer, size) == true) 
	{
		return true;
	}
	else
	{
		delete this->gifLoader;
		this->gifLoader = NULL;
	}

    // Try JPG

    this->jpgLoader = new PKJPGLoader();

	if(this->jpgLoader->load(buffer, size) == true) 
	{
		return true;
	}
	else
	{
		delete this->jpgLoader;
		this->jpgLoader = NULL;
	}

    // Fail

    return false;
}

bool PKImage::unload()
{	
	if(this->pngLoader)
	{
		this->pngLoader->unload();
		delete this->pngLoader;

		this->pngLoader = NULL;
	}

	if(this->jpgLoader)
	{
		this->jpgLoader->unload();
		delete this->jpgLoader;

		this->jpgLoader = NULL;
	}

	if(this->gifLoader)
	{
		this->gifLoader->unload();
		delete this->gifLoader;

		this->gifLoader = NULL;
	}

    return true;
}

char *PKImage::getPixels()
{
	if(this->pngLoader != NULL)
	{
		return this->pngLoader->getPixels();
	}
	else if(this->jpgLoader != NULL)
	{
		return this->jpgLoader->getPixels();
	}
	else if(this->gifLoader != NULL)
	{
		return this->gifLoader->getPixels();
	}
	else
	{
		return this->pixels;
	}
}

unsigned int PKImage::getWidth()
{
	if(this->pngLoader != NULL)
	{
		return this->pngLoader->getWidth();
	}
	else if(this->jpgLoader != NULL)
	{
		return this->jpgLoader->getWidth();
	}
	else if(this->gifLoader != NULL)
	{
		return this->gifLoader->getWidth();
	}
	else
	{
		return this->width;
	}
	
	return 0;
}

unsigned int PKImage::getHeight()
{
	if(this->pngLoader != NULL)
	{
		return this->pngLoader->getHeight();
	}
	else if(this->jpgLoader != NULL)
	{
		return this->jpgLoader->getHeight();
	}
	else if(this->gifLoader != NULL)
	{
		return this->gifLoader->getHeight();
	}
	else
	{
		return this->height;
	}
	
	return 0;
}

#ifdef MACOSX
CGImageRef PKImage::getCGImageRef()
{
	if(this->pngLoader != NULL)
	{
		return this->pngLoader->getCGImageRef();
	}
	else if(this->jpgLoader != NULL)
	{
		return this->jpgLoader->getCGImageRef();
	}
	else if(this->gifLoader != NULL)
	{
		return this->gifLoader->getCGImageRef();
	}
	
	return NULL;
}
#endif

void PKImage::copy(PKImage *source)
{
	if(source == NULL)
	{
		return;
	}

	unsigned int w = source->getWidth();
	unsigned int h = source->getHeight();

	if(w != this->getWidth()  ||
	   h != this->getHeight())
    {
		return;
	}

	char *src = source->getPixels();
	char *dst = this->getPixels();

	if(src == NULL ||
	   dst == NULL)
	{
		return;
	}

	memcpy(dst, src, w * h * 4);
}

void PKImage::copy(uint32_t  fromX,
                   uint32_t  fromY,
                   char     *toBuffer,
                   uint32_t  toX,
                   uint32_t  toY,
                   uint32_t  fwidth,
                   uint32_t  fheight)
{
	unsigned int w = this->getWidth();
	unsigned int h = this->getHeight();

    char *src = this->getPixels() + fromY*w*4 + fromX*4;
	char *dst = toBuffer  + toY*fwidth*4 + toX*4;

	if(src == NULL ||
	   dst == NULL)
	{
		return;
	}

    uint32_t stride = fwidth*4;
    uint32_t full_stride = w*4;
    uint32_t ww = pk_min(stride, full_stride);

    for(unsigned int i=0; i < h; i++)
    {
        memcpy(dst, src, ww);

        src += full_stride;
        dst += stride;
    }
}
