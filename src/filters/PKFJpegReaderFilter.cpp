//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//   

#include "PKFJpegReaderFilter.h"

//
// JPEG lib entry points
//

METHODDEF(void) jpegInitSource(j_decompress_ptr cinfo)
{

}

METHODDEF(boolean) jpegSourceError(j_decompress_ptr cinfo)
{
	filterSource *src = (filterSource *) cinfo->src;

    WARNMS(cinfo, JWRN_JPEG_EOF);

	// Insert a fake EOI marker //

	src->buffer[0] = (JOCTET) 0xFF;
	src->buffer[1] = (JOCTET) JPEG_EOI;

	src->manager.next_input_byte = src->buffer;
	src->manager.bytes_in_buffer = 2;

	return TRUE;
}

METHODDEF(boolean) jpegFillInputBuffer(j_decompress_ptr cinfo)
{
	size_t nbytes = 0;

	filterSource      *src   = (filterSource *) cinfo->src;
	PKFInputConnector *input = src->input;

	PKFDataSampleRequest *request = new PKFDataSampleRequest(JPEG_BUFFER_SIZE);

	PKFReturnCode result = input->requestSample(request);

	if(result != PKF_SUCCESS)
	{
    	request->release();
		return jpegSourceError(cinfo);
	}

   	request->release();

	uint32_t       sampleId = 0;
	PKFDataSample *sample   = NULL;

	result = input->getCurrentSampleId(&sampleId);

	if(result != PKF_SUCCESS)
	{
		return jpegSourceError(cinfo);
	}

	result = PKFSampleManager::lockSample(sampleId, (IPKFSample **) &sample);

	if(result != PKF_SUCCESS)
	{
		return jpegSourceError(cinfo);
	}

	PKFDataBuffer *buffer = sample->getBuffer();

	if(buffer->getDataSize() <= JPEG_BUFFER_SIZE)
	{
		void *data = NULL;
		
		buffer->lock(&data);

		if(data == NULL)
		{
			PKFSampleManager::unlockSample(sampleId);
			return jpegSourceError(cinfo);
		}

		memcpy(src->buffer, data, buffer->getDataSize());
		nbytes = buffer->getDataSize();

		buffer->unlock();
	}

	if(nbytes <= 0) 
	{
        PKFSampleManager::unlockSample(sampleId);
		return jpegSourceError(cinfo);
	}

	src->manager.next_input_byte = src->buffer;
	src->manager.bytes_in_buffer = nbytes;

    PKFSampleManager::unlockSample(sampleId);

	return TRUE;
}

METHODDEF(void) jpegSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
	filterSource      *src   = (filterSource *) cinfo->src;

	if(num_bytes > 0) 
	{
		while(num_bytes > (long) src->manager.bytes_in_buffer)
		{
			num_bytes -= (long) src->manager.bytes_in_buffer;
			
			jpegFillInputBuffer(cinfo);

			if(src->manager.bytes_in_buffer == 2             && 
			   src->buffer[0]               == (JOCTET) 0xFF && 
			   src->buffer[1]               == (JOCTET) JPEG_EOI)
			{
				return;
			}
		}

	    src->manager.next_input_byte += (size_t) num_bytes;
		src->manager.bytes_in_buffer -= (size_t) num_bytes;
	}
}

METHODDEF(void) jpegTermSource(j_decompress_ptr cinfo)
{
	// Nothing to do //
}

//
//
//

