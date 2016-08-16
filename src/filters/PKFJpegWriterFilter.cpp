//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//   

#include "PKFJpegWriterFilter.h"

//
// PKFJpegWriterFilter
//

METHODDEF(void) jpegInitDestination(j_compress_ptr cinfo)
{
	filterDest *dst = (filterDest *) cinfo->dest;
	
	dst->manager.next_output_byte = dst->buffer;
	dst->manager.free_in_buffer   = JPEG_WRITE_SIZE;
}

METHODDEF(boolean) jpegEmptyOutputBuffer(j_compress_ptr cinfo)
{
	filterDest         *dst    = (filterDest *) cinfo->dest;
	PKFOutputConnector *output = dst->output;

	PKFDataMediaTypeDescriptor *request = new PKFDataMediaTypeDescriptor();
	request->setDataSize(JPEG_WRITE_SIZE);

	uint32_t sampleId = 0;

	PKFReturnCode result = output->getSample(request, &sampleId);

	if(result != PKF_SUCCESS)
	{
		delete request;
		return FALSE;
	}

	delete request;

	PKFDataSample *sample   = NULL;

	result = PKFSampleManager::lockSample(sampleId, (IPKFSample **) &sample);

	if(result != PKF_SUCCESS)
	{
		return FALSE;
	}

	PKFDataBuffer *buffer = sample->getBuffer();

	void *data = NULL;
	buffer->lock(&data);

	if(data == NULL)
	{
		return FALSE;
	}

	memcpy(data, dst->buffer, JPEG_WRITE_SIZE);
	buffer->setDataSize(JPEG_WRITE_SIZE);

	buffer->unlock();

	dst->manager.next_output_byte = dst->buffer;
	dst->manager.free_in_buffer   = JPEG_WRITE_SIZE;

	PKFSampleManager::unlockSample(sampleId);

	output->sendSample();

	return TRUE;
}

METHODDEF(void) jpegTermDestination(j_compress_ptr cinfo)
{
	filterDest         *dst    = (filterDest *) cinfo->dest;
	PKFOutputConnector *output = dst->output;

	uint32_t dataCount = JPEG_WRITE_SIZE - dst->manager.free_in_buffer;

	PKFDataMediaTypeDescriptor *request = new PKFDataMediaTypeDescriptor();
	request->setDataSize(dataCount);

	uint32_t sampleId = 0;

	PKFReturnCode result = output->getSample(request, &sampleId);

	if(result != PKF_SUCCESS)
	{
		delete request;
		return;
	}

	delete request;

	PKFDataSample *sample   = NULL;

	result = PKFSampleManager::lockSample(sampleId, (IPKFSample **) &sample);

	if(result != PKF_SUCCESS)
	{
		return;
	}

	PKFDataBuffer *buffer = sample->getBuffer();

	void *data = NULL;
	buffer->lock(&data);

	if(data == NULL)
	{
		return;
	}

	memcpy(data, dst->buffer, dataCount);
	buffer->setDataSize(dataCount);

	buffer->unlock();

	PKFSampleManager::unlockSample(sampleId);

	output->sendSample();
}

//
// PKFJpegWriterFilter
//

std::string PKFJpegWriterFilter::QUALITY_UINT8_PROPERTY = "quality";


PKFJpegWriterFilter::PKFJpegWriterFilter()
{
	// properties

	PKOBJECT_ADD_UINT8_PROPERTY(quality, 80);

	// connectors

	this->input  = new PKFInputConnector(this, "JPEG Input", PKF_TRANSFER_TYPE_PUSH);
	this->output = new PKFOutputConnector(this, "JPEG Output", PKF_TRANSFER_TYPE_PUSH);

	// output - binary 

	this->output->addMediaTypeDescriptor(new PKFDataMediaTypeDescriptor());

	// input - rgb

	this->input->addMediaTypeDescriptor(PKFMediaTypeDescriptorUtility::create8BitARGBImageMediaType());
	this->input->addMediaTypeDescriptor(PKFMediaTypeDescriptorUtility::create8BitRGBImageMediaType());

	// input - grayscale

	this->input->addMediaTypeDescriptor(PKFMediaTypeDescriptorUtility::create8BitGrayscaleImageMediaType());

	this->addConnector(this->input);
	this->addConnector(this->output);

	//
	// We do not setup output media types
	// yet. We wait for the input pin to 
	// be connected...
	//

	this->ready   = false;
	this->imgDesc = NULL;
}

PKFJpegWriterFilter::~PKFJpegWriterFilter()
{
}

