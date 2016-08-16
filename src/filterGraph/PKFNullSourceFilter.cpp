//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFNullSourceFilter.h"

//
// PKFNullSourceFilter
//

PKFNullSourceFilter::PKFNullSourceFilter(PKFTransferType type)
:IPKFFilter()
{
	this->output = new PKFOutputConnector(this, "NullOutput", type);

	this->addConnector(this->output);

	// Data

	PKFDataMediaTypeDescriptor *dataType = new PKFDataMediaTypeDescriptor();
	this->output->addMediaTypeDescriptor(dataType);

	// Image

	PKFImageMediaTypeDescriptor *imageType = new PKFImageMediaTypeDescriptor();
	this->output->addMediaTypeDescriptor(imageType);

	// Custom

	PKFCustomMediaTypeDescriptor *customType = new PKFCustomMediaTypeDescriptor("");
	this->output->addMediaTypeDescriptor(customType);
}

PKFNullSourceFilter::~PKFNullSourceFilter()
{

}

PKFReturnCode PKFNullSourceFilter::process()
{
	return PKF_SUCCESS;
}

