//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFFileSourceFilter.h"

//
// Entry Points
//

PKFReturnCode PKFFileSourceFilter_Create(IPKFFilter **filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = (IPKFFilter *) new PKFFileSourceFilter();

	if(*filter == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFFileSourceFilterProperties_Fill(PKFFilterProperties *prop)
{
	if(prop == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	prop->setFilterId("FILE_SOURCE");
	prop->setFilterName(L"File Reader (Local)");
	prop->setDescription(L"Reads local binary files.");
	prop->setFilterCategory(PKF_FILTER_CATEGORY_DATA);
	prop->setAuthors(L"Damien Chavarria");
	prop->setEmail("roy204@gmail.com");
	prop->setHomepage("http://www.projectketchup.com");
	prop->setVendor(L"TomatoSauce");
	prop->setVersion("1.0");

	return PKF_SUCCESS;
}

//
// PKFFileSourceOutput
//

PKFFileSourceOutput::PKFFileSourceOutput(IPKFFilter *parent, std::string name)
:PKFOutputConnector(parent, name, PKF_TRANSFER_TYPE_PULL)
{
	this->input = NULL;
}

PKFFileSourceOutput::~PKFFileSourceOutput()
{
	
}

void PKFFileSourceOutput::setFile(PKFile *file)
{
	this->input = file;
}

PKFReturnCode PKFFileSourceOutput::sampleRequested(IPKFSampleRequest *request, uint32_t *sampleId)
{
	if(this->input == NULL)
	{
		return PKF_ERR_NOT_READY;
	}

	if(request == NULL || sampleId == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(request->getMediaType() == PKF_MEDIA_TYPE_DATA)
	{
		PKFDataSampleRequest *dsr = (PKFDataSampleRequest *) request;

		if(dsr->getSize() == 0)
		{
			return PKF_ERR_INVALID_PARAM;
		}

		PKFDataMediaTypeDescriptor *desc = new PKFDataMediaTypeDescriptor();
		desc->setDataSize(dsr->getSize());

		PKFReturnCode result = this->getSample(desc, sampleId);

		delete desc;

		if(result != PKF_SUCCESS)
		{
			return PKF_ERR_OUT_OF_MEMORY;
		}

		PKFSampleAutoLock lock(*sampleId);

		PKFDataSample *sample = (PKFDataSample *) lock.getSample();

		if(sample == NULL || sample->getBuffer() == NULL)
		{
			*sampleId = 0;
			return PKF_ERR_NOT_AVAILABLE;
		}

		void *buffer = NULL;
		
		sample->getBuffer()->lock(&buffer);

		if(buffer == NULL)
		{
			*sampleId = 0;

			return PKF_ERR_NOT_AVAILABLE;
		}

		uint32_t read = this->input->read(buffer, dsr->getSize());

		sample->getBuffer()->unlock();

		if(read > 0)
		{
			sample->getBuffer()->setDataSize(read);
			return PKF_SUCCESS;
		}

		*sampleId = 0;

		return PKF_ERR_NOT_AVAILABLE;
	}
	else if(request->getMediaType() == PKF_MEDIA_TYPE_CUSTOM)
	{
		PKFCustomSampleRequest *csr = (PKFCustomSampleRequest *) request;

		PKVariant *type = csr->get(PKFCustomSampleRequest::TYPE_IDENTIFIER_PROPERTY);

		if(type == NULL)
		{
			return PKF_ERR_INVALID_PARAM;
		}

		if(PKVALUE_STRING(type) == "SEEK")
		{
			PKVariant *method = csr->get("METHOD");
			PKVariant *amount = csr->get("OFFSET");
				
			if(PKVALUE_STRING(method) == "START")
			{
				uint64_t result = this->input->seek(PKVALUE_INT64(amount), 
												    PKFILE_SEEK_START);

				if(result == PKVALUE_INT64(amount))
				{
					return PKF_SUCCESS;
				}
			}
			else if(PKVALUE_STRING(method) == "CURRENT")
			{
				uint64_t current = this->input->tell();
				uint64_t result  = this->input->seek(PKVALUE_INT64(amount), 
													 PKFILE_SEEK_CURRENT);

				if(result == current + PKVALUE_INT64(amount))
				{
					return PKF_SUCCESS;
				}
			}
			else if(PKVALUE_STRING(method) == "END")
			{
				uint64_t size   = this->input->getFileSize();
				uint64_t result = this->input->seek(PKVALUE_INT64(amount), 
													PKFILE_SEEK_START);

				if(result == size + PKVALUE_INT64(amount))
				{
					return PKF_SUCCESS;
				}
			}
		}
	}

	return PKF_ERR_NOT_AVAILABLE;
}

//
// PKFFileSourceFilter
//

std::string PKFFileSourceFilter::FILENAME_WSTRING_PROPERTY = "filename";

PKFFileSourceFilter::PKFFileSourceFilter()
:IPKFFilter()
{
	// Filename property

	PKOBJECT_ADD_WSTRING_PROPERTY(filename, L"");

	// Output connector

	this->output = new PKFFileSourceOutput(this, "FileSourceOutput");
	this->addConnector(this->output);

	PKFDataMediaTypeDescriptor *desc = new PKFDataMediaTypeDescriptor();
	this->output->addMediaTypeDescriptor(desc);

	// Input file

	this->input = new PKFile();
}

PKFFileSourceFilter::~PKFFileSourceFilter()
{
	delete this->input;
}

PKFReturnCode PKFFileSourceFilter::onConnected(IPKFConnector *connector, 
											   IPKFConnector *to)
{
	PKVariant *fn = this->get(FILENAME_WSTRING_PROPERTY);

	PKFileError result = this->input->open(PKVALUE_WSTRING(fn), 
									       PKFILE_MODE_READ);

	if(result == PKFILE_OK)
	{
		this->output->setFile(this->input);
		return PKF_SUCCESS;
	}

	return PKF_ERR_OBJECT_NOT_VALID;
}

PKFReturnCode PKFFileSourceFilter::onDisconnected(IPKFConnector *connector)
{
	this->input->close();
	this->output->setFile(NULL);

	return PKF_SUCCESS;
}

PKFReturnCode PKFFileSourceFilter::process()
{
	// Nothing to do
	return PKF_SUCCESS;
}


