//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKPropertyBag.h"

//
//
//

PKPropertyBag::PKPropertyBag()
{
	this->owner = NULL;
}

PKPropertyBag::~PKPropertyBag()
{
	this->deleteProperties();
}

void PKPropertyBag::setOwner(PKPropertyCallback *owner)
{
	this->owner = owner;
}

bool PKPropertyBag::addProperty(std::string   name,
								PKVariantType type)
{
	// Check if we alredy have it

	bool found = false;

	for(unsigned int i=0; i < this->properties.size(); i++)
	{
		if(this->properties[i]->getName() == name)
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		return false;
	}

	PKProperty *prop = new PKProperty(name, type);
	prop->setPropertyCallback(this);
	this->properties.push_back(prop);

	return true;
}

bool PKPropertyBag::addProperty(PKProperty *prop)
{
	if(prop == NULL)
	{
		return false;
	}

	// Check if we alredy have it

	bool found = false;

	for(unsigned int i=0; i < this->properties.size(); i++)
	{
		if(this->properties[i]->getName() == prop->getName())
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		return false;
	}

	prop->setPropertyCallback(this);
	this->properties.push_back(prop);

	return true;
}

uint32_t PKPropertyBag::getNumberOfProperties()
{
	return this->properties.size();
}

PKProperty *PKPropertyBag::getPropertyAt(uint32_t index)
{
	if(index >= 0 && index < this->getNumberOfProperties())
	{
		return this->properties[index];
	}

	return NULL;
}

PKProperty *PKPropertyBag::getProperty(std::string name)
{
	for(unsigned int i=0; i<this->getNumberOfProperties(); i++)
	{
		PKProperty *p = this->getPropertyAt(i);

		if(p != NULL)
		{
			if(p->getName() == name)
			{
				return p;
			}
		}
	}

	return NULL;
}

bool PKPropertyBag::removeProperty(PKProperty *prop)
{
	std::vector <PKProperty *> temp;

	for(unsigned int i=0; i<this->getNumberOfProperties(); i++)
	{
		if(prop != this->getPropertyAt(i))
		{
			temp.push_back(this->getPropertyAt(i));
		}
		else
		{	
			prop->setPropertyCallback(NULL);
		}
	}

	this->properties = temp;
	return true;
}

void PKPropertyBag::deleteProperty(std::string name)
{
	std::vector <PKProperty *> temp;

	for(unsigned int i=0; i<this->getNumberOfProperties(); i++)
	{
		PKProperty *p = this->getPropertyAt(i);

		if(p != NULL)
		{
			if(name != p->getName())
			{
				temp.push_back(this->getPropertyAt(i));
			}
			else
			{
				delete p;
			}
		}
	}

	this->properties = temp;

}

void PKPropertyBag::deleteProperties()
{
	for(unsigned int i=0; i < this->properties.size(); i++)
	{
		delete this->properties[i];
	}

	this->properties.clear();
}

// Get / Set values

PKVariant *PKPropertyBag::get(std::string propertyName)
{
	for(unsigned int i=0; i < this->properties.size(); i++)
	{
		if(this->properties[i]->getName() == propertyName)
		{
			return this->properties[i]->getValue();
		}
	}

	return NULL;
}

PKVariant *PKPropertyBag::getDefault(std::string propertyName)
{
	for(unsigned int i=0; i < this->properties.size(); i++)
	{
		if(this->properties[i]->getName() == propertyName)
		{
			return this->properties[i]->getDefaultValue();
		}
	}

	return NULL;
}

bool PKPropertyBag::set(std::string propertyName,
					    PKVariant  *value,
						bool defaultValue)
{
	bool found = false;
	
	for(unsigned int i=0; i < this->properties.size(); i++)
	{
		if(this->properties[i]->getName() == propertyName)
		{
			// copy value and return
			this->properties[i]->setValue(value, defaultValue);
			found = true;
		}
	}

	return found;
}

bool PKPropertyBag::setNoCallback(std::string propertyName,
								  PKVariant  *value,
								  bool defaultValue)
{
	bool found = false;
	
	for(unsigned int i=0; i < this->properties.size(); i++)
	{
		if(this->properties[i]->getName() == propertyName)
		{
			// copy value and return
			this->properties[i]->setValueNoCallback(value, defaultValue);
			found = true;
		}
	}

	return found;
}


// PKPropertyCallback

void PKPropertyBag::onPropertyChanged(PKProperty *prop,  PKVariant *oldValue)
{
	if(this->owner != NULL)
	{
		this->owner->onPropertyChanged(prop, oldValue);
	}
}
