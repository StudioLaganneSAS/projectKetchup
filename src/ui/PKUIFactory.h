//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKUI_FACTORY_H
#define PKUI_FACTORY_H

#include "IPKObjectFactory.h"

//
// PKUIFactory
//

class PKUIFactory : public IPKObjectFactory
{
public:
	
	PKUIFactory();
	~PKUIFactory();
	
	std::vector<std::string> getAvailableObjectsList();

	PKObject *createObject(std::string className);	
	
private:
	
	std::vector<std::string> objectsAvailable;
};

#endif // PKUI_FACTORY_H