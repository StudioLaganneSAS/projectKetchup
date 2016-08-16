//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_TOOLBAR_ITEM_H
#define PK_TOOLBAR_ITEM_H

#include "PKUI.h"
#include "PKObject.h"

//
// PKToolbarItem
//

class PKToolbarItem : public PKObject
{
public:
	
	//
	// PROPERTIES
	//
	
	static std::string ID_STRING_PROPERTY;
	static std::string TEXT_WSTRING_PROPERTY;
	static std::string ICON_WSTRING_PROPERTY;
	static std::string HELP_WSTRING_PROPERTY;
	
public:	
	
	PKToolbarItem();

protected:
	virtual ~PKToolbarItem();
	
public:
    uint32_t getId();
	void     setId(uint32_t id);
	
private:

	uint32_t id;
};

#endif // PK_TOOLBAR_ITEM_H
