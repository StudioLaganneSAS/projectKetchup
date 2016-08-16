//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKToolbarItem.h"

//
// PROPERTIES
//

std::string PKToolbarItem::ID_STRING_PROPERTY        = "id";
std::string PKToolbarItem::TEXT_WSTRING_PROPERTY     = "text";
std::string PKToolbarItem::ICON_WSTRING_PROPERTY     = "icon";
std::string PKToolbarItem::HELP_WSTRING_PROPERTY     = "help";

//
// PKToolbarItem
//

PKToolbarItem::PKToolbarItem()
{
	PKOBJECT_ADD_STRING_PROPERTY(id, "");

	PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(icon, L"NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(help, L"NOT_SET");
	
	this->id = 0;
}

PKToolbarItem::~PKToolbarItem()
{
	
}

uint32_t PKToolbarItem::getId()
{
	return this->id;
}

void PKToolbarItem::setId(uint32_t id)
{
	this->id = id;
}
