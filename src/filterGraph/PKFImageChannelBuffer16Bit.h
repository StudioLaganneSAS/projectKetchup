//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGECHANNELBUFFER16BIT_H
#define PKFIMAGECHANNELBUFFER16BIT_H

#include "IPKFImageChannelBuffer.h"
#include "PKFDataBuffer.h"

//
// PKFImageChannelBuffer16Bit
//

class PKFImageChannelBuffer16Bit : public IPKFImageChannelBuffer
{
public:

    PKFImageChannelBuffer16Bit(uint32_t width, uint32_t height);
    ~PKFImageChannelBuffer16Bit();

    PKFReturnCode alloc(PKFDataBufferType type = PKF_DATA_BUFFER_MEMORY);
    PKFReturnCode free();

    bool isValid();

    PKFImageDataType getImageDataType();

    uint32_t getWidth();
    uint32_t getHeight();

    uint16_t  getPixel(uint32_t x, uint32_t y);
	void	 setPixel(uint32_t x, uint32_t y, uint16_t value);

    PKFReturnCode lock(uint16_t **pixels);
    PKFReturnCode unlock();

private:

    uint32_t       width;
    uint32_t       height;
    PKFDataBuffer *pixels;

};

#endif // PKFIMAGECHANNELBUFFER16BIT_H
