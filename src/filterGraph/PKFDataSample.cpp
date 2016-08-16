//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFDataSample.h"

//
// PKFDataSample
//

PKFDataSample::PKFDataSample()
{
	this->buffer = NULL;
}

PKFDataSample::~PKFDataSample()
{

}

PKFMediaType PKFDataSample::getMediaType()
{
	return PKF_MEDIA_TYPE_DATA;
}

PKFDataBuffer *PKFDataSample::getBuffer()
{
	return this->buffer;
}

void PKFDataSample::setBuffer(PKFDataBuffer *buffer)
{
	this->buffer = buffer;
}
