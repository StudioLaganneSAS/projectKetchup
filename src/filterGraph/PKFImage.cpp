//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFImage.h"

//
// PKFImage
//

PKFImage::PKFImage(std::string name)
{
	this->name = name;
}

PKFImage::~PKFImage()
{
	for(unsigned int i=0; i < this->channels.size(); i++)
	{
		PKFImageChannel *channel = this->channels[i];
		
		if(channel != NULL)
		{
			delete channel;
		}
	}

	this->channels.clear();
}

std::string PKFImage::getName()
{
	return this->name;
}

PKFReturnCode PKFImage::addChannel(std::string typeDesc, 
								   uint32_t width, 
								   uint32_t height, 
								   PKFImageDataType type,
								   PKFDataBufferType bufferType)
{
	PKFImageChannel *channel = new PKFImageChannel(typeDesc);

	if(channel == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	PKFReturnCode result = channel->alloc(width, height, type, bufferType);

	if(result == PKF_SUCCESS)
	{
		this->channels.push_back(channel);
	}

	return result;
}

uint8_t PKFImage::getNumberOfChannels()
{
	return this->channels.size();
}

PKFReturnCode PKFImage::getChannel(std::string channelDesc, 
								   PKFImageChannel **channel)
{
	if(channel == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	bool found = false;

	for(unsigned int i=0; i < this->channels.size(); i++)
	{
		if(this->channels[i]->getTypeDesc() == channelDesc)
		{
			*channel = this->channels[i];
			found = true;
			break;
		}
	}

	if(!found)
	{
		return PKF_ERR_NOT_AVAILABLE;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFImage::getChannel(uint8_t index, 
								   PKFImageChannel **channel)
{
	if(index >= this->getNumberOfChannels() || channel == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*channel = this->channels[index];

	if(*channel == NULL)
	{
		return PKF_ERR_NOT_ALLOCATED;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFImage::deleteChannel(uint8_t index)
{
	if(index >= this->getNumberOfChannels())
	{
		return PKF_ERR_INVALID_PARAM;
	}

	PKFImageChannel *channel = this->channels[index];

	if(channel != NULL)
	{	
		delete channel;
	}

	std::vector <PKFImageChannel *> new_channels;

	for(int i=0; i < this->getNumberOfChannels(); i++)
	{
		if(i != index)
		{
			new_channels.push_back(this->channels[i]);
		}
	}

	this->channels = new_channels;

	return PKF_SUCCESS;
}

uint32_t PKFImage::getWidth()
{
	if(this->getNumberOfChannels() > 0)
	{
		PKFImageChannel *channel = NULL;
		
		PKFReturnCode result = this->getChannel(0, &channel);

		if(result == PKF_SUCCESS)
		{
			IPKFImageChannelBuffer *buffer = NULL;

			result = channel->getChannelBuffer(&buffer);

			if(result == PKF_SUCCESS)
			{
				return buffer->getWidth();
			}
		}
	}

	return 0;
}

uint32_t PKFImage::getHeight()
{
	if(this->getNumberOfChannels() > 0)
	{
		PKFImageChannel *channel = NULL;
		
		PKFReturnCode result = this->getChannel(0, &channel);

		if(result == PKF_SUCCESS)
		{
			IPKFImageChannelBuffer *buffer = NULL;

			result = channel->getChannelBuffer(&buffer);

			if(result == PKF_SUCCESS)
			{
				return buffer->getHeight();
			}
		}
	}

	return 0;
}

PKFImageDataType PKFImage::getImageDataType()
{
	if(this->getNumberOfChannels() > 0)
	{
		PKFImageChannel *channel = NULL;
		
		PKFReturnCode result = this->getChannel(0, &channel);

		if(result == PKF_SUCCESS)
		{
			IPKFImageChannelBuffer *buffer = NULL;

			result = channel->getChannelBuffer(&buffer);

			if(result == PKF_SUCCESS)
			{
				return buffer->getImageDataType();
			}
		}
	}

	return PKF_IMAGE_DATA_TYPE_ANY;
}
