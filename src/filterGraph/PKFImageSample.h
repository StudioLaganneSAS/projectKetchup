//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFIMAGESAMPLE_H
#define PKFIMAGESAMPLE_H

#include "IPKFSample.h"
#include "PKFImage.h"

//
// PKFImageSample
//

class PKFImageSample : public IPKFSample {

public:
    PKFImageSample();

protected:
    ~PKFImageSample();

public:
    PKFMediaType getMediaType();

    PKFImage *getImage();
    void      setImage(PKFImage *image);

private:
    PKFImage *image;
};

#endif // PKFIMAGESAMPLE_H
