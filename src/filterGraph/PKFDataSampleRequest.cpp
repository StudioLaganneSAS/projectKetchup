//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFDataSampleRequest.h"

//
// PKFDataSampleRequest
//


PKFDataSampleRequest::PKFDataSampleRequest(uint32_t size)
{	
	this->size   = size;
}

PKFDataSampleRequest::~PKFDataSampleRequest()
{

}

PKFMediaType PKFDataSampleRequest::getMediaType()
{
	return PKF_MEDIA_TYPE_DATA;
}

uint32_t PKFDataSampleRequest::getSize()
{
	return this->size;
}

