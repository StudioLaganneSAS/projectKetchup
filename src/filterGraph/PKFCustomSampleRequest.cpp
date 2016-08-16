//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFCustomSampleRequest.h"

std::string PKFCustomSampleRequest::TYPE_IDENTIFIER_PROPERTY = "typeIdentifier";

//
// PKFCustomSampleRequest
//

PKFCustomSampleRequest::PKFCustomSampleRequest(std::string id)
{
	PKOBJECT_ADD_STRING_PROPERTY(typeIdentifier, id);
}

PKFCustomSampleRequest::~PKFCustomSampleRequest()
{

}

PKFMediaType PKFCustomSampleRequest::getMediaType()
{	
	return PKF_MEDIA_TYPE_CUSTOM;
}	

