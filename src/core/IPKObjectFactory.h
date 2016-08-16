//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef IPK_OBJECT_FACTORY_H
#define IPK_OBJECT_FACTORY_H

#include "PKObject.h"

//
// IPKObjectFactory
//

class IPKObjectFactory
{
public:

	//
	// List all object handled by this factory
	//
	virtual std::vector<std::string> getAvailableObjectsList() = 0;

	//
	// Create an object 
	//
	virtual PKObject *createObject(std::string className) = 0;
};


#endif // IPK_OBJECT_FACTORY_H
