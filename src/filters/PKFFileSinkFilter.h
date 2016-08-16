//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#ifndef PKF_FILE_SINK_FILTER_H
#define PKF_FILE_SINK_FILTER_H

#include "IPKFDataSinkFilter.h"
#include "PKFDataSampleRequest.h"
#include "PKFile.h"

//
// PKFFileSinkFilter
//

class PKFFileSinkFilter : public IPKFDataSinkFilter
{
public:

    PKFFileSinkFilter();

protected:
    ~PKFFileSinkFilter();

public:
	PKFReturnCode onConnected(IPKFConnector *connector, IPKFConnector *to);
	PKFReturnCode onDisconnected(IPKFConnector *connector);

    PKFReturnCode push(PKFDataSample *sample);
    PKFReturnCode pushCustom(PKFCustomSample *sample);

public:

	static std::string FILENAME_WSTRING_PROPERTY;

private:
	PKFile *output;
};

//
// Entry Points
//

PKFReturnCode PKFFileSinkFilter_Create(IPKFFilter **filter);
PKFReturnCode PKFFileSinkFilterProperties_Fill(PKFFilterProperties *filterProperties);

#endif // PKF_FILE_SOURCE_FILTER_H


