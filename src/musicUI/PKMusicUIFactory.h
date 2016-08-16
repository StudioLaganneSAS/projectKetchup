//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKMUSICUI_FACTORY_H
#define PKMUSICUI_FACTORY_H

#include "IPKObjectFactory.h"

//
// PKMusicUIFactory
//

class PKMusicUIFactory : public IPKObjectFactory
{
public:
	
	PKMusicUIFactory();
	~PKMusicUIFactory();
	
	std::vector<std::string> getAvailableObjectsList();

	PKObject *createObject(std::string className);	
	
private:
	
	std::vector<std::string> availableObjects;
};

#endif // PKMUSICUI_FACTORY_H