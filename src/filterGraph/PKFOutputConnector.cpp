//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFOutputConnector.h"
#include "PKFInputConnector.h"
#include "IPKFFilter.h"

//
// PKFOutputConnector
//

PKFOutputConnector::PKFOutputConnector(IPKFFilter *parent, 
									   std::string name, 
									   PKFTransferType type)
:IPKFConnector(name, type)
{
	this->parentFilter = parent;
    this->ownSample    = true;
}

PKFOutputConnector::~PKFOutputConnector()
{
	this->disconnect();
}

PKFConnectorType PKFOutputConnector::getType()
{
	return PKF_CONNECTOR_TYPE_OUTPUT;
}

PKFReturnCode PKFOutputConnector::getParentFilter(IPKFFilter **parent)
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

PKFReturnCode PKFOutputConnector::connect(IPKFConnector			  *connectTo, 
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

	if(connectTo->getType() != PKF_CONNECTOR_TYPE_INPUT)
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
			}
		}
		break;
	}

	//
	// let filters know and 
	// check they are ok with this
	//

	if( this->onConnecting(connectTo) != PKF_SUCCESS ||
		connectTo->onConnecting(this) != PKF_SUCCESS )
	{
		this->disconnect();
		return PKF_ERR_CONNECTION_REFUSED;
	}
	//
	// Agreed on a transfer type
	//

	if(forcedType != NULL)
	{
		this->connectInternal(connectTo, forcedType, forcedType);
	}
	else
	{
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

	PKFReturnCode final = ((PKFInputConnector *)this->connectedTo)->validateConnection();

	if(final != PKF_SUCCESS)
	{
		this->disconnect();
		return PKF_ERR_CONNECTION_REFUSED;
	}

	//
	// let filters know and 
	// check they are ok with this
	//

	if( this->onConnected(this->connectedTo) != PKF_SUCCESS ||
		this->connectedTo->onConnected(this) != PKF_SUCCESS )
	{
		this->disconnect();
		return PKF_ERR_CONNECTION_REFUSED;
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFOutputConnector::onConnecting(IPKFConnector *to)
{
	if(this->parentFilter == NULL)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

	return this->parentFilter->onConnecting(this, to);
}

PKFReturnCode PKFOutputConnector::onConnected(IPKFConnector *to)
{
	if(this->parentFilter == NULL)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

	return this->parentFilter->onConnected(this, to);
}

PKFReturnCode PKFOutputConnector::onDisconnected()
{
	if(this->parentFilter == NULL)
	{
		return PKF_ERR_OBJECT_NOT_VALID;
	}

    // deallocate current sample if any

    if(this->currentSampleId != 0)
    {
        PKFReturnCode result = PKFSampleManager::deallocateSample(this->currentSampleId);

        if(result != PKF_SUCCESS)
        {
            return result;
        }
    }

	return this->parentFilter->onDisconnected(this);
}

PKFReturnCode PKFOutputConnector::getSample(IPKFMediaTypeDescriptor *desc, uint32_t *sampleId)
{
    if(desc == NULL || sampleId == NULL)
    {
        return PKF_ERR_INVALID_PARAM;
    }

    if(this->currentSampleId != 0 && this->ownSample)
    {
        PKFReturnCode result = PKFSampleManager::deallocateSample(this->currentSampleId);

        if(result != PKF_SUCCESS)
        {
            return result;
        }
    }
       
    this->currentSampleId = 0;

    PKFReturnCode result = PKFSampleManager::allocateSample(desc, &this->currentSampleId);

    if(result != PKF_SUCCESS)
    {
        return result;
    }

    *sampleId = this->currentSampleId;
    this->ownSample = true;

    return PKF_SUCCESS;
}

PKFReturnCode PKFOutputConnector::forceSampleId(uint32_t sampleId)
{
    if(this->currentSampleId != 0 && this->ownSample)
    {
        PKFReturnCode result = PKFSampleManager::deallocateSample(this->currentSampleId);

        if(result != PKF_SUCCESS)
        {
            return result;
        }
    }
    
    this->currentSampleId = sampleId;
    this->ownSample = false;

    return PKF_SUCCESS;
}

PKFReturnCode PKFOutputConnector::sendSample()
{
	if(this->connectedTo == NULL)
	{
		return PKF_ERR_NOT_CONNECTED;
	}

	if(this->connectedTo->getType() != PKF_CONNECTOR_TYPE_INPUT)
	{
		return PKF_ERR_WRONG_CONNECTOR_TYPE;
	}

    if(this->currentSampleId == 0)
    {
        return PKF_ERR_NOT_READY;
    }

	PKFInputConnector *input = (PKFInputConnector *) this->connectedTo;

	return input->sampleReceived(this->currentSampleId);
}

PKFReturnCode PKFOutputConnector::sampleRequested(IPKFSampleRequest *request, uint32_t *sampleId)
{
	return PKF_ERR_NOT_AVAILABLE;
}

