//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
// 

#include "PKFCustomSample.h"

//
// PKFCustomSample
//

std::string PKFCustomSample::TYPE_IDENTIFIER_PROPERTY = "typeIdentifier";

//
//
//

PKFCustomSample::PKFCustomSample(std::string id)
{
	PKOBJECT_ADD_STRING_PROPERTY(typeIdentifier, id);
}

PKFCustomSample::~PKFCustomSample()
{

}

PKFMediaType PKFCustomSample::getMediaType()
{
	return PKF_MEDIA_TYPE_CUSTOM;
}
