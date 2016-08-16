//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFInputConnector.h"
#include "PKFOutputConnector.h"
#include "IPKFFilter.h"

//
// PKFInputConnector
//

PKFInputConnector::PKFInputConnector(IPKFFilter *parent, 
									 std::string name, 
									 PKFTransferType type)
:IPKFConnector(name, type)
{
	this->parentFilter = parent;
}

PKFInputConnector::~PKFInputConnector()
{
	this->disconnect();
}

PKFConnectorType PKFInputConnector::getType()
{
	return PKF_CONNECTOR_TYPE_INPUT;
}

PKFReturnCode PKFInputConnector::getParentFilter(IPKFFilter **parent)
{
	if(parent == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(this->parentFilter == NULL)
	{
		*parent = NULL;
		return PKF_ERR_NOT_AVAILABLE;
	}

	*parent = this->parentFilter;

	return PKF_SUCCESS;
}

PKFReturnCode PKFInputConnector::connect(IPKFConnector			 *connectTo, 
	  									 IPKFMediaTypeDescriptor *forcedType)
{
	if(connectTo == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(this->connectedTo != NULL || this->currentMediaType != NULL)
	{
		return PKF_ERR_ALREADY_CONNECTED;
	}

	if(connectTo->getType() != PKF_CONNECTOR_TYPE_OUTPUT)
	{
		return PKF_ERR_WRONG_CONNECTOR_TYPE;
	}

	//
	// Check transer type
	//

	switch(this->getTransferType())
	{
	case PKF_TRANSFER_TYPE_PUSH:
		{
			switch(connectTo->getTransferType())
			{
			case PKF_TRANSFER_TYPE_PULL:
				{
					return PKF_ERR_WRONG_TRANSFER_TYPE;
				}
				break;
			default:
				{
				}
				break;
			}
		}
		break;

	case PKF_TRANSFER_TYPE_PULL:
		{
			switch(connectTo->getTransferType())
			{
			case PKF_TRANSFER_TYPE_PUSH:
				{
					return PKF_ERR_WRONG_TRANSFER_TYPE;
				}
				break;
			default:
				{
				}
				break;
			}
		}
		break;
	}

	//
	// let filters know and 
	// check they are ok with this
	//

	if( connectTo->onConnecting(this) != PKF_SUCCESS ||
		this->onConnecting(connectTo) != PKF_SUCCESS )
	{
		this->disconnect();
		return PKF_ERR_CONNECTION_REFUSED;
	}

	//
	// Agree on a transfer type
	//

	if(forcedType != NULL)
	{
		//
		// Try to connect with this Media Type
		//

		this->connectInternal(connectTo, forcedType, forcedType);
	}
	else
	{
		//
		// Enumerate types that match
		//

		for(unsigned int i=0; i < connectTo->getNumberOfMediaTypeDescriptors(); i++)
		{
			for(unsigned int j=0; j < this->getNumberOfMediaTypeDescriptors(); j++)
			{
				IPKFMediaTypeDescriptor *mine = NULL; 
				IPKFMediaTypeDescriptor *his  = NULL; 
				
				PKFReturnCode resM = this->getMediaTypeDescriptor(j, &mine);
				PKFReturnCode resH = connectTo->getMediaTypeDescriptor(i, &his);

				if(resM != PKF_SUCCESS || resH != PKF_SUCCESS)
				{
					return PKF_ERR_NOT_READY;
				}

				//
				// Match types
				//

				if(PKFMediaTypeDescriptorUtility::matchMediaTypes(mine, his))
				{
					this->connectInternal(connectTo, his, mine);
					break;
				}
			}
		
			if(this->currentMediaType != NULL)
			{
				break;
			}
		}
	}

	if(this->currentMediaType == NULL || this->connectedTo == NULL)
	{
		return PKF_ERR_TYPES_NOT_COMPATIBLE;
	}

	PKFReturnCode final = this->validateConnection();

	if(final != PKF_SUCCESS)
	{
		this->disconnect();
		return PKF_ERR_CONNECTION_REFUSED;
	}

	//
	// let filters know and 
	// check they are ok with this
	//

	if( this->connectedTo->onConnected(this) != PKF_SUCCESS ||
		this->onConnected(this->connectedTo) != PKF_SUCCESS )
	{
		this->disconnect();
		return PKF_ERR_CONNECTION_REFUSED;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFInputConnector::onConnecting(IPKFConnector *to)
{
	if(this->parentFilter == NULL)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

	return this->parentFilter->onConnecting(this, to);
}

PKFReturnCode PKFInputConnector::onConnected(IPKFConnector *to)
{
	if(this->parentFilter == NULL)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

	return this->parentFilter->onConnected(this, to);
}

PKFReturnCode PKFInputConnector::onDisconnected()
{
	if(this->parentFilter == NULL)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

	return this->parentFilter->onDisconnected(this);
}

PKFReturnCode PKFInputConnector::requestSample(IPKFSampleRequest *request)
{
	uint32_t sampleId;

	if(this->connectedTo == NULL)
	{
		return PKF_ERR_NOT_CONNECTED;
	}		

	if(request == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(this->connectedTo->getType() != PKF_CONNECTOR_TYPE_OUTPUT)
	{
		return PKF_ERR_NOT_CONNECTED;
	}

	PKFOutputConnector *connector = (PKFOutputConnector *) this->connectedTo;

	PKFReturnCode result = connector->sampleRequested(request, &sampleId); 

	if(result != PKF_SUCCESS)
	{
		this->currentSampleId = 0;
		return PKF_ERR_NOT_AVAILABLE;
	}

	this->currentSampleId = sampleId;

	return result;
}

PKFReturnCode PKFInputConnector::validateConnection()
{
	//
	// The default implementation
	// always accepts the connetion
	//

	return PKF_SUCCESS;
}

PKFReturnCode PKFInputConnector::sampleReceived(uint32_t sampleId)
{
	if(this->currentSampleId != 0)
	{
		PKFSampleManager::deallocateSample(this->currentSampleId);
		this->currentSampleId = 0;
	}

	this->currentSampleId = sampleId;

	if((this->parentFilter != NULL) && 
	   (this->getTransferType() == PKF_TRANSFER_TYPE_PUSH))
	{
		PKVariant *ar = this->parentFilter->get(IPKFFilter::AUTORUN_BOOL_PROPERTY);

		if(PKVALUE_BOOL(ar))
		{
			return this->parentFilter->run();
		}
	}

	return PKF_SUCCESS;
}

