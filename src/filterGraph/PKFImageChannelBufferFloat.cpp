//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFImageChannelBufferFloat.h"

//
// PKFImageChannelBufferFloat
//

PKFImageChannelBufferFloat::PKFImageChannelBufferFloat(uint32_t width, uint32_t height)
{
	this->width  = width;
	this->height = height;

	this->pixels = NULL;
}

PKFImageChannelBufferFloat::~PKFImageChannelBufferFloat()
{
	this->free();
}

PKFReturnCode PKFImageChannelBufferFloat::alloc(PKFDataBufferType type)
{
	if(this->pixels != NULL)
	{
		return PKF_ERR_ALREADY_ALLOCATED;
	}

	this->pixels = new PKFDataBuffer(type);

	PKFReturnCode err = this->pixels->alloc(sizeof(float)*this->width*this->height);

	return err;
}

PKFReturnCode PKFImageChannelBufferFloat::free()
{
	if(this->pixels != NULL)
	{
		this->pixels->free();
		delete this->pixels;
		this->pixels = NULL;
	}

	return PKF_SUCCESS;
}

bool PKFImageChannelBufferFloat::isValid()
{
	return (this->pixels != NULL && this->pixels->isValid());
}

PKFImageDataType PKFImageChannelBufferFloat::getImageDataType()
{
	return PKF_IMAGE_DATA_TYPE_FLOAT;
}

uint32_t PKFImageChannelBufferFloat::getWidth()
{
	return this->width;
}

uint32_t PKFImageChannelBufferFloat::getHeight()
{
	return this->height;
}

float PKFImageChannelBufferFloat::getPixel(uint32_t x, uint32_t y)
{
	if(!this->isValid())
	{
		return 0;
	}	

	if(x >= this->width || y >= this->height)
	{
		return 0;
	}

	float *pix = NULL;
	this->pixels->lock((void **) &pix);

	if(pix == NULL)
	{
		return 0;
	}

	float pixel = pix[y*this->width + x];
	this->pixels->unlock();

	return pixel;
}

void PKFImageChannelBufferFloat::setPixel(uint32_t x, uint32_t y, float value)
{
	if(!this->isValid())
	{
		return;
	}	

	if(x >= this->width || y >= this->height)
	{
		return;
	}

	float *pix = NULL;
	this->pixels->lock((void **) &pix);

	if(pix == NULL)
	{
		return;
	}

	pix[y*this->width + x] = value;
	this->pixels->unlock();
}

PKFReturnCode PKFImageChannelBufferFloat::lock(float **pixels)
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

	*pixels = (float *) pix;
	return PKF_SUCCESS;
}

PKFReturnCode PKFImageChannelBufferFloat::unlock()
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

