//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKMusicUIFactory.h"

// Objects

#include "PKChordDiagramUI.h"
#include "PKChordDiagramVectorUI.h"

//
// PKMusicUIFactory
//

PKMusicUIFactory::PKMusicUIFactory()
{
	this->availableObjects.push_back("PKChordDiagramUI");
	this->availableObjects.push_back("PKChordDiagramVectorUI");
}

PKMusicUIFactory::~PKMusicUIFactory()
{
}
		
std::vector<std::string> PKMusicUIFactory::getAvailableObjectsList()
{
	return this->availableObjects;
}

PKObject *PKMusicUIFactory::createObject(std::string className)
{
	if(className == "PKChordDiagramUI")
	{
		return new PKChordDiagramUI();
	}

	if(className == "PKChordDiagramVectorUI")
	{
		return new PKChordDiagramVectorUI();
	}

	return NULL;
}
		
