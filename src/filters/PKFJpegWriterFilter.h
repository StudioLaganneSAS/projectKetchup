//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#ifndef PKF_JPEG_WRITER_FILTER_H
#define PKF_JPEG_WRITER_FILTER_H

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

#define JPEG_WRITE_SIZE 32768

typedef struct {

    struct jpeg_destination_mgr  manager;						/* jpeg fields  */
    JOCTET				         buffer[JPEG_WRITE_SIZE];		/* buffer       */
    PKFOutputConnector	        *output;						/* output filter */

} filterDest;


//
// PKFJpegWriterFilter
//

class PKFJpegWriterFilter : public IPKFFilter
{
public:

    PKFJpegWriterFilter();

protected:
    ~PKFJpegWriterFilter();

public:
    PKFReturnCode process();

	PKFReturnCode onConnected(IPKFConnector *connector, IPKFConnector *to);
	PKFReturnCode onDisconnected(IPKFConnector *connector);

public:

	static std::string QUALITY_UINT8_PROPERTY;

private:

	filterDest       jpegDest;
	bool			 ready;

	PKFInputConnector  *input;
	PKFOutputConnector *output;

	PKFImageMediaTypeDescriptor *imgDesc;
	
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr	    jerr;
};

//
// Entry points
//

PKFReturnCode PKFJpegWriterFilter_Create(IPKFFilter **filter);

PKFReturnCode PKFJpegWriterFilterProperties_Fill(PKFFilterProperties *filterProperties);


#endif // PKF_JPEG_WRITER_FILTER_H
