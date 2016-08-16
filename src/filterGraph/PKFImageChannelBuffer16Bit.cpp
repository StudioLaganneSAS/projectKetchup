//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFImageChannelBuffer16Bit.h"

//
// PKFImageChannelBuffer16Bit
//

PKFImageChannelBuffer16Bit::PKFImageChannelBuffer16Bit(uint32_t width, uint32_t height)
{
	this->width  = width;
	this->height = height;

	this->pixels = NULL;
}

PKFImageChannelBuffer16Bit::~PKFImageChannelBuffer16Bit()
{
	this->free();
}

PKFReturnCode PKFImageChannelBuffer16Bit::alloc(PKFDataBufferType type)
{
	if(this->pixels != NULL)
	{
		return PKF_ERR_ALREADY_ALLOCATED;
	}

	this->pixels = new PKFDataBuffer(type);

	PKFReturnCode err = this->pixels->alloc(sizeof(uint16_t)*this->width*this->height);

	return err;
}

PKFReturnCode PKFImageChannelBuffer16Bit::free()
{
	if(this->pixels != NULL)
	{
		this->pixels->free();
		delete this->pixels;
		this->pixels = NULL;
	}

	return PKF_SUCCESS;
}

bool PKFImageChannelBuffer16Bit::isValid()
{
	return (this->pixels != NULL && this->pixels->isValid());
}

PKFImageDataType PKFImageChannelBuffer16Bit::getImageDataType()
{
	return PKF_IMAGE_DATA_TYPE_16_BITS;
}

uint32_t PKFImageChannelBuffer16Bit::getWidth()
{
	return this->width;
}

uint32_t PKFImageChannelBuffer16Bit::getHeight()
{
	return this->height;
}

uint16_t PKFImageChannelBuffer16Bit::getPixel(uint32_t x, uint32_t y)
{
	if(!this->isValid())
	{
		return 0;
	}	

	if(x >= this->width || y >= this->height)
	{
		return 0;
	}

	uint16_t *pix = NULL;
	this->pixels->lock((void **) &pix);

	if(pix == NULL)
	{
		return 0;
	}

	uint16_t pixel = pix[y*this->width + x];
	this->pixels->unlock();

	return pixel;
}

void PKFImageChannelBuffer16Bit::setPixel(uint32_t x, uint32_t y, uint16_t value)
{
	if(!this->isValid())
	{
		return;
	}	

	if(x >= this->width || y >= this->height)
	{
		return;
	}

	uint16_t *pix = NULL;
	this->pixels->lock((void **) &pix);

	if(pix == NULL)
	{
		return;
	}

	pix[y*this->width + x] = value;
	this->pixels->unlock();
}

PKFReturnCode PKFImageChannelBuffer16Bit::lock(uint16_t **pixels)
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

	*pixels = (uint16_t *) pix;
	return PKF_SUCCESS;
}

PKFReturnCode PKFImageChannelBuffer16Bit::unlock()
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

