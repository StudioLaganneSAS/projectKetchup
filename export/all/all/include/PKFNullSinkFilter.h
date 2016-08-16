//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKFNULLSINKFILTER_H
#define PKFNULLSINKFILTER_H

#include "IPKFFilter.h"
#include "PKFInputConnector.h"
#include "PKFMediaTypeDescriptorUtility.h"

//
// PKFNullSinkFilter
// 

class PKFNullSinkFilter : public IPKFFilter
{
public:

    PKFNullSinkFilter(PKFTransferType type);
    virtual ~PKFNullSinkFilter();

    //
    // process is implemented to do nothing
    //

    virtual PKFReturnCode process();

protected:
    PKFInputConnector *input;

};

#endif // PKFNULLSINKFILTER_H
