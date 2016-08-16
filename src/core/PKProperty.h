//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_PROPERTY_H
#define PK_PROPERTY_H

#include "PKTypes.h"
#include "PKVariant.h"
#include "PKPropertyCallback.h"

//
// PKProperty
//

class PKProperty
{

	friend class PKObject;
	friend class PKPropertyBag;

public:

	PKProperty(std::string name, PKVariantType type);
	~PKProperty();

	std::string getName();
	void setName(std::string n);

	PKVariant *getValue();
	void	   setValue(PKVariant *val, 
						bool defaultValue = false);

	PKVariant *getDefaultValue();

	void setPropertyCallback(PKPropertyCallback *callback);

	std::string toXML();
	
protected:

	void setValueNoCallback(PKVariant *value, 
						    bool defaultValue = false);

private:

	std::string name;
	PKVariant  *value;
	PKVariant  *defaultValue;

	PKPropertyCallback *callback;
};

#endif // PK_PROPERTY_H