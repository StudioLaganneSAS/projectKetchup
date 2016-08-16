//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGE_H
#define PKFIMAGE_H

#include "PKFImageChannel.h"

//
// PKFImage
//

class PKFImage
{
public:

	PKFImage(std::string name = "");
    ~PKFImage();

    std::string getName();

	PKFReturnCode addChannel(std::string typeDesc, 
							 uint32_t width, 
							 uint32_t height, 
							 PKFImageDataType type,
							 PKFDataBufferType bufferType = PKF_DATA_BUFFER_MEMORY);

    uint8_t getNumberOfChannels();

    PKFReturnCode getChannel(uint8_t index, 
							 PKFImageChannel **channel);

	PKFReturnCode getChannel(std::string channelDesc, 
							 PKFImageChannel **channel);

	PKFReturnCode deleteChannel(uint8_t index);

	// helpers

	uint32_t   getWidth();
	uint32_t   getHeight();

	PKFImageDataType getImageDataType();

private:
    std::string	name;
    std::vector <PKFImageChannel *> channels;
};

#endif // PKFIMAGE_H
