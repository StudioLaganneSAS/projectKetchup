//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKFDATASAMPLEREQUEST_H
#define PKFDATASAMPLEREQUEST_H

#include "IPKFSampleRequest.h"

//
// PKFDataSampleRequest
//

class PKFDataSampleRequest : public IPKFSampleRequest 
{
public:

    PKFDataSampleRequest(uint32_t size);

protected:
    ~PKFDataSampleRequest();

public:
    PKFMediaType getMediaType();
    uint32_t getSize();

private:
    uint32_t size;
};

#endif //DATASAMPLEREQUEST_H
