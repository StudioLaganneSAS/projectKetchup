//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFCustomMediaTypeDescriptor.h"

//
// PKFCustomMediaTypeDescriptor
//

std::string PKFCustomMediaTypeDescriptor::TYPE_IDENTIFIER_PROPERTY = "typeIdentifier";

PKFCustomMediaTypeDescriptor::PKFCustomMediaTypeDescriptor(std::string descriptor)
{
	PKOBJECT_ADD_STRING_PROPERTY(typeIdentifier, descriptor);
}	

PKFCustomMediaTypeDescriptor::~PKFCustomMediaTypeDescriptor()
{

}

PKFMediaType PKFCustomMediaTypeDescriptor::getMediaType()
{
	return PKF_MEDIA_TYPE_CUSTOM;
}
