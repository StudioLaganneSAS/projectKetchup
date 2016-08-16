//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFImageChannelBuffer8Bit.h"

//
// PKFImageChannelBuffer8Bit
//

PKFImageChannelBuffer8Bit::PKFImageChannelBuffer8Bit(uint32_t width, uint32_t height)
{
	this->width  = width;
	this->height = height;

	this->pixels = NULL;
}

PKFImageChannelBuffer8Bit::~PKFImageChannelBuffer8Bit()
{
	this->free();
}

PKFReturnCode PKFImageChannelBuffer8Bit::alloc(PKFDataBufferType type)
{
	PKFReturnCode err = PKF_SUCCESS;

	if(this->pixels != NULL)
	{
		return PKF_ERR_ALREADY_ALLOCATED;
	}

	this->pixels = new PKFDataBuffer(type);
	err = this->pixels->alloc(sizeof(uint8_t)*this->width*this->height);

	return err;
}

PKFReturnCode PKFImageChannelBuffer8Bit::free()
{
	if(this->pixels != NULL)
	{
		this->pixels->free();
		delete this->pixels;
		this->pixels = NULL;
	}

	return PKF_SUCCESS;
}

bool PKFImageChannelBuffer8Bit::isValid()
{
	return (this->pixels != NULL && this->pixels->isValid());
}

PKFImageDataType PKFImageChannelBuffer8Bit::getImageDataType()
{
	return PKF_IMAGE_DATA_TYPE_8_BITS;
}

uint32_t PKFImageChannelBuffer8Bit::getWidth()
{
	return this->width;
}

uint32_t PKFImageChannelBuffer8Bit::getHeight()
{
	return this->height;
}

uint8_t PKFImageChannelBuffer8Bit::getPixel(uint32_t x, uint32_t y)
{
	if(!this->isValid())
	{
		return 0;
	}	

	if(x >= this->width || y >= this->height)
	{
		return 0;
	}

	uint8_t *pix = NULL;
	this->pixels->lock((void **) &pix);

	if(pix == NULL)
	{
		return 0;
	}

	uint8_t pixel = pix[y*this->width + x];
	this->pixels->unlock();

	return pixel;
}

void PKFImageChannelBuffer8Bit::setPixel(uint32_t x, uint32_t y, uint8_t value)
{
	if(!this->isValid())
	{
		return;
	}	

	if(x >= this->width || y >= this->height)
	{
		return;
	}

	uint8_t *pix = NULL;
	this->pixels->lock((void **) &pix);

	if(pix == NULL)
	{
		return;
	}

	pix[y*this->width + x] = value;
	this->pixels->unlock();
}

PKFReturnCode PKFImageChannelBuffer8Bit::lock(uint8_t **pixels)
{
	if(this->pixels == NULL)
	{
		return PKF_ERR_NOT_READY;
	}

	void *pix = NULL;

	this->pixels->lock(&pix);

	if(pix == NULL)
	{
		return PKF_ERR_NOT_READY;
	}

	*pixels = (uint8_t *) pix;
	return PKF_SUCCESS;
}

PKFReturnCode PKFImageChannelBuffer8Bit::unlock()
{
	if(this->pixels == NULL)
	{
		return PKF_ERR_NOT_READY;
	}

	bool result = this->pixels->unlock();

	if(result)
	{
		return PKF_SUCCESS;
	}

	return PKF_ERR_NOT_LOCKED;
}

