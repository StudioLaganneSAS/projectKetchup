//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//


#ifndef IPKFSAMPLE_H
#define IPKFSAMPLE_H

#include "PKFTypes.h"
#include "PKCritSec.h"
#include "PKObject.h"

class IPKFSample : public PKObject {

protected:
    IPKFSample();
    virtual ~IPKFSample();

public:
    bool isLocked();

    virtual PKFReturnCode lock();
    virtual PKFReturnCode unlock();

    virtual PKFMediaType getMediaType() = 0;

private:
	PKCritSec mutex;
};

#endif // IPKFSAMPLE_H
