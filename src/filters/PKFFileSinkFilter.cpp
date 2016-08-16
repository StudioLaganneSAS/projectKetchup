//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFFileSinkFilter.h"

//
// PKFFileSinkFilter
//

PKFReturnCode PKFFileSinkFilter_Create(IPKFFilter **filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = (IPKFFilter *) new PKFFileSinkFilter();

	if(*filter == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFFileSinkFilterProperties_Fill(PKFFilterProperties *prop)
{
	if(prop == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	prop->setFilterId("FILE_SINK");
	prop->setFilterName(L"File Writer (Local)");
	prop->setDescription(L"Writes local binary files.");
	prop->setFilterCategory(PKF_FILTER_CATEGORY_DATA);
	prop->setAuthors(L"Damien Chavarria");
	prop->setEmail("roy204@gmail.com");
	prop->setHomepage("http://www.projectketchup.com");
	prop->setVendor(L"TomatoSauce");
	prop->setVersion("1.0");

	return PKF_SUCCESS;
}

//
// PKFFileSinkFilter
//

std::string PKFFileSinkFilter::FILENAME_WSTRING_PROPERTY = "filename";

PKFFileSinkFilter::PKFFileSinkFilter()
{
	// Add filename property
	PKOBJECT_ADD_WSTRING_PROPERTY(filename, L"");

	// No output file yet
	this->output = NULL;

	// Create input pin type
	PKFDataMediaTypeDescriptor *desc = new PKFDataMediaTypeDescriptor();
	this->input->addMediaTypeDescriptor(desc);

	// Set autorun
	PKVariantBool b(true);
	this->set(IPKFFilter::AUTORUN_BOOL_PROPERTY, &b);
}

PKFFileSinkFilter::~PKFFileSinkFilter()
{
	if(this->output != NULL)
	{
		this->output->close();
		delete this->output;
	}
}

PKFReturnCode PKFFileSinkFilter::onConnected(IPKFConnector *connector, 
											 IPKFConnector *to)
{
	this->output = new PKFile();

	PKVariant *fn = this->get(FILENAME_WSTRING_PROPERTY);

	if(this->output->open(PKVALUE_WSTRING(fn), 
						  PKFILE_MODE_WRITE) == PKFILE_OK)
	{
		return PKF_SUCCESS;
	}

	return PKF_ERR_NOT_AVAILABLE;
}

PKFReturnCode PKFFileSinkFilter::onDisconnected(IPKFConnector *connector)
{
	if(this->output != NULL)
	{
		if(this->output->close() == PKFILE_OK)
		{
			delete this->output;
			this->output = NULL;

			return PKF_SUCCESS;
		}
	}

	return PKF_ERR_NOT_AVAILABLE;
}

PKFReturnCode PKFFileSinkFilter::push(PKFDataSample *sample)
{
	if(this->output == NULL)
	{
		return PKF_ERR_NOT_READY;
	}

	if(sample == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}
	
	PKFDataBuffer *buffer = sample->getBuffer();

	if(buffer == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	void *buff = NULL;
	buffer->lock(&buff);

	if(buff == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	uint32_t size = this->output->write(buff, buffer->getDataSize());

	buffer->unlock();

	if(size != buffer->getDataSize())
	{
		return PKF_ERR_NOT_AVAILABLE;
	}

	return PKF_SUCCESS;	
}

PKFReturnCode PKFFileSinkFilter::pushCustom(PKFCustomSample *sample)
{
	if(this->output == NULL)
	{
		return PKF_ERR_NOT_READY;
	}

	if(sample == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	PKVariant *type = sample->get(PKFCustomSample::TYPE_IDENTIFIER_PROPERTY);

	if(type == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(PKVALUE_STRING(type) == "SEEK")
	{
		PKVariant *method = sample->get("METHOD");
		PKVariant *amount = sample->get("OFFSET");

		if(PKVALUE_STRING(method) == "START")
		{
			uint64_t result = this->output->seek(PKVALUE_INT64(amount), 
											     PKFILE_SEEK_START);

			if(result == PKVALUE_INT64(amount))
			{
				return PKF_SUCCESS;
			}
		}
		else if(PKVALUE_STRING(method) == "CURRENT")
		{
			uint64_t current = this->output->tell();
			uint64_t result  = this->output->seek(PKVALUE_INT64(amount),
												  PKFILE_SEEK_CURRENT);

			if(result == current + PKVALUE_INT64(amount))
			{
				return PKF_SUCCESS;
			}
		}
		else if(PKVALUE_STRING(method) == "END")
		{
			uint64_t size   = this->output->getFileSize();

			uint64_t result = this->output->seek(PKVALUE_INT64(amount),
												 PKFILE_SEEK_START);

			if(result == size + PKVALUE_INT64(amount))
			{
				return PKF_SUCCESS;
			}
		}
	}

	return PKF_ERR_INVALID_PARAM;
}


