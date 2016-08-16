//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGECHANNELBUFFER8BIT_H
#define PKFIMAGECHANNELBUFFER8BIT_H

#include "IPKFImageChannelBuffer.h"
#include "PKFDataBuffer.h"

//
// PKFImageChannelBuffer8Bit
//

class PKFImageChannelBuffer8Bit : public IPKFImageChannelBuffer 
{
public:    

    PKFImageChannelBuffer8Bit(uint32_t width, uint32_t height);
    ~PKFImageChannelBuffer8Bit();

    PKFReturnCode alloc(PKFDataBufferType type = PKF_DATA_BUFFER_MEMORY);
    PKFReturnCode free();

    bool isValid();

    PKFImageDataType getImageDataType();

    uint32_t getWidth();
    uint32_t getHeight();

    uint8_t  getPixel(uint32_t x, uint32_t y);
	void	 setPixel(uint32_t x, uint32_t y, uint8_t value);

    PKFReturnCode lock(uint8_t **pixels);
    PKFReturnCode unlock();

private:

    uint32_t       width;
    uint32_t       height;
    PKFDataBuffer *pixels;
};

#endif // PKFIMAGECHANNELBUFFER8BIT_H