PKFReturnCode PKFJpegWriterFilter::onConnected(IPKFConnector *connector, IPKFConnector *to)
{
	if(connector == this->input)
	{
		this->ready = false;
	
		//
		// Setup the JPEG object and obtain
		// information to setup the data types
		//

		this->cinfo.err = jpeg_std_error(&this->jerr);

		jpeg_create_compress(&this->cinfo);
		
		//
		// setup custom source manager
		// 

		memset(this->jpegDest.buffer, 0, JPEG_WRITE_SIZE);
		this->jpegDest.output = this->output;

		this->jpegDest.manager.init_destination    = jpegInitDestination;
		this->jpegDest.manager.empty_output_buffer = jpegEmptyOutputBuffer;
		this->jpegDest.manager.term_destination    = jpegTermDestination;

		this->cinfo.dest = &(this->jpegDest.manager);

		//
		// See what media type we have
		//

		IPKFMediaTypeDescriptor *desc = NULL;

		PKFReturnCode result = this->input->getCurrentMediaType(&desc);

		if(result != PKF_SUCCESS)
		{
			jpeg_destroy_compress(&this->cinfo);
			return PKF_ERR_NOT_CONNECTED;
		}

		if(desc->getMediaType() != PKF_MEDIA_TYPE_IMAGE)
		{
			jpeg_destroy_compress(&this->cinfo);
			return PKF_ERR_NO_MATCHING_TYPES;
		}

		PKFImageMediaTypeDescriptor *imageDesc = (PKFImageMediaTypeDescriptor *) desc;

		// check colorspace

		if(imageDesc->getNumberOfChannels() == 3 ||
		   imageDesc->getNumberOfChannels() == 4)
		{
			PKFImageChannelDescriptor *channel1;
			PKFImageChannelDescriptor *channel2;
			PKFImageChannelDescriptor *channel3;

			imageDesc->getChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_RED, &channel1);
			imageDesc->getChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_GREEN, &channel2);
			imageDesc->getChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_BLUE, &channel3);

			if(channel1 == NULL || channel2 == NULL || channel3 == NULL)
			{
				return PKF_ERR_NO_MATCHING_TYPES;
			}

			if(channel1->getImageDataType() == PKF_IMAGE_DATA_TYPE_8_BITS  &&
			   channel1->getDescription()   == PKFImageChannel::PKF_TYPE_RGB_RED   &&
			   channel2->getImageDataType() == PKF_IMAGE_DATA_TYPE_8_BITS  &&
			   channel2->getDescription()   == PKFImageChannel::PKF_TYPE_RGB_GREEN &&
			   channel3->getImageDataType() == PKF_IMAGE_DATA_TYPE_8_BITS  &&
			   channel3->getDescription()   == PKFImageChannel::PKF_TYPE_RGB_BLUE)
			{
				this->ready   = true;
				this->imgDesc = imageDesc;
				
				return PKF_SUCCESS;
			}
		}
		else if(imageDesc->getNumberOfChannels() == 1)
		{
			PKFImageChannelDescriptor *channel1;

			imageDesc->getChannelDescriptor(0, &channel1);

			if(channel1->getImageDataType() == PKF_IMAGE_DATA_TYPE_8_BITS  &&
			   channel1->getDescription()   == PKFImageChannel::PKF_TYPE_GRAYSCALE)
			{
				this->ready   = true;
				this->imgDesc = imageDesc;
				
				return PKF_SUCCESS;
			}
		}

		jpeg_destroy_compress(&this->cinfo);
		return PKF_ERR_NO_MATCHING_TYPES;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFJpegWriterFilter::onDisconnected(IPKFConnector *connector)
{
	PKFReturnCode result = PKF_SUCCESS;

	if(connector == this->input && this->ready)
	{
		if(this->imgDesc != NULL)
		{
			this->imgDesc = NULL;
		}

		this->ready = false;

		//
		// Invalidate ouput and close JPEG
		// object handle...
		//

		jpeg_destroy_compress(&this->cinfo);
	}

	return result;
}

