//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKFOUTPUTCONNECTOR_H
#define PKFOUTPUTCONNECTOR_H

#include "IPKFConnector.h"
#include "IPKFSample.h"
#include "IPKFSampleRequest.h"
#include "PKFMediaTypeDescriptorUtility.h"

class IPKFFilter;

//
// PKFOutputConnector
// 

class PKFOutputConnector : public IPKFConnector
{
public:
	PKFOutputConnector(IPKFFilter *parent, 
					   std::string name, 
					   PKFTransferType type);

protected:
    ~PKFOutputConnector();

public:
    PKFConnectorType getType();
    PKFReturnCode    getParentFilter(IPKFFilter **parent);

    PKFReturnCode    connect(IPKFConnector			 *connectTo, 
						     IPKFMediaTypeDescriptor *forcedType = NULL);

	virtual PKFReturnCode    onConnecting(IPKFConnector *to);
	virtual PKFReturnCode    onConnected(IPKFConnector *to);
	virtual PKFReturnCode    onDisconnected();


    //
    // getSample() called to allocate a sample
    // to be sent to downstream filter...
    //

    virtual PKFReturnCode getSample(IPKFMediaTypeDescriptor *desc, uint32_t *sampleId);
    
    //
    // forceSampleId() to be called to
    // force an id and avoid
    // allocation (sample pass through)
    //

    PKFReturnCode forceSampleId(uint32_t sampleId);
    
    //
    // sendSample() can be called to deliver the
    // sample to the downstream connector when
    // the connection operates in PUSH mode.
    //

    PKFReturnCode sendSample();

    //
    // sampleRequested() can be called by the connected
    // downstream input pin when it requests data in PULL mode.
    //

    virtual PKFReturnCode sampleRequested(IPKFSampleRequest *request, uint32_t *sampleId);

private:

    IPKFFilter *parentFilter;
    bool		ownSample;
};

#endif // PKFOUTPUTCONNECTOR_H
