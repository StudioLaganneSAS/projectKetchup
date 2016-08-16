//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFImageMediaTypeDescriptor.h"

//
// PKFImageMediaTypeDescriptor
//

PKFImageMediaTypeDescriptor::PKFImageMediaTypeDescriptor()
{

}

PKFImageMediaTypeDescriptor::~PKFImageMediaTypeDescriptor()
{
	for(unsigned int i=0; i < this->getNumberOfChannels(); i++)
	{
		PKFImageChannelDescriptor *channel = this->channelDescriptors[i];

		if(channel != NULL)
		{
			delete channel;
		}
	}	

	this->channelDescriptors.clear();
}

PKFMediaType PKFImageMediaTypeDescriptor::getMediaType()
{
	return PKF_MEDIA_TYPE_IMAGE;
}

uint32_t PKFImageMediaTypeDescriptor::getNumberOfChannels()
{
	return this->channelDescriptors.size();
}

PKFReturnCode PKFImageMediaTypeDescriptor::addChannelDescriptor(std::string channelDesc, 
																uint32_t width, 
																uint32_t height, 
																PKFImageDataType dataType)
{
	PKFImageChannelDescriptor *channel = new PKFImageChannelDescriptor(channelDesc, width, height, dataType);
	this->channelDescriptors.push_back(channel);

	return PKF_SUCCESS;
}

PKFReturnCode PKFImageMediaTypeDescriptor::getChannelDescriptor(uint32_t index, PKFImageChannelDescriptor ** channelDesc)
{
	if( (channelDesc == NULL) ||
		(index >= this->getNumberOfChannels()) )
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*channelDesc = this->channelDescriptors[index];

	return PKF_SUCCESS;
}

PKFReturnCode PKFImageMediaTypeDescriptor::getChannelDescriptor(std::string channelDescriptorName, 
																PKFImageChannelDescriptor **channelDesc)
{
	if(channelDesc == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	bool found = false;

	for(unsigned int i=0; i < this->channelDescriptors.size(); i++)
	{
		if(this->channelDescriptors[i]->getDescription() == channelDescriptorName)
		{
			*channelDesc = this->channelDescriptors[i];
			found = true;
			break;
		}
	}

	if(found)
	{
		return PKF_SUCCESS;
	}

	return PKF_ERR_NOT_AVAILABLE;
}

