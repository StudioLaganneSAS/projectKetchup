//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKFINPUTCONNECTOR_H
#define PKFINPUTCONNECTOR_H

#include "IPKFConnector.h"
#include "IPKFSample.h"
#include "IPKFSampleRequest.h"
#include "PKFSampleManager.h"
#include "PKFMediaTypeDescriptorUtility.h"

class IPKFFilter;

//
// PKFInputConnector
// 

class PKFInputConnector : public IPKFConnector 
{
public:
    PKFInputConnector(IPKFFilter	 *parent, 
					  std::string     name, 
					  PKFTransferType type);

protected:
    ~PKFInputConnector();

public:
    PKFConnectorType getType();
    PKFReturnCode    getParentFilter(IPKFFilter **parent);

    PKFReturnCode    connect(IPKFConnector			 *connectTo, 
						     IPKFMediaTypeDescriptor *forcedType = NULL);

	virtual PKFReturnCode    onConnecting(IPKFConnector *to);
	virtual PKFReturnCode    onConnected(IPKFConnector *to);
	virtual PKFReturnCode    onDisconnected();

    //
    // requestSample() can be called to read a sample
    // when the connector operates in PULL mode. It will
    // call sampleRequested() on the connected output
    // conenctor and keep the sampleId if it gets one
    //

    PKFReturnCode requestSample(IPKFSampleRequest *request);

    //
    // validateConnection() will be called by
    // connect() after the connection has been
    // made. Throught this the filter can validate
    // or invalidate the connection in a custom
    // fashion, for example by querying the connected
    // filter's type or by trying to read a sample.
    //

    virtual PKFReturnCode validateConnection();

    //
    // sampleReceived() will be called when a new sample
    // arrives at the input of the filter. It will then be
    // stored, until needed by the filter.
    // This will only be called if the connector's transfer
    // method is PUSH (i.e it receives pushed samples).
    //

    virtual PKFReturnCode sampleReceived(uint32_t sampleId);

private:
    IPKFFilter *parentFilter;
};

#endif // PKFINPUTCONNECTOR_H
