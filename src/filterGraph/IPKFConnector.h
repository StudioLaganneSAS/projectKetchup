//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef IPKFCONNECTOR_H
#define IPKFCONNECTOR_H

#include "PKFTypes.h"
#include "PKObject.h"
#include "IPKFMediaTypeDescriptor.h"

//
// IPKFConnector
//

class IPKFConnector : public PKObject
{
public:

	IPKFConnector(std::string name, 
				  PKFTransferType type);

    virtual PKFConnectorType getType() = 0;

    virtual PKFReturnCode connect(IPKFConnector		     *connectTo, 
						          IPKFMediaTypeDescriptor *forcedType = NULL) = 0;
    
	virtual PKFReturnCode disconnect();

	virtual PKFReturnCode onConnecting(IPKFConnector *to) = 0;
	virtual PKFReturnCode onConnected(IPKFConnector *to)  = 0;
	virtual PKFReturnCode onDisconnected()                = 0;

	std::string           getName();
    PKFTransferType       getTransferType();
    PKFReturnCode         getConnectedTo(IPKFConnector **connector);
    PKFReturnCode         getCurrentSampleId(uint32_t *sampleId);
	bool				  isConnected();

    uint32_t              getNumberOfMediaTypeDescriptors();

    PKFReturnCode         addMediaTypeDescriptor(IPKFMediaTypeDescriptor * desc);
    PKFReturnCode         removeMediaTypeDescriptor(uint32_t index);

    PKFReturnCode         getMediaTypeDescriptor(uint32_t index, IPKFMediaTypeDescriptor **desc);

    PKFReturnCode         setCurrentMediaType(IPKFMediaTypeDescriptor * desc);
    PKFReturnCode         getCurrentMediaType(IPKFMediaTypeDescriptor **desc);

private:
	std::vector <IPKFMediaTypeDescriptor *> types;
    PKFTransferType transferType;

protected:

    virtual ~IPKFConnector();

    IPKFConnector           *connectedTo;
    uint32_t                 currentSampleId;
    IPKFMediaTypeDescriptor *currentMediaType;

    PKFReturnCode connectInternal(IPKFConnector		      *connectTo, 
								  IPKFMediaTypeDescriptor *his,
							      IPKFMediaTypeDescriptor *ours);

};

#endif // IPKFCONNECTOR_H
