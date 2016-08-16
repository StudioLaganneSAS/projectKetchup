//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFDataMediaTypeDescriptor.h"

//
// PKFDataMediaTypeDescriptor
//

PKFDataMediaTypeDescriptor::PKFDataMediaTypeDescriptor()
{
	this->size = 0;
}

PKFDataMediaTypeDescriptor::~PKFDataMediaTypeDescriptor()
{

}

PKFMediaType PKFDataMediaTypeDescriptor::getMediaType()
{
	return PKF_MEDIA_TYPE_DATA;
}

uint32_t PKFDataMediaTypeDescriptor::getDataSize()
{
	return this->size;
}

void PKFDataMediaTypeDescriptor::setDataSize(uint32_t size)
{
	this->size = size;
}
