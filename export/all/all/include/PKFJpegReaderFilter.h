//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#ifndef PKF_JPEG_READER_FILTER_H
#define PKF_JPEG_READER_FILTER_H

#include "IPKFFilter.h"
#include "PKFInputConnector.h"
#include "PKFOutputConnector.h"
#include "PKFSampleManager.h"
#include "PKFSampleAutoLock.h"
#include "PKFDataSampleRequest.h"
#include "PKFImageSampleRequest.h"
#include "PKFMediaTypeDescriptorUtility.h"

#ifdef WIN32
#include <windows.h>
#endif

// libjpeg

#ifdef __cplusplus

extern "C" {

#include "jpeglib.h"
#include "jerror.h"

}

#endif

//
// Local stuff
//

#define JPEG_BUFFER_SIZE 32768

typedef struct {

    struct jpeg_source_mgr  manager;						/* jpeg fields  */
    JOCTET				    buffer[JPEG_BUFFER_SIZE];		/* buffer       */
    PKFInputConnector	   *input;							/* input filter */

} filterSource;

//
// PKFJpegReaderFilter
//

class PKFJpegReaderFilter : public IPKFFilter
{
public:

    PKFJpegReaderFilter();

protected:
    ~PKFJpegReaderFilter();

public:
    PKFReturnCode process();

	PKFReturnCode onConnected(IPKFConnector *connector, IPKFConnector *to);
	PKFReturnCode onDisconnected(IPKFConnector *connector);

private:

	filterSource     jpegSource;
	bool			 ready;

	PKFInputConnector  *input;
	PKFOutputConnector *output;

	PKFImageMediaTypeDescriptor *imgDesc;
	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr		  jerr;
};

//
// Entry points
//

PKFReturnCode PKFJpegReaderFilter_Create(IPKFFilter **filter);
PKFReturnCode PKFJpegReaderFilterProperties_Fill(PKFFilterProperties *filterProperties);


#endif // PKF_JPEG_READER_FILTER_H


