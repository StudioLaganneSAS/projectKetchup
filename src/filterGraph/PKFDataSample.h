//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFDATASAMPLE_H
#define PKFDATASAMPLE_H

#include "IPKFSample.h"
#include "PKFDataBuffer.h"

class PKFDataSample : public IPKFSample {

public:
    PKFDataSample();

protected:
    ~PKFDataSample();

public:
    PKFMediaType getMediaType();

    PKFDataBuffer *getBuffer();
    void           setBuffer(PKFDataBuffer *buffer);

private:
    PKFDataBuffer *buffer;
};

#endif // PKFDATASAMPLE_H
