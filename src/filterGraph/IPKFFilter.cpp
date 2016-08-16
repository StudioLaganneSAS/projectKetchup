//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "IPKFFilter.h"
#include "PKTime.h"

//
// IPKFFilter
//

std::string IPKFFilter::AUTORUN_BOOL_PROPERTY = "autoRun";


IPKFFilter::IPKFFilter()
{
	this->properties  = NULL;
	this->status      = PKF_FILTER_STATUS_IDLE;
	this->parentGraph = NULL;

	PKOBJECT_ADD_BOOL_PROPERTY(autoRun, false);
}

IPKFFilter::~IPKFFilter()
{
	if(this->status == PKF_FILTER_STATUS_RUNNING)
	{
		this->interrupt();
	}

	for(unsigned int i=0; i < this->connectors.size(); i++)
	{
		if(this->connectors[i]->isConnected())
		{
			this->connectors[i]->disconnect();
		}

		this->connectors[i]->release();
	}

	this->connectors.clear();
}

//
// Call run() to have the filter operate
// on its input. Call interrupt() to finish
//  a run() operation early.
//

PKFReturnCode IPKFFilter::run()
{
	this->setStatus(PKF_FILTER_STATUS_RUNNING);

	this->feedback("RUN_START", NULL);

	PKFReturnCode result = this->process();

	this->feedback("RUN_END", NULL);

	this->setStatus(PKF_FILTER_STATUS_IDLE);

	return result;
}

PKFReturnCode IPKFFilter::interrupt()
{
	if(this->getStatus() == PKF_FILTER_STATUS_RUNNING)
	{
		this->setStatus(PKF_FILTER_STATUS_INTERRUPTING);

		while(this->getStatus() != PKF_FILTER_STATUS_IDLE)
		{
			// sleep here
			PKTime::sleep(100);
		}
	}

	return PKF_SUCCESS;
}

//
// Override this in the GUI class to provide
// feedback or interaction() during the run()
// operation.
//

PKFReturnCode IPKFFilter::feedback(std::string info, void *parameter)
{
	return PKF_SUCCESS;
}

PKFReturnCode IPKFFilter::addConnector(IPKFConnector *connector)
{
	if(connector == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	this->connectors.push_back(connector);

	return PKF_SUCCESS;
}

PKFReturnCode IPKFFilter::removeConnector(uint32_t index)
{
	if(index >= this->connectors.size())
	{
		return PKF_ERR_INVALID_PARAM;
	}

	std::vector <IPKFConnector *> new_connectors;

	for(unsigned int i=0; i < this->connectors.size(); i++)
	{
		if(i != index)
		{
			new_connectors.push_back(this->connectors[i]);
		}
	}

	this->connectors = new_connectors;

	return PKF_SUCCESS;
}

uint32_t IPKFFilter::getNumberOfConnectors()
{
	return this->connectors.size();
}

PKFReturnCode IPKFFilter::getConnector(uint32_t index, IPKFConnector **connector)
{
	if(connector == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(index >= this->connectors.size())
	{
		*connector = NULL;
		return PKF_ERR_INVALID_PARAM;
	}

	*connector = this->connectors[index];

	return PKF_SUCCESS;
}

PKFFilterProperties * IPKFFilter::getFilterProperties()
{
	return this->properties;
}

void IPKFFilter::setFilterProperties(PKFFilterProperties *properties)
{
	this->properties = properties;
}

void IPKFFilter::setFilterGraph(PKFFilterGraph  *graph)
{
	this->parentGraph = graph;
}

PKFReturnCode IPKFFilter::getFilterGraph(PKFFilterGraph **graph)
{
	if(graph == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(this->parentGraph == NULL)
	{
		return PKF_ERR_NOT_AVAILABLE;
	}

	*graph = this->parentGraph;

	return PKF_SUCCESS;
}

void IPKFFilter::setStatus(PKFFilterStatus status)
{
	this->status = status;
}

PKFFilterStatus IPKFFilter::getStatus()
{
	return this->status;
}

//
// Default Implementations
//

PKFReturnCode IPKFFilter::onConnecting(IPKFConnector *connector, IPKFConnector *to)
{
	return PKF_SUCCESS;
}

PKFReturnCode IPKFFilter::onConnected(IPKFConnector *connector, IPKFConnector *to)
{
	return PKF_SUCCESS;
}

PKFReturnCode IPKFFilter::onDisconnected(IPKFConnector *connector)
{
	return PKF_SUCCESS;
}
