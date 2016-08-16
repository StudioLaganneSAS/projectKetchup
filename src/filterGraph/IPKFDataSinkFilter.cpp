//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "IPKFDataSinkFilter.h"

//
// IPKFDataSinkFilter
//

IPKFDataSinkFilter::IPKFDataSinkFilter()
:IPKFFilter()
{
	this->input  = new PKFInputConnector(this, "DataSinkInput", PKF_TRANSFER_TYPE_PUSH);
	this->addConnector(this->input);
}

IPKFDataSinkFilter::~IPKFDataSinkFilter()
{

}

PKFReturnCode IPKFDataSinkFilter::pushCustom(PKFCustomSample *sample)
{
	return PKF_ERR_NOT_AVAILABLE;
}

PKFReturnCode IPKFDataSinkFilter::process()
{
	if(this->input == NULL)
	{
		return PKF_ERR_NOT_AVAILABLE;
	}

	IPKFConnector *in;

	if(this->input->getConnectedTo(&in) != PKF_SUCCESS)
	{
		return PKF_ERR_NOT_CONNECTED;
	}

	uint32_t sampleId = 0;

	PKFReturnCode result = this->input->getCurrentSampleId(&sampleId);

	if(result != PKF_SUCCESS)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

	IPKFSample *sample = NULL;

	result = PKFSampleManager::lockSample(sampleId, &sample);

	if(result != PKF_SUCCESS)
	{
		return PKF_ERR_NOT_AVAILABLE;
	}

	switch(sample->getMediaType())
	{
	case PKF_MEDIA_TYPE_CUSTOM:
		{
			result = this->pushCustom((PKFCustomSample *) sample);
		}
		break;

	case PKF_MEDIA_TYPE_DATA:
		{
			result = this->push((PKFDataSample *) sample);
		}
		break;

	default:
		{
			result = PKF_ERR_INVALID_PARAM;
		}
		break;
	}

	PKFSampleManager::unlockSample(sampleId);
	
	return result;
}

