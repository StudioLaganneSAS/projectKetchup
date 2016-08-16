//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGECHANNELDESCRIPTOR_H
#define PKFIMAGECHANNELDESCRIPTOR_H

#include "PKFTypes.h"
#include <string>

class PKFImageChannelDescriptor 
{
public:
    PKFImageChannelDescriptor(std::string desc, 
							  uint32_t width, 
							  uint32_t height, 
							  PKFImageDataType type);

    ~PKFImageChannelDescriptor();

    std::string      getDescription();
    uint32_t         getWidth();
    uint32_t         getHeight();
    PKFImageDataType getImageDataType();

private:
    std::string      description;
    uint32_t         width;
    uint32_t         height;
    PKFImageDataType imageDataType;
};

#endif // PKFIMAGECHANNELDESCRIPTOR_H
