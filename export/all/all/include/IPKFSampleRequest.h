//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef IPKFSAMPLEREQUEST_H
#define IPKFSAMPLEREQUEST_H

#include "PKFTypes.h"
#include "PKObject.h"

class IPKFSampleRequest : public PKObject
{
public:

    virtual PKFMediaType getMediaType() = 0;

protected:

    virtual ~IPKFSampleRequest();
};

#endif // IPKFSAMPLEREQUEST_H
