//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef IPKFIMAGETRANSFORMFILTER_H
#define IPKFIMAGETRANSFORMFILTER_H

#include "IPKFFilter.h"
#include "PKFInputConnector.h"
#include "PKFOutputConnector.h"
#include "PKFMediaTypeDescriptorUtility.h"
#include "PKFSampleManager.h"

//
// IPKFImageTransformFilter
//


class IPKFImageTransformFilter : public IPKFFilter
{
public:

    IPKFImageTransformFilter();
    virtual ~IPKFImageTransformFilter();

    //
    // process is implemented to get the current
    // sample from the input pin and call the
    // virtual method transform passing it.
    //

    virtual PKFReturnCode process();

    //
    // Implement the transform operations here
    //

    virtual PKFReturnCode transform(PKFImageSample *inOut)				     = 0;
    virtual PKFReturnCode transform(PKFImageSample *in, PKFImageSample *out) = 0;

protected:
    PKFInputConnector  *input;
    PKFOutputConnector *output;

	static std::string INPLACE_BOOL_PROPERTY;
};

#endif // IPKFIMAGETRANSFORMFILTER_H
