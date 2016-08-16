//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGEMEDIATYPEDESCRIPTOR_H
#define PKFIMAGEMEDIATYPEDESCRIPTOR_H

#include "IPKFMediaTypeDescriptor.h"
#include "PKFImageChannelDescriptor.h"

class PKFImageMediaTypeDescriptor : public IPKFMediaTypeDescriptor {

public:
    PKFImageMediaTypeDescriptor();
    ~PKFImageMediaTypeDescriptor();

	PKFReturnCode addChannelDescriptor(std::string channelDesc, 
									   uint32_t width, 
									   uint32_t height, 
									   PKFImageDataType dataType);

    PKFMediaType getMediaType();

    uint32_t   getNumberOfChannels();

    PKFReturnCode getChannelDescriptor(uint32_t index, 
									   PKFImageChannelDescriptor **channelDesc);

	PKFReturnCode getChannelDescriptor(std::string channelDescriptorName, 
									   PKFImageChannelDescriptor **channelDesc);

private:
    std::vector <PKFImageChannelDescriptor *> channelDescriptors;
};

#endif // PKFIMAGEMEDIATYPEDESCRIPTOR_H
