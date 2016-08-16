//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFImageSampleRequest.h"

//
// PKFImageSampleRequest
//

PKFImageSampleRequest::~PKFImageSampleRequest()
{
}

PKFMediaType PKFImageSampleRequest::getMediaType()
{
	return PKF_MEDIA_TYPE_IMAGE;
}
