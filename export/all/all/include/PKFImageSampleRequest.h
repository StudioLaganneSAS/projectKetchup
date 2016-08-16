//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKFIMAGESAMPLEREQUEST_H
#define PKFIMAGESAMPLEREQUEST_H

#include "IPKFSampleRequest.h"

class PKFImageSampleRequest : public IPKFSampleRequest {
public:
    PKFMediaType getMediaType();

protected:
    ~PKFImageSampleRequest();
};

#endif // PKFIMAGESAMPLEREQUEST_H
