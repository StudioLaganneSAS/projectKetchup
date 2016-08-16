//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFImageChannel.h"

//
// PKFImageChannel Descs
//

std::string PKFImageChannel::PKF_TYPE_RGB_RED      = "RGB_RED";;
std::string PKFImageChannel::PKF_TYPE_RGB_GREEN    = "RGB_GREEN";;
std::string PKFImageChannel::PKF_TYPE_RGB_BLUE     = "RGB_BLUE";
std::string PKFImageChannel::PKF_TYPE_GRAYSCALE    = "GRAYSCALE";
std::string PKFImageChannel::PKF_TYPE_ALPHA        = "ALPHA";
std::string PKFImageChannel::PKF_TYPE_CYMK_CYAN    = "CYMK_CYAN";
std::string PKFImageChannel::PKF_TYPE_CYMK_YELLOW  = "CYMK_YELLOW";
std::string PKFImageChannel::PKF_TYPE_CYMK_MAGENTA = "CYMK_MAGENTA";
std::string PKFImageChannel::PKF_TYPE_CYMK_KHOL    = "CYMK_KHOL";

//
// PKFImageChannel
//


PKFImageChannel::PKFImageChannel(std::string typeDesc)
{
	this->buffer   = NULL;
	this->typeDesc = typeDesc;
}

PKFImageChannel::~PKFImageChannel()
{
	this->free();
}

PKFReturnCode PKFImageChannel::alloc(uint32_t width, 
									 uint32_t height, 
									 PKFImageDataType  dataType,
									 PKFDataBufferType bufferType)
{
	return PKFImageChannelBufferFactory::createImageChannelBuffer(width, 
																  height, 
																  dataType, 
																  &this->buffer,
																  bufferType);
}

PKFReturnCode PKFImageChannel::free()
{
	if(this->buffer == NULL)
	{
		return PKF_ERR_NOT_ALLOCATED;
	}

	PKFReturnCode result = this->buffer->free();

	if(result == PKF_SUCCESS)
	{
		delete this->buffer;
		this->buffer = NULL;
	}	

	return result;
}

bool PKFImageChannel::isValid()
{
	return (this->buffer != NULL);
}

std::string PKFImageChannel::getTypeDesc()
{
	return this->typeDesc;
}

PKFReturnCode PKFImageChannel::getChannelBuffer(IPKFImageChannelBuffer **buffer)
{
	if(buffer == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}	

	if(this->buffer == NULL)
	{
		return PKF_ERR_NOT_ALLOCATED;
	}	

	*buffer = this->buffer;

	return PKF_SUCCESS;
}

