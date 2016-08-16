//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFFiltersFactory.h"

#include "PKFFileSourceFilter.h"
#include "PKFFileSinkFilter.h"
#include "PKFJpegReaderFilter.h"
#include "PKFJpegWriterFilter.h"
#include "PKFBmpReaderFilter.h"
//#include "ImageReaderFilter.h"

//
// PKFFiltersFactory
//

PKFFiltersFactory::PKFFiltersFactory()
{
	// FileSourceFilter

	this->addFilter(PKFFileSourceFilter_Create,
					PKFFileSourceFilterProperties_Fill);

	// FileSinkFilter

	this->addFilter(PKFFileSinkFilter_Create,
					PKFFileSinkFilterProperties_Fill);

	// JpegReaderFilter

	this->addFilter(PKFJpegReaderFilter_Create,
					PKFJpegReaderFilterProperties_Fill);

	// BmpReaderFilter

	this->addFilter(PKFBmpReaderFilter_Create,
					PKFBmpReaderFilterProperties_Fill);

	// JpegWriterFilter

	this->addFilter(PKFJpegWriterFilter_Create,
					PKFJpegWriterFilterProperties_Fill);

	// ImageReaderFilter

//	this->addFilter(ImageReaderFilter_Create,
//					ImageReaderFilterProperties_Fill);
}

//
// Entry point
//

PKFReturnCode PKFFilters_FilterFactoryCreate(IPKFFilterFactory **factory)
{
	if(factory == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*factory = new PKFFiltersFactory();

	if(*factory == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	return PKF_SUCCESS;
}
