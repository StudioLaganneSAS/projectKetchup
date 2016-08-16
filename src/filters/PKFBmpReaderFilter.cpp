//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFBmpReaderFilter.h"

#ifdef LINUX
#ifndef abs
#define abs(x) (x < 0 ? -x : x)
#endif
#endif

//
// Constants
//

#define BMP_BI_RGB      0
#define BMP_BI_RLE8     1
#define BMP_BI_RLE4     2
#define BMP_BI_BITFIELD 3

//
// PKFBmpReaderFilter (utils)
//

uint32_t PKFBmpReaderFilter::readSource(uint8_t *buff, uint32_t size)
{
	//
	// Check if we have been asked
	// to cancel here, that way it
	// will get checked when reading 
	// each line of the bitmap and 
	// during RLE loading...
	//

	if(this->getStatus() == PKF_FILTER_STATUS_INTERRUPTING)
	{
		return 0;
	}	

	PKFDataSampleRequest *request = new PKFDataSampleRequest(size);

	PKFReturnCode result = input->requestSample(request);

	if(result != PKF_SUCCESS)
	{
		request->release();
		return 0;
	}

	uint32_t       sampleId = 0;
	PKFDataSample *sample   = NULL;

	result = input->getCurrentSampleId(&sampleId);

	if(result != PKF_SUCCESS)
	{
		request->release();
		return 0;
	}

	result = PKFSampleManager::lockSample(sampleId, (IPKFSample **) &sample);

	if(result != PKF_SUCCESS)
	{
		request->release();
		return 0;
	}

	PKFDataBuffer *buffer = sample->getBuffer();

	if(buffer->getDataSize() > 0)
	{
		void *data = NULL;
		
		buffer->lock(&data);

		if(data == NULL)
		{
    		request->release();
			return 0;
		}

		uint32_t rsize = buffer->getDataSize();

		memcpy(buff, data, rsize);

		buffer->unlock();
	
		request->release();
		
		return rsize;
	}

	request->release();
	return 0;
}

//
// PKFBmpReaderFilter
//

PKFBmpReaderFilter::PKFBmpReaderFilter()
{
	this->input  = new PKFInputConnector(this, "BMPInput", PKF_TRANSFER_TYPE_PULL);
	this->output = new PKFOutputConnector(this, "BMPOutput", PKF_TRANSFER_TYPE_PUSH);

	PKFDataMediaTypeDescriptor *desc = new PKFDataMediaTypeDescriptor();
	this->input->addMediaTypeDescriptor(desc);

	this->addConnector(this->input);
	this->addConnector(this->output);

	//
	// We do not setup output media types
	// yet. We wait for the input pin to 
	// be connected to do so (so we know 
	// the actual image/channels type)...
	//

	this->ready = false;
	this->imgDesc = NULL;
}

PKFBmpReaderFilter::~PKFBmpReaderFilter()
{

}

