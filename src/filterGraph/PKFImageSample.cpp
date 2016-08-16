//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
// 

#include "PKFImageSample.h"

//
// PKFImageSample
//

PKFImageSample::PKFImageSample()
:IPKFSample()
{
	this->image = NULL;
}

PKFImageSample::~PKFImageSample()
{

}

PKFMediaType PKFImageSample::getMediaType()
{
	return PKF_MEDIA_TYPE_IMAGE;
}

PKFImage *PKFImageSample::getImage()
{
	return this->image;
}

void PKFImageSample::setImage(PKFImage *image)
{
	this->image = image;
}

