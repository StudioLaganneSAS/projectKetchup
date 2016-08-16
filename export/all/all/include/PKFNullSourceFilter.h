//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKFNULLSOURCEFILTER_H
#define PKFNULLSOURCEFILTER_H

#include "IPKFFilter.h"
#include "PKFOutputConnector.h"
#include "PKFMediaTypeDescriptorUtility.h"

//
// PKFNullSourceFilter
// 

class PKFNullSourceFilter : public IPKFFilter
{
public:

    PKFNullSourceFilter(PKFTransferType type);
    virtual ~PKFNullSourceFilter();

    //
    // process() is implemented to do nothing
    //

    virtual PKFReturnCode process();

protected:
    PKFOutputConnector *output;
};

#endif // PKFNULLSOURCEFILTER_H
