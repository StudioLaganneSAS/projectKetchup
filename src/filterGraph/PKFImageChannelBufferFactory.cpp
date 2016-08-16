//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFImageChannelBufferFactory.h"

//
// PKFImageChannelBufferFactory
//


PKFReturnCode PKFImageChannelBufferFactory::createImageChannelBuffer(uint32_t       width, 
																     uint32_t       height, 
																     PKFImageDataType  type, 
																     IPKFImageChannelBuffer **buffer,
																     PKFDataBufferType btype)
{
	IPKFImageChannelBuffer *b;
	
	if(width  == 0 || height == 0)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	switch(type)
	{
	case PKF_IMAGE_DATA_TYPE_8_BITS:
		{
			b = new PKFImageChannelBuffer8Bit(width, height);
		}
		break;

	case PKF_IMAGE_DATA_TYPE_16_BITS:
		{
			b = new PKFImageChannelBuffer16Bit(width, height);
		}
		break;

	case PKF_IMAGE_DATA_TYPE_FLOAT:
		{
			b = new PKFImageChannelBufferFloat(width, height);
		}
		break;

	default:
		{
			return PKF_ERR_INVALID_PARAM;
		}
	}

	if(b == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	PKFReturnCode result = b->alloc(btype);

	if(result != PKF_SUCCESS)
	{
		*buffer = NULL;
		return result;
	}

	*buffer = b;

	return PKF_SUCCESS;
}
