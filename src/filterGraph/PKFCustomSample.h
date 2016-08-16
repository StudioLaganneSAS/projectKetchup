//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFCUSTOMSAMPLE_H
#define PKFCUSTOMSAMPLE_H

#include "IPKFSample.h"

//
// PKFCustomSample
//

class PKFCustomSample : public IPKFSample 
{
public:

    PKFCustomSample(std::string identifier);

protected:
    ~PKFCustomSample();

public:
    PKFMediaType getMediaType();

	static std::string TYPE_IDENTIFIER_PROPERTY;
};

#endif // PKFCUSTOMSAMPLE_H
