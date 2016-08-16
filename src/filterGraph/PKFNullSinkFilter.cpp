//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFNullSinkFilter.h"

//
// PKFNullSinkFilter
//

PKFNullSinkFilter::PKFNullSinkFilter(PKFTransferType type)
:IPKFFilter()
{
	this->input = new PKFInputConnector(this, "NullInput", type);

	this->addConnector(this->input);

	// Data

	PKFDataMediaTypeDescriptor *dataType = new PKFDataMediaTypeDescriptor();
	this->input->addMediaTypeDescriptor(dataType);

	// Image

	PKFImageMediaTypeDescriptor *imageType = new PKFImageMediaTypeDescriptor();
	this->input->addMediaTypeDescriptor(imageType);

	// Custom

	PKFCustomMediaTypeDescriptor *customType = new PKFCustomMediaTypeDescriptor("");
	this->input->addMediaTypeDescriptor(customType);
}

PKFNullSinkFilter::~PKFNullSinkFilter()
{

}

PKFReturnCode PKFNullSinkFilter::process()
{
	return PKF_SUCCESS;
}

