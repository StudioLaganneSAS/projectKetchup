//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKF_BMP_READER_FILTER_H
#define PKF_BMP_READER_FILTER_H

#include "IPKFFilter.h"
#include "PKFInputConnector.h"
#include "PKFOutputConnector.h"
#include "PKFSampleManager.h"
#include "PKFSampleAutoLock.h"
#include "PKFDataSampleRequest.h"
#include "PKFImageSampleRequest.h"
#include "PKFMediaTypeDescriptorUtility.h"

//
// bmpHeader_t
//

typedef struct bmpHeader_s {

	uint32_t   fileSize;
	uint32_t   reserved;
	uint32_t   dataStart;
	uint32_t   unused;
	uint32_t   width;
	int32_t    height;
	uint16_t   planes;
	uint16_t   bpp;
	uint32_t   compression;
	uint32_t   datasize;
	uint32_t   xres;

	uint32_t   yres;
	uint32_t   colors;
	uint32_t   impColors;

} bmpHeader_t;

//
// PKFBmpReaderFilter
//

class PKFBmpReaderFilter : public IPKFFilter
{
public:

    PKFBmpReaderFilter();

    PKFReturnCode process();

	PKFReturnCode onConnected(IPKFConnector *connector, IPKFConnector *to);
	PKFReturnCode onDisconnected(IPKFConnector *connector);

protected:

    ~PKFBmpReaderFilter();

private:

	// Color-specific read routines

	void readRGB1(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette);
	void readRGB4(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette);
	void readRGB8(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette);
	
	void readRGB16(uint8_t *r, uint8_t *g, uint8_t *b);
	void readRGB24(uint8_t *r, uint8_t *g, uint8_t *b);
	void readRGB32(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a);

	void readRLE4(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette);
	void readRLE8(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *palette);

	void readBITFIELDS16(uint8_t *r, uint8_t *g, uint8_t *b);
	void readBITFIELDS32(uint8_t *r, uint8_t *g, uint8_t *b);

	// Byte reading

	uint32_t readSource(uint8_t *buffer, 
						uint32_t size);
	
	bool     getNextByte(uint8_t *buffer, 
						 uint32_t *size, 
						 uint32_t *index, 
						 uint8_t *value);

	bool ready;

	PKFInputConnector  *input;
	PKFOutputConnector *output;

	bmpHeader_t header;

	PKFImageMediaTypeDescriptor *imgDesc;
};

//
// Entry Points
//

PKFReturnCode PKFBmpReaderFilter_Create(IPKFFilter **filter);
PKFReturnCode PKFBmpReaderFilterProperties_Fill(PKFFilterProperties *filterProperties);


#endif // PKF_BMP_READER_FILTER_H


