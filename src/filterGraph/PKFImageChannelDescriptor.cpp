//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
// 

#include "PKFImageChannelDescriptor.h"

//
// PKFImageChannelDescriptor
//

PKFImageChannelDescriptor::PKFImageChannelDescriptor(std::string desc, 
													 uint32_t width, 
													 uint32_t height, 
													 PKFImageDataType type)
{
	this->description   = desc;
	this->width         = width;
	this->height        = height;
	this->imageDataType = type;
}

PKFImageChannelDescriptor::~PKFImageChannelDescriptor()
{
	
}

std::string PKFImageChannelDescriptor::getDescription()
{
	return this->description;
}

uint32_t PKFImageChannelDescriptor::getWidth()
{
	return this->width;
}

uint32_t PKFImageChannelDescriptor::getHeight()
{
	return this->height;
}

PKFImageDataType PKFImageChannelDescriptor::getImageDataType()
{
	return this->imageDataType;
}

