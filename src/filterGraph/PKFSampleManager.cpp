//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFSampleManager.h"

//
// Statics
//

int								  PKFSampleManager::sampleCount = 0;
PKCritSec      					  PKFSampleManager::mutex;
std::map <uint32_t, IPKFSample *> PKFSampleManager::samples;
std::wstring					  PKFSampleManager::scratchPath = L"";
PKFSampleAllocationType			  PKFSampleManager::allocType = PKF_SAMPLE_ALLOCATE_ALL_IN_MEMORY;


//
// PKFSampleManager
//

PKFReturnCode PKFSampleManager::setScratchPath(std::wstring path)
{
	PKFSampleManager::scratchPath = path;
	return PKF_SUCCESS;
}

std::wstring PKFSampleManager::getScratchPath()
{
	return PKFSampleManager::scratchPath;
}

PKFReturnCode PKFSampleManager::setDefaultAllocationType(PKFSampleAllocationType type)
{
	if(type == PKF_SAMPLE_ALLOCATE_DEFAULT)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	PKFSampleManager::allocType = type;
	return PKF_SUCCESS;
}

PKFSampleAllocationType PKFSampleManager::getDefaultAllocationType()
{
	return PKFSampleManager::allocType;
}

PKFReturnCode PKFSampleManager::allocateSample(IPKFMediaTypeDescriptor *desc, 
											   uint32_t *sampleId,
											   PKFSampleAllocationType type)
{
	PKCritLock l(&PKFSampleManager::mutex);

	if(desc == NULL || sampleId == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	switch(desc->getMediaType())
	{
	case PKF_MEDIA_TYPE_IMAGE:
		{
			bool inScratch = false;

			if(type == PKF_SAMPLE_ALLOCATE_DEFAULT)
			{
				type = PKFSampleManager::allocType;
			}

			switch(type)
			{
			case PKF_SAMPLE_ALLOCATE_ALL_IN_SCRATCH:
			case PKF_SAMPLE_ALLOCATE_IMAGES_IN_SCRATCH:
				{
					inScratch = true;
				}
			}

			PKFImageMediaTypeDescriptor *iDesc = (PKFImageMediaTypeDescriptor *) desc;

			PKFImageSample *sample = new PKFImageSample();
			PKFImage       *image  = new PKFImage();

			if(sample == NULL || image == NULL) 
			{
				return PKF_ERR_OUT_OF_MEMORY;
			}

			for(unsigned int i=0; i < iDesc->getNumberOfChannels(); i++)
			{
				PKFImageChannelDescriptor *ch = NULL;
					
				PKFReturnCode result = iDesc->getChannelDescriptor(i, &ch);

				if(result != PKF_SUCCESS)
				{
					delete image;
					sample->release();

					return result;
				}

				result = image->addChannel(ch->getDescription(),
										   ch->getWidth(),
										   ch->getHeight(),
										   ch->getImageDataType(),
										   (inScratch ? PKF_DATA_BUFFER_SCRATCH : PKF_DATA_BUFFER_MEMORY));

				if(result != PKF_SUCCESS)
				{
					delete image;
					sample->release();

					return result;
				}
			}

			PKFSampleManager::sampleCount++;
			sample->setImage(image);

			PKFSampleManager::samples[PKFSampleManager::sampleCount] = sample;
		}
		break;

	case PKF_MEDIA_TYPE_DATA:
		{
			PKFSampleAllocationType whatToDo;
		
			if(type == PKF_SAMPLE_ALLOCATE_ALL_IN_SCRATCH)
			{
				whatToDo = type;
			}
			else 
			{
				whatToDo = PKFSampleManager::allocType;
			}
			
			PKFDataMediaTypeDescriptor *dDesc = (PKFDataMediaTypeDescriptor *) desc;

			PKFDataSample *sample = new PKFDataSample();
			PKFDataBuffer *buffer = new PKFDataBuffer((whatToDo == PKF_SAMPLE_ALLOCATE_ALL_IN_SCRATCH ? PKF_DATA_BUFFER_SCRATCH : PKF_DATA_BUFFER_MEMORY));

			if(sample == NULL || buffer == NULL) 
			{
				return PKF_ERR_OUT_OF_MEMORY;
			}

			PKFReturnCode result = buffer->alloc(dDesc->getDataSize());

			if(result != PKF_SUCCESS)
			{
				sample->release();
				delete buffer;

				return result;
			}	

			PKFSampleManager::sampleCount++;
			sample->setBuffer(buffer);

			PKFSampleManager::samples[PKFSampleManager::sampleCount] = sample;
		}
		break;

	case PKF_MEDIA_TYPE_CUSTOM:
		{
			PKFCustomMediaTypeDescriptor *cDesc = (PKFCustomMediaTypeDescriptor *) desc;

			PKVariant *dv = cDesc->get(PKFCustomMediaTypeDescriptor::TYPE_IDENTIFIER_PROPERTY);
			PKFCustomSample *sample = new PKFCustomSample(PKVALUE_STRING(dv));

			if(sample == NULL) 
			{
				return PKF_ERR_OUT_OF_MEMORY;
			}

			PKFSampleManager::sampleCount++;

			PKFSampleManager::samples[PKFSampleManager::sampleCount] = sample;
		}
		break;
	}

	*sampleId = PKFSampleManager::sampleCount;

	return PKF_SUCCESS;
}

PKFReturnCode PKFSampleManager::deallocateSample(uint32_t sampleId)
{
	PKCritLock l(&PKFSampleManager::mutex);

	IPKFSample *sample = PKFSampleManager::samples[sampleId];

	if(sample == NULL)
	{
		return PKF_ERR_NOT_ALLOCATED;
	}	

	switch(sample->getMediaType())
	{
	case PKF_MEDIA_TYPE_IMAGE:
		{
			PKFImageSample *sp = (PKFImageSample *) sample;

			PKFImage *image = sp->getImage();

			if(image != NULL)
			{
				delete image;
			}

			sp->release();

			PKFSampleManager::samples[sampleId] = NULL;
		}
		break;

	case PKF_MEDIA_TYPE_DATA:
		{
			PKFDataSample *sp = (PKFDataSample *) sample;

			PKFDataBuffer *buffer = sp->getBuffer();

			if(buffer != NULL)
			{
				delete buffer;
			}

			sp->release();

			PKFSampleManager::samples[sampleId] = NULL;
		}
		break;

	case PKF_MEDIA_TYPE_CUSTOM:
		{
			PKFCustomSample *sp = (PKFCustomSample *) sample;
			sp->release();

			PKFSampleManager::samples[sampleId] = NULL;
		}
		break;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFSampleManager::lockSample(uint32_t sampleId, IPKFSample **sample)
{
	PKCritLock l(&PKFSampleManager::mutex);
	
	if(sample == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	IPKFSample *s = PKFSampleManager::samples[sampleId];

	if(s == NULL)
	{
		return PKF_ERR_NOT_ALLOCATED;
	}

	if(s->isLocked())
	{
		return PKF_ERR_ALREADY_LOCKED;
	}

	s->lock();

	*sample = s;

	return PKF_SUCCESS;
}

PKFReturnCode PKFSampleManager::unlockSample(uint32_t sampleId)
{
	PKCritLock l(&PKFSampleManager::mutex);
	
	IPKFSample *s = PKFSampleManager::samples[sampleId];

	if(s == NULL)
	{
		return PKF_ERR_NOT_ALLOCATED;
	}

	if(!s->isLocked())
	{
		return PKF_ERR_NOT_LOCKED;
	}

	s->unlock();

	return PKF_SUCCESS;
}