PKFReturnCode PKFBmpReaderFilter::onConnected(IPKFConnector *connector, 
											  IPKFConnector *to)
{
	if(connector == this->input)
	{
		this->ready = false;
	
		//
		// Check what we got and
		// parse BMP header if any...
		//

		uint32_t headerSize = sizeof(bmpHeader_t) + 2;
		uint8_t *buffer = new uint8_t[headerSize];

		if(buffer == NULL)
		{
			return PKF_ERR_OUT_OF_MEMORY;
		}

		if(this->readSource(buffer, headerSize) != headerSize)
		{
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		// Check header

		if(buffer[0] != 'B'	|| buffer[1] != 'M')
		{
			delete [] buffer;
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		memcpy(&this->header, buffer+2, sizeof(bmpHeader_t));

		delete [] buffer;

		if((this->header.bpp != 1  &&
		    this->header.bpp != 4  &&
		    this->header.bpp != 8  &&
		    this->header.bpp != 16 &&
		    this->header.bpp != 24 &&
		    this->header.bpp != 32) ||
		   (this->header.compression != BMP_BI_RGB   &&
		    this->header.compression != BMP_BI_RLE4  &&
			this->header.compression != BMP_BI_RLE8  &&
			this->header.compression != BMP_BI_BITFIELD) )
		{
			// Invalid file?

			return PKF_ERR_OBJECT_NOT_VALID;
		}

		if(this->header.bpp == 32 && 
		   this->header.compression == BMP_BI_RGB)
		{
			this->imgDesc = (PKFImageMediaTypeDescriptor *) 
							PKFMediaTypeDescriptorUtility::create8BitARGBImageMediaType(this->header.width, 
																			        abs(this->header.height));
		}
		else
		{
			this->imgDesc = (PKFImageMediaTypeDescriptor *) 
							PKFMediaTypeDescriptorUtility::create8BitRGBImageMediaType(this->header.width, 
																			       abs(this->header.height));
		}

		this->output->addMediaTypeDescriptor(this->imgDesc);
		this->ready = true;
	}


	return PKF_SUCCESS;
}

PKFReturnCode PKFBmpReaderFilter::onDisconnected(IPKFConnector *connector)
{
	PKFReturnCode result = PKF_SUCCESS;

	if(connector == this->input)
	{
		if(this->imgDesc != NULL)
		{
			delete this->imgDesc;
			this->imgDesc = NULL;
		}

		this->ready = false;
		this->output->removeMediaTypeDescriptor(0);
	}

	return result;
}

//
// Read sub-routines
//

void PKFBmpReaderFilter::readRGB1(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette)
{
	uint32_t lwidth     = ((header.width/8) + (header.width%8 ? 1 : 0));
	uint32_t linelength = lwidth + (lwidth % 4 ? (4 - (lwidth % 4)) : 0);

	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;
	
	if(this->header.height > 0)
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t h  = abs(this->header.height);
	uint32_t w  = this->header.width/8;
	uint8_t odd = (header.width % 8);

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			for(int k = 0; k < 8; k++)
			{
				uint8_t index = ((line[loffset] >> (7-k)) & 0x01);

				b[doffset] = palette[4*index];
				g[doffset] = palette[4*index + 1];
				r[doffset] = palette[4*index + 2];
			
				doffset += 1;
			}

			loffset++;
		}

		if(odd)
		{
			for(int k = 0; k < odd; k++)
			{
				uint8_t index = ((line[loffset] >> (7-k)) & 0x01);

				b[doffset] = palette[4*index];
				g[doffset] = palette[4*index + 1];
				r[doffset] = palette[4*index + 2];
			
				doffset += 1;
			}
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readRGB4(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette)
{
	uint32_t lwidth     = (header.width/2) + (header.width%2);
	uint32_t linelength =  lwidth + ((lwidth % 4) ? (4 - (lwidth % 4)): 0);

	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;
	
	if(this->header.height > 0)
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t w  = this->header.width/2;
	uint32_t h  = abs(this->header.height);
	uint8_t odd = this->header.width % 2;

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			uint8_t index1 = ((line[loffset] >> 4) & 0x0F);
			uint8_t index2 = ((line[loffset])      & 0x0F);

			b[doffset] = palette[4*index1];
			g[doffset] = palette[4*index1 + 1];
			r[doffset] = palette[4*index1 + 2];
			
			b[doffset+1] = palette[4*index2];
			g[doffset+1] = palette[4*index2 + 1];
			r[doffset+1] = palette[4*index2 + 2];

			doffset += 2;
			loffset++;
		}

		if(odd)
		{
			uint8_t index1 = ((line[loffset] >> 4) & 0x0F);

			b[doffset] = palette[4*index1];
			g[doffset] = palette[4*index1 + 1];
			r[doffset] = palette[4*index1 + 2];
			
			doffset += 1;
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readRGB8(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette)
{
	uint32_t linelength = header.width + (header.width % 4 ? (4 - (header.width % 4)) : 0);
	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;
	
	if(this->header.height > 0)
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t w = this->header.width;
	uint32_t h = abs(this->header.height);

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			uint8_t index = line[loffset];

			b[doffset] = palette[4*index];
			g[doffset] = palette[4*index + 1];
			r[doffset] = palette[4*index + 2];
			
			doffset++;
			loffset++;
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readRGB16(uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint32_t lwidth = header.width*2;
	uint32_t linelength = lwidth + (lwidth % 4 ? (4 - (lwidth % 4)) : 0);
	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;
	
	if(this->header.height > 0)
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t w = this->header.width;
	uint32_t h = abs(this->header.height);

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			uint16_t pixel = ((uint16_t *) line)[loffset]; // 555

			r[doffset] = ((pixel >> 10) << 3) & 0xFF;
			g[doffset] = ((pixel >> 5)  << 3) & 0xFF;
			b[doffset] = ((pixel << 3)      ) & 0xFF;
			
			doffset++;
			loffset++;
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readRGB24(uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint32_t lwidth = header.width*3;
	uint32_t linelength = lwidth + (lwidth % 4 ? (4 - (lwidth % 4)) : 0);
	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;
	
	if(this->header.height > 0)
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t w = this->header.width;
	uint32_t h = abs(this->header.height);

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			// order is BGR

			r[doffset] = line[loffset + 2];
			g[doffset] = line[loffset + 1];
			b[doffset] = line[loffset];
			
			doffset++;
			loffset += 3;
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readRGB32(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a)
{
	uint32_t linelength = this->header.width*4;
	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;

	if(this->header.height > 0) // not top down
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t w = this->header.width;
	uint32_t h = abs(this->header.height);

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			// order is BGRA

			a[doffset] = line[loffset + 3];
			r[doffset] = line[loffset + 2];
			g[doffset] = line[loffset + 1];
			b[doffset] = line[loffset];
			
			doffset++;
			loffset += 4;
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readBITFIELDS16(uint8_t *r, uint8_t *g, uint8_t *b)
{
	// read masks

	uint32_t masks[3];
	uint8_t  offs[3];
	uint8_t  offd[3];

	if(this->readSource((uint8_t *) masks, 12) != 12)
	{
		// invalid file
		return;
	}

	for(uint8_t n = 0; n < 3; n++)
	{
		offs[n] = 0;
		offd[n] = 0;

		if(masks[n] == 0)
			continue;

		// find first bit set

		while(true)
		{
			if((masks[n] & (1 << offs[n])))
			{
				break;
			}

			offs[n]++;
		}

		// find last bit set

		while(true)
		{
			if( ((masks[n] >> offs[n]) & (1 << offd[n])) == 0)
			{
				break;
			}

			offd[n]++;
		}
	}

	// read data

	uint32_t lwidth = header.width*2;
	uint32_t linelength = lwidth + (lwidth % 4 ? (4 - (lwidth % 4)) : 0);
	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;
	
	if(this->header.height > 0)
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t w = this->header.width;
	uint32_t h = abs(this->header.height);

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			uint16_t pixel = ((uint16_t *) line)[loffset]; // 555

			r[doffset] = (((pixel & masks[0]) >> offs[0]) << (8 - offd[0]));
			g[doffset] = (((pixel & masks[1]) >> offs[1]) << (8 - offd[1]));
			b[doffset] = (((pixel & masks[2]) >> offs[2]) << (8 - offd[2]));
			
			doffset++;
			loffset++;
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readBITFIELDS32(uint8_t *r, uint8_t *g, uint8_t *b)
{
	// read masks

	uint32_t masks[3];
	uint8_t  offs[3];
	uint8_t  offd[3];

	if(this->readSource((uint8_t *) masks, 12) != 12)
	{
		// invalid file
		return;
	}

	for(uint8_t n = 0; n < 3; n++)
	{
		offs[n] = 0;
		offd[n] = 0;

		if(masks[n] == 0)
			continue;

		// find first bit set

		while(true)
		{
			if((masks[n] & (1 << offs[n])))
			{
				break;
			}

			offs[n]++;
		}

		// find last bit set

		while(true)
		{
			if( ((masks[n] >> offs[n]) & (1 << offd[n])) == 0)
			{
				break;
			}

			offd[n]++;
		}
	}

	// read data

	uint32_t linelength = header.width*4;
	uint8_t *line = new uint8_t[linelength];

	uint32_t doffset = 0;
	
	if(this->header.height > 0)
	{
		doffset = (this->header.height - 1)*this->header.width;
	}

	uint32_t w = this->header.width;
	uint32_t h = abs(this->header.height);

	for(unsigned int j=0; j < h; j++)
	{
		if(this->readSource(line, linelength) != linelength)
		{
			break;
		}

		uint32_t loffset = 0;

		for(unsigned int i=0; i < w; i++)
		{
			uint32_t pixel = ((uint32_t *) line)[loffset]; // XXXX ?

			r[doffset] = (((pixel & masks[0]) >> offs[0]) << (8 - offd[0]));
			g[doffset] = (((pixel & masks[1]) >> offs[1]) << (8 - offd[1]));
			b[doffset] = (((pixel & masks[2]) >> offs[2]) << (8 - offd[2]));
			
			doffset++;
			loffset++;
		}

		if(this->header.height > 0)
		{
			doffset -= 2*this->header.width;
		}
	}
	
	delete [] line;
}

void PKFBmpReaderFilter::readRLE4(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette)
{
	// Read RLE4 data by blocks of 32768 bytes

	uint8_t  buffer[32768];
	bool     done = false;
	
	uint32_t x = 0;
	uint32_t y = this->header.height - 1;

	uint32_t index = 0;
	uint32_t read  = this->readSource(buffer, 32768);

	while(!done)
	{
		uint8_t head;
		
		if(!this->getNextByte(buffer, &read, &index, &head))
		{
			return;
		}

		if(head > 0)
		{
			uint8_t pindex;
			
			if(!this->getNextByte(buffer, &read, &index, &pindex))
			{
				return;
			}

			for(int k=0; k < head; k++)
			{
				int offset = y*this->header.width + x;

				if(x < this->header.width)
				{
					if(k & 1)
					{
						b[offset] = palette[4*(pindex & 0x0F)];
						g[offset] = palette[4*(pindex & 0x0F) + 1];
						r[offset] = palette[4*(pindex & 0x0F) + 2];
					}
					else
					{
						b[offset] = palette[4*((pindex >> 4) & 0x0F)];
						g[offset] = palette[4*((pindex >> 4) & 0x0F) + 1];
						r[offset] = palette[4*((pindex >> 4) & 0x0F) + 2];
					}
				}

				x++;
			}
		}
		else
		{
			uint8_t tail;
			
			if(!this->getNextByte(buffer, &read, &index, &tail))
			{
				return;
			}

			if(tail == 0)
			{
				// end of line
				x = 0;
				y--;
			}
			else if(tail == 1)
			{	
				done = true;
			}
			else if(tail == 2)
			{
				// move cursor

				uint8_t xoff;
				uint8_t yoff;

				if(!this->getNextByte(buffer, &read, &index, &xoff) ||
				   !this->getNextByte(buffer, &read, &index, &yoff))
				{
					return;
				}

				x += xoff;
				y -= yoff;
			}
			else
			{
				int off = 0;

				for(int k = 0; k < (tail/2); k++)
				{
					uint8_t pindex;
					
					if(!this->getNextByte(buffer, &read, &index, &pindex))
					{
						return;
					}

					off++;

					int offset = y*this->header.width + x;

					if(x < this->header.width)
					{
						b[offset] = palette[4*((pindex >> 4) & 0x0F)];
						g[offset] = palette[4*((pindex >> 4) & 0x0F) + 1];
						r[offset] = palette[4*((pindex >> 4) & 0x0F) + 2];
					}

					x++;

					if(x < this->header.width)
					{
						b[offset+1] = palette[4*(pindex & 0x0F)];
						g[offset+1] = palette[4*(pindex & 0x0F) + 1];
						r[offset+1] = palette[4*(pindex & 0x0F) + 2];
					}

					x++;
				}

				if(tail%2) 
				{
					uint8_t pindex;
					
					if(!this->getNextByte(buffer, &read, &index, &pindex))
					{
						return;
					}

					off++;

					int offset = y*this->header.width + x;

					if(x < this->header.width)
					{
						b[offset] = palette[4*((pindex >> 4) & 0x0F)];
						g[offset] = palette[4*((pindex >> 4) & 0x0F) + 1];
						r[offset] = palette[4*((pindex >> 4) & 0x0F) + 2];
					}

					x++;
				}

				if(off % 2)
				{
					uint8_t dummy;
					this->getNextByte(buffer, &read, &index, &dummy);
				}
			}
		}
	}
}

void PKFBmpReaderFilter::readRLE8(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette)
{
	// Read RLE8 data by blocks of 32768 bytes

	uint8_t  buffer[32768];
	bool     done = false;

	uint32_t x = 0;
	uint32_t y = this->header.height - 1;

	uint32_t index = 0;
	uint32_t read  = this->readSource(buffer, 32768);

	while(!done)
	{
		uint8_t head;
		
		if(!this->getNextByte(buffer, &read, &index, &head))
		{
			return;
		}

		if(head > 0)
		{
			uint8_t pindex;
			
			if(!this->getNextByte(buffer, &read, &index, &pindex))
			{
				return;
			}

			for(int k=0; k < head; k++)
			{
				int offset = y*this->header.width + x;

				b[offset] = palette[4*pindex];
				g[offset] = palette[4*pindex + 1];
				r[offset] = palette[4*pindex + 2];

				x++;
			}
		}
		else
		{
			uint8_t tail;
			
			if(!this->getNextByte(buffer, &read, &index, &tail))
			{
				return;
			}

			if(tail == 0)
			{
				// end of line - nothing to do
				x = 0;
				y--;
			}
			else if(tail == 1)
			{	
				done = true;
			}
			else if(tail == 2)
			{
				// move cursor

				uint8_t xoff; 
				uint8_t yoff;

				if(!this->getNextByte(buffer, &read, &index, &xoff) ||
				   !this->getNextByte(buffer, &read, &index, &yoff))
				{
					return;
				}

				x += xoff;
				y -= yoff;

			}
			else
			{
				for(int k = 0; k < tail; k++)
				{
					uint8_t pindex;
					
					if(!this->getNextByte(buffer, &read, &index, &pindex))
					{
						return;
					}

					int offset = y*this->header.width + x;

					b[offset] = palette[4*pindex];
					g[offset] = palette[4*pindex + 1];
					r[offset] = palette[4*pindex + 2];

					x++;
				}

				if(tail%2) 
				{
					uint8_t dummy;
					this->getNextByte(buffer, &read, &index, &dummy);
				}
			}
		}
	}
}

//
//
//

PKFReturnCode PKFBmpReaderFilter::process()
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

	//
	// Allocate and lock output sample
	//

    uint32_t currentSampleid;

	result = this->output->getSample(this->imgDesc, &currentSampleid);

	if(result != PKF_SUCCESS)
	{
		this->ready = false;
		return result;
	}

	PKFImageSample *sample = NULL;
	PKFSampleManager::lockSample(currentSampleid, (IPKFSample **) &sample);

	// RGB
	
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

	// Alpha

	PKFImageChannel *achannel = NULL;
	PKFImageChannelBuffer8Bit *abuffer = NULL;

	if(this->header.bpp == 32 && 
	   this->header.compression == BMP_BI_RGB &&
	   sample->getImage()->getNumberOfChannels() == 4)
	{
		sample->getImage()->getChannel(PKFImageChannel::PKF_TYPE_ALPHA, &achannel);
		achannel->getChannelBuffer((IPKFImageChannelBuffer **) &abuffer);
	}

	//
	// Read palette if any
	//

	uint8_t *palette = NULL;

	if(this->header.bpp == 8)
	{
		uint32_t numcolors = this->header.colors;

		if(numcolors == 0)
		{
			numcolors = 256;
		}

		if(numcolors > 256)
		{
			this->ready = false;
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		// Read palette

		palette = new uint8_t[numcolors*4];

		if(palette == NULL ||
		   this->readSource(palette, numcolors*4) != (numcolors*4))
		{
			this->ready = false;
			return PKF_ERR_OBJECT_NOT_VALID;
		}
	}

	if(this->header.bpp == 4)
	{
		uint32_t numcolors = this->header.colors;

		if(numcolors == 0)
		{
			numcolors = 16;
		}

		if(numcolors > 16)
		{
			this->ready = false;
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		// Read palette

		palette = new uint8_t[numcolors*4];

		if(palette == NULL ||
		   this->readSource(palette, numcolors*4) != (numcolors*4))
		{
			this->ready = false;
			return PKF_ERR_OBJECT_NOT_VALID;
		}
	}

	if(this->header.bpp == 1)
	{
		uint32_t numcolors = this->header.colors;

		if(numcolors == 0)
		{
			numcolors = 2;
		}

		if(numcolors > 2)
		{
			this->ready = false;
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		// Read palette

		palette = new uint8_t[numcolors*4];

		if(palette == NULL ||
		   this->readSource(palette, numcolors*4) != (numcolors*4))
		{
			this->ready = false;
			return PKF_ERR_OBJECT_NOT_VALID;
		}
	}

	//
	// Read image
	//

	uint8_t *r = NULL; rbuffer->lock(&r);
	uint8_t *g = NULL; gbuffer->lock(&g);
	uint8_t *b = NULL; bbuffer->lock(&b);

	if(this->header.compression == BMP_BI_RGB)
	{
		switch(this->header.bpp)
		{
		case 1: // 0 = black, 1 = white
			{
				this->readRGB1(r, g, b, palette);
			}
			break;

		case 4: // 16 colors
			{
				this->readRGB4(r, g, b, palette);
			}
			break;

		case 8: // 256 colors
			{
				this->readRGB8(r, g, b, palette);
			}
			break;

		case 16: // 16 bpp
			{
				this->readRGB16(r, g, b);
			}
			break;

		case 24: // 24 bpp
			{
				this->readRGB24(r, g, b);
			}
			break;

		case 32: // 32 bpp
			{
				uint8_t *a = NULL; 
				
				if(sample->getImage()->getNumberOfChannels() == 4)
				{
					abuffer->lock(&a);
					this->readRGB32(r, g, b, a);
					abuffer->unlock();
				}
			}
			break;

		default:  // do nothing
			{
			}
			break;
		}
	}
	else if(this->header.compression == BMP_BI_RLE8)
	{
		this->readRLE8(r, g, b, palette);
	}
	else if(this->header.compression == BMP_BI_RLE4)
	{
		this->readRLE4(r, g, b, palette);
	}
	else if(this->header.compression == BMP_BI_BITFIELD)
	{
		switch(this->header.bpp)
		{
		case 16: // 16 bpp
			{
				this->readBITFIELDS16(r, g, b);
			}
			break;

		case 32: // 32 bpp
			{
				this->readBITFIELDS32(r, g, b);
			}
			break;

		default:  // do nothing
			{
			}
			break;
		}
	}

	rbuffer->unlock();
	gbuffer->unlock();
	bbuffer->unlock();

	if(palette != NULL)
	{
		delete [] palette;
	}

	PKFSampleManager::unlockSample(currentSampleid);
	this->output->sendSample();

	return PKF_SUCCESS;
}

//
//
//

bool PKFBmpReaderFilter::getNextByte(uint8_t *buffer, 
									 uint32_t *size, 
									 uint32_t *index, 
									 uint8_t *value)
{
	if(*index == *size)
	{
		*size  = this->readSource(buffer, 32768);

		if(*size == 0)
		{
			return false;
		}

		*index = 0;
	}

	*value = buffer[*index];
	(*index)++;

	return true;
}

//
//
//

PKFReturnCode PKFBmpReaderFilter_Create(IPKFFilter **filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = (IPKFFilter *) new PKFBmpReaderFilter();

	if(*filter == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFBmpReaderFilterProperties_Fill(PKFFilterProperties *prop)
{
	if(prop == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	prop->setFilterId("BMP_READER");
	prop->setFilterName(L"Bitmap Image Reader");
	prop->setDescription(L"Reads the Windows Bitmap (BMP) image format.");
	prop->setFilterCategory(PKF_FILTER_CATEGORY_IMAGE);
	prop->setAuthors(L"Damien Chavarria");
	prop->setEmail("roy204@gmail.com");
	prop->setHomepage("http://www.projectketchup.com");
	prop->setVendor(L"TomatoSauce");
	prop->setVersion("1.0");

	return PKF_SUCCESS;
}


