//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef IPKFMEDIATYPEDESCRIPTOR_H
#define IPKFMEDIATYPEDESCRIPTOR_H

#include "PKObject.h"
#include "PKFTypes.h"

//
// IPKFMediaTypeDescriptor
// -----------------------
//
// Defines the interface for a media type
// descriptor (used by PKFConnectors).
// Classes that inherit must implement
// getMediaType() to return the correct type.
//

class IPKFMediaTypeDescriptor : public PKObject 
{
public:
    virtual PKFMediaType getMediaType() = 0;

protected:
    virtual ~IPKFMediaTypeDescriptor();
};

#endif // IPKFMEDIATYPEDESCRIPTOR_H
