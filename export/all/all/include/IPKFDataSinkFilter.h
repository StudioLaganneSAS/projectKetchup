//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef IPKFDATASINKFILTER_H
#define IPKFDATASINKFILTER_H

#include "IPKFFilter.h"
#include "PKFInputConnector.h"
#include "PKFSampleManager.h"

//
// IPKFDataSinkFilter
//

class IPKFDataSinkFilter : public IPKFFilter
{
public:

    IPKFDataSinkFilter();
    virtual ~IPKFDataSinkFilter();

    //
    // process is implemented to get the current
    // sample from the input pin and call the
    // virtual method transform passing it.
    //

    virtual PKFReturnCode process();

    //
    // Reimplement the custom sample push operation here
	// if you need to...
    //

    virtual PKFReturnCode pushCustom(PKFCustomSample *sample);

    //
    // Implement the data push operation here
    //

    virtual PKFReturnCode push(PKFDataSample *sample) = 0;

protected:
    PKFInputConnector *input;
};

#endif // IPKFDATASINKFILTER_H
