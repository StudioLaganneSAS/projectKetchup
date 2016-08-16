//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#include "IPKFFilterFactory.h"
#include "PKFFilterProperties.h"

//
// IPKFFilterFactory
//

IPKFFilterFactory::IPKFFilterFactory()
{

}

IPKFFilterFactory::~IPKFFilterFactory()
{
	for(uint32_t i=0; i < this->getNumberOfFilters(); i++)
	{
		if(this->records[i] != NULL)
		{			
			delete this->records[i];
		}			
	}

	this->records.clear();
}

PKFReturnCode IPKFFilterFactory::addFilter(PKF_IFilterCreate        *filterCreateFunction,
										   PKF_FilterPropertiesFill *filterPropertiesFillFunction)
{
	if(filterCreateFunction == NULL || 
	   filterPropertiesFillFunction == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	PKFFilterFactoryRecord *rec = new PKFFilterFactoryRecord;

	if(rec == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	rec->createFunction         = filterCreateFunction;
	rec->fillPropertiesFunction = filterPropertiesFillFunction;

	this->records.push_back(rec);

	return PKF_SUCCESS;
}

//
// Use those three functions to
// create the filters you want...
//

uint32_t IPKFFilterFactory::getNumberOfFilters()
{
	return this->records.size();
}

PKFReturnCode IPKFFilterFactory::getFilterProperties(uint32_t index, PKFFilterProperties **prop)
{
	if((index >= this->getNumberOfFilters()) ||
	   (prop  == NULL))
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*prop = new PKFFilterProperties();

	if(*prop == NULL)
	{
		return PKF_ERR_OUT_OF_MEMORY;
	}

	if(this->records[index]->fillPropertiesFunction != NULL)
	{
		return this->records[index]->fillPropertiesFunction(*prop);
	}

	return PKF_ERR_NOT_READY;
}

PKFReturnCode IPKFFilterFactory::createFilter(uint32_t index, IPKFFilter **filter)
{
	if((index >= this->getNumberOfFilters()) ||
	   (filter == NULL))
	{
		return PKF_ERR_INVALID_PARAM;
	}

	if(this->records[index]->createFunction == NULL ||
	   this->records[index]->fillPropertiesFunction == NULL)
	{
		return PKF_ERR_NOT_READY;
	}

	PKFFilterProperties *prop = NULL;

	PKFReturnCode result = this->getFilterProperties(index, &prop);

	if(result != PKF_SUCCESS)
	{
		return result;
	}

	result = this->records[index]->createFunction(filter);

	if(result != PKF_SUCCESS)
	{
		return result;
	}

	(*filter)->setFilterProperties(prop);

	return PKF_SUCCESS;
}

PKFReturnCode IPKFFilterFactory::createFilter(std::string filterId, IPKFFilter **filter)
{
	uint32_t index = 0;
	bool     found = false;

	for(uint32_t i=0; i < this->getNumberOfFilters(); i++)
	{
		PKFFilterProperties *prop;

		if(this->getFilterProperties(i, &prop) == PKF_SUCCESS)
		{
			if(prop->getFilterId() == filterId)
			{
				found = true;
				index = i;
	
				delete prop;

				break;
			}

			delete prop;
		}
	}	

	if(found)
	{
		return this->createFilter(index, filter);
	}

	return PKF_ERR_NOT_AVAILABLE;
}