PKFReturnCode PKFJpegWriterFilter::process()
{
	PKFReturnCode result;

	if(!this->ready || this->imgDesc == NULL)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

	uint32_t sampleId = 0;

	result = this->input->getCurrentSampleId(&sampleId);

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

	PKFImageSample *isample = (PKFImageSample *) sample;
	PKFImage       *image   = isample->getImage();

	if((this->imgDesc->getNumberOfChannels() == 3 &&
	    image->getNumberOfChannels()         == 3) ||
	   (this->imgDesc->getNumberOfChannels() == 4 &&
	    image->getNumberOfChannels()         == 4))
	{
		// (A)RGB

		this->cinfo.image_width      = image->getWidth();
		this->cinfo.image_height     = image->getHeight();
		this->cinfo.input_components = 3;
		this->cinfo.in_color_space   = JCS_RGB;
	}
	else if(this->imgDesc->getNumberOfChannels() == 1 &&
			image->getNumberOfChannels()         == 1)
	{
		// GRAYSCALE

		this->cinfo.image_width      = image->getWidth();
		this->cinfo.image_height     = image->getHeight();
		this->cinfo.input_components = 1;
		this->cinfo.in_color_space   = JCS_GRAYSCALE;
	}
	else
	{
		PKFSampleManager::unlockSample(sampleId);
		return PKF_ERR_NO_MATCHING_TYPES;
	}

	jpeg_set_defaults(&this->cinfo);

	PKVariant *quality = this->get(QUALITY_UINT8_PROPERTY);
	jpeg_set_quality(&this->cinfo, PKVALUE_UINT8(quality), FALSE);

	jpeg_start_compress(&this->cinfo, TRUE);

	JSAMPLE *scanline = new JSAMPLE[cinfo.image_width * cinfo.input_components];

	switch(image->getNumberOfChannels())
	{
	case 1: // GRAYSCALE
		{
			PKFImageChannel *channel = NULL;
			image->getChannel(0, &channel);

			PKFImageChannelBuffer8Bit *buffer = NULL;
			channel->getChannelBuffer((IPKFImageChannelBuffer **) &buffer);

			uint8_t *buff = NULL;
			buffer->lock(&buff);

			if(buff == NULL)
			{
				PKFSampleManager::unlockSample(sampleId);
				return PKF_ERR_INVALID_PARAM;
			}

			for(unsigned int i=0; i < this->cinfo.image_height; i++)
			{
				memcpy(scanline, 
					   buff + i*this->cinfo.image_width, 
					   this->cinfo.image_width);

				jpeg_write_scanlines(&this->cinfo, &scanline, 1);
			}

			buffer->unlock();
		}
		break;

	case 3: // RGB
	case 4: // ARGB
		{
			PKFImageChannel *rchannel = NULL;
			image->getChannel(PKFImageChannel::PKF_TYPE_RGB_RED, &rchannel);

			PKFImageChannel *gchannel = NULL;
			image->getChannel(PKFImageChannel::PKF_TYPE_RGB_GREEN, &gchannel);

			PKFImageChannel *bchannel = NULL;
			image->getChannel(PKFImageChannel::PKF_TYPE_RGB_BLUE, &bchannel);

			PKFImageChannelBuffer8Bit *rbuffer = NULL;
			rchannel->getChannelBuffer((IPKFImageChannelBuffer **) &rbuffer);

			PKFImageChannelBuffer8Bit *gbuffer = NULL;
			gchannel->getChannelBuffer((IPKFImageChannelBuffer **) &gbuffer);

			PKFImageChannelBuffer8Bit *bbuffer = NULL;
			bchannel->getChannelBuffer((IPKFImageChannelBuffer **) &bbuffer);

			uint8_t *r = NULL; rbuffer->lock(&r);
			uint8_t *g = NULL; gbuffer->lock(&g);
			uint8_t *b = NULL; bbuffer->lock(&b);

			for(unsigned int j=0; j < this->cinfo.image_height; j++)
			{
				for(unsigned int i=0; i < this->cinfo.image_width; i++)
				{
					scanline[3*i]     = r[i + j*this->cinfo.image_width];
					scanline[3*i + 1] = g[i + j*this->cinfo.image_width];
					scanline[3*i + 2] = b[i + j*this->cinfo.image_width];
				}

				jpeg_write_scanlines(&this->cinfo, &scanline, 1);
			}

			rbuffer->unlock();
			gbuffer->unlock();
			bbuffer->unlock();
		}
		break;
	}

	delete [] scanline;

	jpeg_finish_compress(&this->cinfo);

	PKFSampleManager::unlockSample(sampleId);

	// binary samples have already been sent by
	// the destination manager from cinfo

	return PKF_SUCCESS;
}


//
// Entry points
//

PKFReturnCode PKFJpegWriterFilter_Create(IPKFFilter **filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = (IPKFFilter *) new PKFJpegWriterFilter();

	if(*filter == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFJpegWriterFilterProperties_Fill(PKFFilterProperties *prop)
{
	if(prop == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	prop->setFilterId("JPEG_WRITER");
	prop->setFilterName(L"Jpeg Image Writer");
	prop->setDescription(L"Writes the JPEG image format.");
	prop->setFilterCategory(PKF_FILTER_CATEGORY_IMAGE);
	prop->setAuthors(L"Damien Chavarria");
	prop->setEmail("roy204@gmail.com");
	prop->setHomepage("http://www.projectketchup.com");
	prop->setVendor(L"TomatoSauce");
	prop->setVersion("1.0");

	return PKF_SUCCESS;
}