PKFJpegReaderFilter::PKFJpegReaderFilter()
{
	this->input  = new PKFInputConnector(this, "JPEG Input", PKF_TRANSFER_TYPE_PULL);
	this->output = new PKFOutputConnector(this, "JPEG Output", PKF_TRANSFER_TYPE_PUSH);

	PKFDataMediaTypeDescriptor *desc = new PKFDataMediaTypeDescriptor();
	this->input->addMediaTypeDescriptor(desc);

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

PKFJpegReaderFilter::~PKFJpegReaderFilter()
{
}

PKFReturnCode PKFJpegReaderFilter::onConnected(IPKFConnector *connector, IPKFConnector *to)
{
	if(connector == this->input)
	{
		this->ready = false;
	
		//
		// Setup the JPEG object and obtain
		// information to setup the output types
		//

		this->cinfo.err = jpeg_std_error(&this->jerr);

		jpeg_create_decompress(&this->cinfo);
		
		//
		// setup custom source manager
		// 

		memset(this->jpegSource.buffer, 0, JPEG_BUFFER_SIZE);
		this->jpegSource.input = this->input;

		this->jpegSource.manager.init_source       = jpegInitSource;
		this->jpegSource.manager.fill_input_buffer = jpegFillInputBuffer;
		this->jpegSource.manager.skip_input_data   = jpegSkipInputData;
		this->jpegSource.manager.term_source       = jpegTermSource;
		this->jpegSource.manager.resync_to_restart = jpeg_resync_to_restart;  // use default method
                                                                              //
		this->jpegSource.manager.bytes_in_buffer   = 0;						  // forces fill_input_buffer on first read
                                                                              //
		this->jpegSource.manager.next_input_byte   = this->jpegSource.buffer; // until buffer loaded
                                                                              //

		this->cinfo.src = &(this->jpegSource.manager);
		
		jpeg_read_header(&cinfo, TRUE);
		
		//
		// Check what we got
		//

		switch(this->cinfo.jpeg_color_space)
		{
		case JCS_UNKNOWN:
			{
				jpeg_destroy_decompress(&this->cinfo);
				this->imgDesc = NULL;

				return PKF_ERR_INVALID_PARAM;
			}
			break;
	
		case JCS_GRAYSCALE:
			{
				this->imgDesc = (PKFImageMediaTypeDescriptor *) 
					PKFMediaTypeDescriptorUtility::create8BitGrayscaleImageMediaType(cinfo.image_width,
																					 cinfo.image_height);

				cinfo.out_color_space = JCS_GRAYSCALE;
			}
			break;
		
		case JCS_RGB:
		case JCS_YCbCr:
		case JCS_CMYK:
		case JCS_YCCK:
			{
				this->imgDesc = (PKFImageMediaTypeDescriptor *) 
					PKFMediaTypeDescriptorUtility::create8BitRGBImageMediaType(cinfo.image_width,
																			   cinfo.image_height);

				cinfo.out_color_space = JCS_RGB;
			}
			break;
		}

		this->output->addMediaTypeDescriptor(this->imgDesc);
	
		this->ready = true;
	}


	return PKF_SUCCESS;
}

PKFReturnCode PKFJpegReaderFilter::onDisconnected(IPKFConnector *connector)
{
	PKFReturnCode result = PKF_SUCCESS;

	if(connector == this->input)
	{
		if(this->imgDesc != NULL)
		{
			delete this->imgDesc;
			this->imgDesc = NULL;
		}

		//
		// Invalidate ouput and close JPEG
		// object handle...
		//

		if(this->ready)
		{
			jpeg_destroy_decompress(&this->cinfo);
		}

		this->ready = false;
		this->output->removeMediaTypeDescriptor(0);
	}

	return result;
}

PKFReturnCode PKFJpegReaderFilter::process()
{
	PKFReturnCode result;

	if(!this->ready)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

    uint32_t id = 0;
    this->output->getCurrentSampleId(&id);

	if(id != 0)
	{
		//
		// Image is already decompressed
		//

		return PKF_SUCCESS;
	}

	jpeg_start_decompress(&this->cinfo);

	//
	// Allocate and lock output sample
	//

    uint32_t currentSampleid = 0;

	result = this->output->getSample(this->imgDesc, &currentSampleid);

	if(result != PKF_SUCCESS)
	{
		jpeg_finish_decompress(&this->cinfo);
		this->ready = false;

		return result;
	}

	PKFImageSample *sample = NULL;
	PKFSampleManager::lockSample(currentSampleid, (IPKFSample **) &sample);

	JSAMPLE *scanline = new JSAMPLE[cinfo.output_width * cinfo.output_components];

	switch(cinfo.out_color_space)
	{
	case JCS_GRAYSCALE:
		{
			PKFImageChannel *channel = NULL;
			sample->getImage()->getChannel(0, &channel);

			PKFImageChannelBuffer8Bit *buffer = NULL;
			channel->getChannelBuffer((IPKFImageChannelBuffer **) &buffer);

			uint8_t *buff = NULL;
			buffer->lock(&buff);

			if(buff == NULL)
			{
				delete [] scanline;

				PKFSampleManager::unlockSample(currentSampleid);
				this->output->sendSample();

				return PKF_ERR_INVALID_PARAM;
			}

			for(unsigned int i=0; i < cinfo.output_height; i++)
			{
				int amount = jpeg_read_scanlines(&cinfo, &scanline, 1);

				if(amount == 1)
				{
					memcpy(buff + i*cinfo.output_width, 
						   scanline, cinfo.output_width);
				}
				else
				{
					break;
				}
			}

			buffer->unlock();
		}
		break;

	case JCS_RGB:
		{
			PKFImageChannel *rchannel = NULL;
			sample->getImage()->getChannel(PKFImageChannel::PKF_TYPE_RGB_RED, &rchannel);
			PKFImageChannel *gchannel = NULL;
			sample->getImage()->getChannel(PKFImageChannel::PKF_TYPE_RGB_GREEN, &gchannel);
			PKFImageChannel *bchannel = NULL;
			sample->getImage()->getChannel(PKFImageChannel::PKF_TYPE_RGB_BLUE, &bchannel);

			PKFImageChannelBuffer8Bit *rbuffer = NULL;
			rchannel->getChannelBuffer((IPKFImageChannelBuffer **) &rbuffer);
			PKFImageChannelBuffer8Bit *gbuffer = NULL;
			gchannel->getChannelBuffer((IPKFImageChannelBuffer **) &gbuffer);
			PKFImageChannelBuffer8Bit *bbuffer = NULL;
			bchannel->getChannelBuffer((IPKFImageChannelBuffer **) &bbuffer);

			uint8_t *r = NULL; rbuffer->lock(&r);
			uint8_t *g = NULL; gbuffer->lock(&g);
			uint8_t *b = NULL; bbuffer->lock(&b);

			if(r == NULL || g == NULL || b == NULL)
			{
				rbuffer->unlock();
				gbuffer->unlock();
				bbuffer->unlock();

				delete [] scanline;

				PKFSampleManager::unlockSample(currentSampleid);
				this->output->sendSample();

				return PKF_ERR_INVALID_PARAM;
			}

			for(unsigned int i=0; i < cinfo.output_height; i++)
			{
				int amount = jpeg_read_scanlines(&cinfo, &scanline, 1);

				if(amount == 1)
				{
					// Copy whole line to RGB channels //

					for(unsigned int n=0; n < cinfo.output_width; n++)
					{
						r[n + i*cinfo.output_width] = scanline[3*n    ];
						g[n + i*cinfo.output_width] = scanline[3*n + 1];
						b[n + i*cinfo.output_width] = scanline[3*n + 2];
					}
				}
				else
				{
					break;
				}
			}

			rbuffer->unlock();
			gbuffer->unlock();
			bbuffer->unlock();
		}
		break;
	}

	delete [] scanline;

	PKFSampleManager::unlockSample(currentSampleid);
	this->output->sendSample();

	return PKF_SUCCESS;
}

//
// Entry points
//

PKFReturnCode PKFJpegReaderFilter_Create(IPKFFilter **filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = (IPKFFilter *) new PKFJpegReaderFilter();

	if(*filter == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFJpegReaderFilterProperties_Fill(PKFFilterProperties *prop)
{
	if(prop == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	prop->setFilterId("JPEG_READER");
	prop->setFilterName(L"Jpeg Image Reader");
	prop->setDescription(L"Reads the JPEG image format.");
	prop->setFilterCategory(PKF_FILTER_CATEGORY_IMAGE);
	prop->setAuthors(L"Damien Chavarria");
	prop->setEmail("roy204@gmail.com");
	prop->setHomepage("http://www.projectketchup.com");
	prop->setVendor(L"TomatoSauce");
	prop->setVersion("1.0");

	return PKF_SUCCESS;
}


