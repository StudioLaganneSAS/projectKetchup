//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKFSAMPLEAUTOLOCK_H
#define PKFSAMPLEAUTOLOCK_H

#include "IPKFSample.h"
#include "PKFSampleManager.h"

class PKFSampleAutoLock 
{
public:
    PKFSampleAutoLock(uint32_t sampleId);
    ~PKFSampleAutoLock();

    IPKFSample *getSample();

private:
    IPKFSample *sample;
	uint32_t  sampleId;
};

#endif // PKFSAMPLEAUTOLOCK_H
