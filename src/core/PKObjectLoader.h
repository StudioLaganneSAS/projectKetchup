//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_OBJECT_LOADER_H
#define PK_OBJECT_LOADER_H

#include "PKObject.h"
#include "IPKObjectFactory.h"

//
// PKObjectLoader
//

class PKObjectLoader
{
public:

	PKObjectLoader();
	~PKObjectLoader();

	// Setup

	void addObjectFactory(IPKObjectFactory *factory);

	// Loading

	PKObject *loadFromXML(std::string xml);

	// Error checking

	bool getError();
	
protected:

	std::string skipWhitespace(std::string);

	std::string loadObject(PKObject *object,
						   std::string className,
						   std::string xml);

public:

	void setObjectProperty(PKObject *object,
						   std::string name,
						   std::string value);

private:
	
	bool error;
	std::vector <IPKObjectFactory *> factories;
};

#endif // PK_OBJECT_LOADER_H
