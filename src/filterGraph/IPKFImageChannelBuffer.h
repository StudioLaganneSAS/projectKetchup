//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
// 

#ifndef IPKFIMAGECHANNELBUFFER_H
#define IPKFIMAGECHANNELBUFFER_H

#include "PKFTypes.h"

//
// IPKFImageChannelBuffer
//

class IPKFImageChannelBuffer
{    
public:
    virtual ~IPKFImageChannelBuffer();

    virtual PKFImageDataType getImageDataType() = 0;

    virtual uint32_t getWidth()  = 0;
    virtual uint32_t getHeight() = 0;

    virtual PKFReturnCode alloc(PKFDataBufferType type = PKF_DATA_BUFFER_MEMORY) = 0;
    virtual PKFReturnCode free()												 = 0;

    virtual bool isValid() = 0;

};

#endif //IIMAGEBUFFER_H
