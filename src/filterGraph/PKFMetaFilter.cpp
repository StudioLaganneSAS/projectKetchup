//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFMetaFilter.h"

//
// PKFMetaFilter
// 

PKFMetaFilter::PKFMetaFilter()
{

}

PKFMetaFilter::~PKFMetaFilter()
{

}

// input

PKFReturnCode PKFMetaFilter::registerInputFilter(IPKFFilter *filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	for(uint32_t i=0; i < this->inputFilters.size(); i++)
	{
		if(this->inputFilters[i] == filter)
		{
			return PKF_ERR_ALREADY_REGISTERED;
		}
	}

	this->inputFilters.push_back(filter);

	return PKF_SUCCESS;
}

uint32_t PKFMetaFilter::getNumberOfInputFilters()
{
	return this->inputFilters.size();
}

PKFReturnCode PKFMetaFilter::getInputFilter(uint32_t index, IPKFFilter **filter)
{
	if(filter == NULL || index >= this->getNumberOfInputFilters())
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = this->inputFilters[index];

	return PKF_SUCCESS;
}

PKFReturnCode PKFMetaFilter::unregisterInputFilter(uint32_t index)
{
	std::vector <IPKFFilter *> temp;

	for(uint32_t i=0; i < this->inputFilters.size(); i++)
	{
		if(i != index)
		{
			temp.push_back(this->inputFilters[i]);
		}
	}

	this->inputFilters = temp;

	return PKF_SUCCESS;
}

PKFReturnCode PKFMetaFilter::unregisterInputFilter(IPKFFilter *filter)
{
	std::vector <IPKFFilter *> temp;

	for(uint32_t i=0; i < this->inputFilters.size(); i++)
	{
		if(this->inputFilters[i] != filter)
		{
			temp.push_back(this->inputFilters[i]);
		}
	}

	this->inputFilters = temp;

	return PKF_SUCCESS;
}
	
PKFReturnCode PKFMetaFilter::clearInputFilters()
{
	this->inputFilters.clear();
	return PKF_SUCCESS;
}

// output

PKFReturnCode PKFMetaFilter::registerOutputFilter(IPKFFilter *filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	for(uint32_t i=0; i < this->outputFilters.size(); i++)
	{
		if(this->outputFilters[i] == filter)
		{
			return PKF_ERR_ALREADY_REGISTERED;
		}
	}

	this->outputFilters.push_back(filter);

	return PKF_SUCCESS;
}

uint32_t PKFMetaFilter::getNumberOfOutputFilters()
{
	return this->outputFilters.size();
}

PKFReturnCode PKFMetaFilter::getOutputFilter(uint32_t index, IPKFFilter **filter)
{
	if(filter == NULL || index >= this->getNumberOfOutputFilters())
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = this->outputFilters[index];

	return PKF_SUCCESS;
}

PKFReturnCode PKFMetaFilter::unregisterOutputFilter(uint32_t index)
{
	std::vector <IPKFFilter *> temp;

	for(uint32_t i=0; i < this->outputFilters.size(); i++)
	{
		if(i != index)
		{
			temp.push_back(this->outputFilters[i]);
		}
	}

	this->outputFilters = temp;

	return PKF_SUCCESS;
}

PKFReturnCode PKFMetaFilter::unregisterOutputFilter(IPKFFilter *filter)
{
	std::vector <IPKFFilter *> temp;

	for(uint32_t i=0; i < this->outputFilters.size(); i++)
	{
		if(this->outputFilters[i] != filter)
		{
			temp.push_back(this->outputFilters[i]);
		}
	}

	this->outputFilters = temp;

	return PKF_SUCCESS;
}
	
PKFReturnCode PKFMetaFilter::clearOutputFilters()
{
	this->outputFilters.clear();
	return PKF_SUCCESS;
}


// Reimplemented from IFilter to
// delegate to sub filters

uint32_t PKFMetaFilter::getNumberOfConnectors()
{
	uint32_t count = 0;

	for(uint32_t i=0; i < this->inputFilters.size(); i++)
	{
		if(this->inputFilters[i] == NULL)
		{
			continue;
		}

		for(uint32_t n=0; n < this->inputFilters[i]->getNumberOfConnectors(); n++)
		{
			IPKFConnector *conn = NULL;
			
			if(this->inputFilters[i]->getConnector(n, &conn) == PKF_SUCCESS)
			{
				if(conn->getType() == PKF_CONNECTOR_TYPE_INPUT)
				{
					count++;
				}
			}
		}
	}

	for(uint32_t j=0; j < this->outputFilters.size(); j++)
	{
		if(this->outputFilters[j] == NULL)
		{
			continue;
		}

		for(uint32_t n=0; n < this->outputFilters[j]->getNumberOfConnectors(); n++)
		{
			IPKFConnector *conn = NULL;
			
			if(this->outputFilters[j]->getConnector(n, &conn) == PKF_SUCCESS)
			{
				if(conn->getType() == PKF_CONNECTOR_TYPE_OUTPUT)
				{
					count++;
				}
			}
		}
	}

	return count;
}

PKFReturnCode PKFMetaFilter::getConnector(uint32_t index, IPKFConnector **connector)
{
	uint32_t count = 0;

	if(index > this->getNumberOfConnectors() ||
	   connector == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	for(uint32_t i=0; i < this->inputFilters.size(); i++)
	{
		if(this->inputFilters[i] == NULL)
		{
			continue;
		}

		for(uint32_t n=0; n < this->inputFilters[i]->getNumberOfConnectors(); n++)
		{
			IPKFConnector *conn = NULL;
			
			if(this->inputFilters[i]->getConnector(n, &conn) == PKF_SUCCESS)
			{
				if(conn->getType() == PKF_CONNECTOR_TYPE_INPUT)
				{
					if(count == index)
					{
						*connector = conn;
						return PKF_SUCCESS;
					}

					count++;
				}
			}
		}
	}

	for(uint32_t j=0; j < this->outputFilters.size(); j++)
	{
		if(this->outputFilters[j] == NULL)
		{
			continue;
		}

		for(uint32_t n=0; n < this->outputFilters[j]->getNumberOfConnectors(); n++)
		{
			IPKFConnector *conn = NULL;
			
			if(this->outputFilters[j]->getConnector(n, &conn) == PKF_SUCCESS)
			{
				if(conn->getType() == PKF_CONNECTOR_TYPE_OUTPUT)
				{
					if(count == index)
					{
						*connector = conn;
						return PKF_SUCCESS;
					}

					count++;
				}
			}
		}
	}

	*connector = NULL;
	return PKF_ERR_INVALID_PARAM;
}
