//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKPROPERTY_BAG_H
#define PKPROPERTY_BAG_H

#include "PKProperty.h"
#include "PKPropertyCallback.h"
#include <vector>

//
// PKPropertyBag
//

class PKPropertyBag : PKPropertyCallback
{

	friend class PKObject;

public:
	PKPropertyBag();
	~PKPropertyBag();

	void setOwner(PKPropertyCallback *owner);

    // Properties management

	bool addProperty(std::string   name,
					 PKVariantType type);

	bool addProperty(PKProperty *prop);
	bool removeProperty(PKProperty *prop);

	uint32_t getNumberOfProperties();

	PKProperty *getPropertyAt(uint32_t index);
	PKProperty *getProperty(std::string name);

	void deleteProperty(std::string name);
	void deleteProperties();

	// Get / Set values

	PKVariant *get(std::string propertyName);

	bool set(std::string propertyName,
			 PKVariant  *value,
			 bool defaultValue = false);

	PKVariant *getDefault(std::string propertyName);

private:

	PKPropertyCallback		  *owner;
	std::vector <PKProperty *> properties;

	// PKPropertyCallback

	void onPropertyChanged(PKProperty *prop, PKVariant *oldValue);

public:

    // Use only if you know what you are doing !!

    bool setNoCallback(std::string propertyName,
					   PKVariant  *value,
					   bool defaultValue = false);
};

#endif // PKPROPERTY_BAG_H
