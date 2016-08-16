//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKF_FILE_SOURCE_FILTER_H
#define PKF_FILE_SOURCE_FILTER_H

#include "IPKFFilter.h"
#include "PKFOutputConnector.h"
#include "PKFSampleManager.h"
#include "PKFSampleAutoLock.h"
#include "PKFDataSampleRequest.h"
#include "PKFCustomSampleRequest.h"
#include "PKFile.h"

//
// PKFFileSourceOutput
//

class PKFFileSourceOutput : public PKFOutputConnector
{
public:
	
	PKFFileSourceOutput(IPKFFilter *parent, 
						std::string name);

protected:
    ~PKFFileSourceOutput();

public:
    void setFile(PKFile *file);

    PKFReturnCode sampleRequested(IPKFSampleRequest *request, 
								  uint32_t *sampleId);

private:
	PKFile *input;
};

//
// PKFFileSourceFilter
//

class PKFFileSourceFilter : public IPKFFilter
{
public:

    PKFFileSourceFilter();

protected:
    ~PKFFileSourceFilter();

public:
    PKFReturnCode onConnected(IPKFConnector *connector,
							  IPKFConnector *to);

	PKFReturnCode onDisconnected(IPKFConnector *connector);

    PKFReturnCode process();

public:

	static std::string FILENAME_WSTRING_PROPERTY;

private:
	PKFFileSourceOutput *output;
	PKFile              *input;
};

//
// Entry Points
//

PKFReturnCode PKFFileSourceFilter_Create(IPKFFilter **filter);
PKFReturnCode PKFFileSourceFilterProperties_Fill(PKFFilterProperties *filterProperties);


#endif // PKF_FILE_SOURCE_FILTER_H


