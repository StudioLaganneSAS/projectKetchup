//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "IPKFConnector.h"

//
// IPKFConnector
//

IPKFConnector::IPKFConnector(std::string cname, PKFTransferType type)
{
	this->connectedTo      = NULL;
	this->currentMediaType = NULL;
	this->currentSampleId  = 0;
	this->transferType	   = type;

	PKOBJECT_ADD_STRING_PROPERTY(name, cname);
}

IPKFConnector::~IPKFConnector()
{
	if(this->connectedTo != NULL)
	{
		this->disconnect();
	}

	for(unsigned int i=0; i < this->types.size(); i++)
	{
		IPKFMediaTypeDescriptor *desc = this->types[i];
		
		if(desc != NULL)
		{
			desc->release();
		}
	}

	this->types.clear();
}

PKFReturnCode IPKFConnector::connectInternal(IPKFConnector			 *connectTo, 
											 IPKFMediaTypeDescriptor *his,
											 IPKFMediaTypeDescriptor *ours)
{
	if(this->connectedTo != NULL)
	{
		return PKF_ERR_ALREADY_CONNECTED;
	}

	this->connectedTo	   = connectTo;
	this->currentMediaType = ours;
	this->currentSampleId  = 0;

	this->connectedTo->connectedTo      = this;
	this->connectedTo->currentMediaType = his;
	this->connectedTo->currentSampleId  = 0;

	return PKF_SUCCESS;
}

PKFReturnCode IPKFConnector::disconnect()
{
	if(this->connectedTo == NULL)
	{
		return PKF_ERR_NOT_CONNECTED;
	}

	this->connectedTo->onDisconnected();
	this->onDisconnected();

	this->connectedTo->connectedTo      = NULL;
	this->connectedTo->currentMediaType = NULL;
	this->connectedTo->currentSampleId  = 0;

	this->connectedTo	   = NULL;
	this->currentMediaType = NULL;
	this->currentSampleId  = 0;

	return PKF_SUCCESS;
}

std::string IPKFConnector::getName()
{
	PKVariant *n = this->get("name");

	if(n != NULL)
	{
		return PKVALUE_STRING(n);
	}

	return "";
}

PKFTransferType IPKFConnector::getTransferType()
{
	return this->transferType;
}

bool IPKFConnector::isConnected()
{
	return ((this->connectedTo != NULL) && 
		    (this->currentMediaType != NULL));
}

PKFReturnCode IPKFConnector::getConnectedTo(IPKFConnector **connector)
{
	if(connector == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}	

	if(this->connectedTo == NULL)
	{
		*connector = NULL;
		return PKF_ERR_NOT_CONNECTED;
	}

	*connector = this->connectedTo;

	return PKF_SUCCESS;
}

PKFReturnCode IPKFConnector::getCurrentSampleId(uint32_t *sampleId)
{
	if(sampleId == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(this->currentSampleId == 0)
	{
		*sampleId = 0;
		return PKF_ERR_NOT_AVAILABLE;
	}

	*sampleId = this->currentSampleId;

	return PKF_SUCCESS;
}

uint32_t IPKFConnector::getNumberOfMediaTypeDescriptors()
{
	return this->types.size();
}

PKFReturnCode IPKFConnector::addMediaTypeDescriptor(IPKFMediaTypeDescriptor * desc)
{
	if(desc == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	this->types.push_back(desc);

	return PKF_SUCCESS;
}

PKFReturnCode IPKFConnector::removeMediaTypeDescriptor(uint32_t index)
{
	if(index >= this->types.size())
	{
		return PKF_ERR_INVALID_PARAM;
	}

	std::vector <IPKFMediaTypeDescriptor *> new_types;

	for(unsigned int i=0; i < this->types.size(); i++)
	{
		if(i != index)
		{
			new_types.push_back(this->types[i]);
		}
	}

	this->types = new_types;

	return PKF_SUCCESS;
}

PKFReturnCode IPKFConnector::getMediaTypeDescriptor(uint32_t index, IPKFMediaTypeDescriptor **desc)
{
	if(desc == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(index >= this->types.size())
	{
		*desc = NULL;
		return PKF_ERR_INVALID_PARAM;
	}

	*desc = this->types[index];

	return PKF_SUCCESS;
}

PKFReturnCode IPKFConnector::setCurrentMediaType(IPKFMediaTypeDescriptor * desc)
{
	this->currentMediaType = desc;
	return PKF_SUCCESS;
}

PKFReturnCode IPKFConnector::getCurrentMediaType(IPKFMediaTypeDescriptor **desc)
{
	if(desc == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(this->currentMediaType == NULL)
	{
		*desc = NULL;
		return PKF_ERR_NOT_CONNECTED;
	}

	*desc = this->currentMediaType;

	return PKF_SUCCESS;
}

