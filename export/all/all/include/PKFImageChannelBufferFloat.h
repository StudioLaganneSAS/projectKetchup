//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGECHANNELBUFFERFLOAT_H
#define PKFIMAGECHANNELBUFFERFLOAT_H

#include "IPKFImageChannelBuffer.h"
#include "PKFDataBuffer.h"

//
// PKFImageChannelBufferFloat
//

class PKFImageChannelBufferFloat : public IPKFImageChannelBuffer {

public:

    PKFImageChannelBufferFloat(uint32_t width, uint32_t height);
    ~PKFImageChannelBufferFloat();

    PKFReturnCode alloc(PKFDataBufferType type = PKF_DATA_BUFFER_MEMORY);
    PKFReturnCode free();

    bool isValid();

    PKFImageDataType getImageDataType();

    uint32_t getWidth();
    uint32_t getHeight();

    float getPixel(uint32_t x, uint32_t y);
	void  setPixel(uint32_t x, uint32_t y, float value);

    PKFReturnCode lock(float **pixels);
    PKFReturnCode unlock();

private:

    uint32_t       width;
    uint32_t       height;
    PKFDataBuffer *pixels;
};

#endif // PKFIMAGECHANNELBUFFERFLOAT_H
