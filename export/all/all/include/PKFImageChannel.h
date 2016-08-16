//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGECHANNEL_H
#define PKFIMAGECHANNEL_H

#include "IPKFImageChannelBuffer.h"
#include "PKFImageChannelBufferFactory.h"

//
// PKFImageChannel
//

class PKFImageChannel 
{
public:

	PKFImageChannel(std::string typeDesc);
    ~PKFImageChannel();

    PKFReturnCode alloc(uint32_t width, 
					    uint32_t height, 
					    PKFImageDataType  dataType,
					    PKFDataBufferType bufferType = PKF_DATA_BUFFER_MEMORY);
    PKFReturnCode free();

    bool isValid();

	std::string   getTypeDesc();
    PKFReturnCode getChannelBuffer(IPKFImageChannelBuffer **buffer);

private:    
    IPKFImageChannelBuffer* buffer;
	std::string typeDesc;

public:
	static std::string PKF_TYPE_RGB_RED;
    static std::string PKF_TYPE_RGB_GREEN;
    static std::string PKF_TYPE_RGB_BLUE;
    static std::string PKF_TYPE_GRAYSCALE;
    static std::string PKF_TYPE_ALPHA;
    static std::string PKF_TYPE_CYMK_CYAN;
    static std::string PKF_TYPE_CYMK_YELLOW;
    static std::string PKF_TYPE_CYMK_MAGENTA;
    static std::string PKF_TYPE_CYMK_KHOL;
};

#endif // PKFIMAGECHANNEL_H
