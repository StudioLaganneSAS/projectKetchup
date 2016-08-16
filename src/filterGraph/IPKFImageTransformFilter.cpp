//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "IPKFImageTransformFilter.h"


std::string IPKFImageTransformFilter::INPLACE_BOOL_PROPERTY = "inPlace";

//
// IPKFImageTransformFilter
//

IPKFImageTransformFilter::IPKFImageTransformFilter()
:IPKFFilter()
{
	this->input  = new PKFInputConnector(this, "ImageInput", PKF_TRANSFER_TYPE_PUSH);
	this->output = new PKFOutputConnector(this, "ImageOutput", PKF_TRANSFER_TYPE_PUSH);

	this->addConnector(this->input);
	this->addConnector(this->output);

	PKOBJECT_ADD_BOOL_PROPERTY(inPlace, true);
}

IPKFImageTransformFilter::~IPKFImageTransformFilter()
{

}

PKFReturnCode IPKFImageTransformFilter::process()
{
	if(this->input  == NULL || 
	   this->output == NULL)
	{
		return PKF_ERR_NOT_AVAILABLE;
	}

	IPKFConnector *in;
	IPKFConnector *out;

	if(this->input->getConnectedTo(&in)   != PKF_SUCCESS ||
	   this->output->getConnectedTo(&out) != PKF_SUCCESS)
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

	if(sample->getMediaType() != PKF_MEDIA_TYPE_IMAGE)
	{
		PKFSampleManager::unlockSample(sampleId);
		return PKF_ERR_NO_MATCHING_TYPES;
	}

	PKVariant *inplace = this->get(INPLACE_BOOL_PROPERTY);

	if(PKVALUE_BOOL(inplace))
	{
		// Do not create new sample, just pass along

		result = this->transform((PKFImageSample *) sample);

		if(result != PKF_SUCCESS)
		{
			PKFSampleManager::unlockSample(sampleId);
			return result;
		}

		PKFSampleManager::unlockSample(sampleId);

        result = this->output->forceSampleId(sampleId);

        if(result != PKF_SUCCESS)
        {
            return result;
        }

		result = this->output->sendSample();
	}
	else
	{
		// Allocate output sample

		uint32_t outputSampleId = 0;

		PKFImage                    *img  = ((PKFImageSample *)sample)->getImage();
		PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

		for(int i=0; i < img->getNumberOfChannels(); i++)
		{
			PKFImageChannel *ch = NULL;

			PKFReturnCode r = img->getChannel(i, &ch);

			if(r != PKF_SUCCESS)
			{
				PKFSampleManager::unlockSample(sampleId);
				return r;
			}

			desc->addChannelDescriptor(ch->getTypeDesc(), 
									   img->getWidth(), 
									   img->getHeight(), 
									   img->getImageDataType());
		}

		result = this->output->getSample(desc, &outputSampleId);

		delete desc;

		if(result != PKF_SUCCESS)
		{
			PKFSampleManager::unlockSample(sampleId);
			return result;
		}

		// lock it

		IPKFSample *outputSample = NULL;

		result = PKFSampleManager::lockSample(outputSampleId, &outputSample);

		if(result != PKF_SUCCESS)
		{
			PKFSampleManager::unlockSample(sampleId);
			return PKF_ERR_NOT_AVAILABLE;
		}

		// transform

		result = this->transform((PKFImageSample *) sample, (PKFImageSample *) outputSample);

		if(result != PKF_SUCCESS)
		{
			PKFSampleManager::unlockSample(sampleId);
			PKFSampleManager::unlockSample(outputSampleId);
			return result;
		}

		PKFSampleManager::unlockSample(sampleId);
		PKFSampleManager::unlockSample(outputSampleId);

		// send new sample

		result = this->output->sendSample();
		
		if(result != PKF_SUCCESS)
		{
			return result;
		}
	}

	return result;
}

