//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGECHANNELBUFFERFACTORY_H
#define PKFIMAGECHANNELBUFFERFACTORY_H

//
// PKFImageChannelBufferFactory
//

#include "IPKFImageChannelBuffer.h"
#include "PKFImageChannelBuffer8Bit.h"
#include "PKFImageChannelBuffer16Bit.h"
#include "PKFImageChannelBufferFloat.h"

class PKFImageChannelBufferFactory 
{
public:    

    static PKFReturnCode createImageChannelBuffer(uint32_t width, 
												  uint32_t height, 
												  PKFImageDataType type, 
												  IPKFImageChannelBuffer **buffer,
												  PKFDataBufferType btype = PKF_DATA_BUFFER_MEMORY);
private:    

};

#endif // PKFIMAGECHANNELBUFFERFACTORY_H
