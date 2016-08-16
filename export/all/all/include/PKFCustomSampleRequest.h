//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKFCUSTOMSAMPLEREQUEST_H
#define PKFCUSTOMSAMPLEREQUEST_H

#include "IPKFSampleRequest.h"

//
// PKFCustomSampleRequest
//

class PKFCustomSampleRequest : public IPKFSampleRequest
{
public:
	PKFCustomSampleRequest(std::string typeDesc);

protected:
    ~PKFCustomSampleRequest();

public:
    PKFMediaType getMediaType();

public:

	static std::string TYPE_IDENTIFIER_PROPERTY;
};

#endif // PKFCUSTOMSAMPLEREQUEST_H
