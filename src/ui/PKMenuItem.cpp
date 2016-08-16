//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKMenuItem.h"
#include "PKMenu.h"

//
// PROPERTIES
//

std::string PKMenuItem::ID_STRING_PROPERTY           = "id";
std::string PKMenuItem::TEXT_WSTRING_PROPERTY        = "text";
std::string PKMenuItem::SHORTCUT_STRING_PROPERTY     = "shortcut";
std::string PKMenuItem::ENABLED_BOOL_PROPERTY        = "enabled";
std::string PKMenuItem::NEEDS_CHECKBOX_BOOL_PROPERTY = "needsCheckBox";
std::string PKMenuItem::CHECKED_BOOL_PROPERTY        = "checked";
std::string PKMenuItem::RIGHT_BOOL_PROPERTY          = "right";
std::string PKMenuItem::ICON_WSTRING_PROPERTY        = "icon";


//
// PKMenuItem
//

PKMenuItem::PKMenuItem()
{
    this->id   = 0;
    this->icon = NULL;
    this->menu = NULL;

#ifdef WIN32
    this->hicon = NULL;
#endif

    PKOBJECT_ADD_STRING_PROPERTY(id, "");
    PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
    PKOBJECT_ADD_STRING_PROPERTY(shortcut, "");
    PKOBJECT_ADD_BOOL_PROPERTY(enabled, true);
    PKOBJECT_ADD_BOOL_PROPERTY(needsCheckBox, false);
    PKOBJECT_ADD_BOOL_PROPERTY(checked, false);
    PKOBJECT_ADD_BOOL_PROPERTY(right, false);
    PKOBJECT_ADD_WSTRING_PROPERTY(icon, L"");
}

PKMenuItem::~PKMenuItem()
{
    if(this->getChildrenCount() == 1)
    {
        this->getChildAt(0)->release();
    }

#ifdef WIN32
    if(this->hicon != NULL)
    {
        DestroyIcon(this->hicon);
    }    
#endif

    if(this->icon != NULL)
    {
        delete this->icon;
    }
}

void PKMenuItem::setId(int32_t id)
{
    this->id = id;
}

int32_t PKMenuItem::getId()
{
    return this->id;
}

void PKMenuItem::setIcon(PKImage *icon)
{
    this->icon = icon;
}

PKImage *PKMenuItem::getIcon()
{
    return this->icon;
}

#ifdef WIN32

void PKMenuItem::setHIcon(HICON h)
{
    this->hicon = h;
}

HICON PKMenuItem::getHIcon()
{
    return this->hicon;
}

#endif


void PKMenuItem::setMenu(PKMenu *menu)
{
    this->menu = menu;
}

PKMenu *PKMenuItem::getMenu()
{
    return this->menu;
}

// 
// Properties
//

void PKMenuItem::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
    PKVariant *i   = this->get(PKMenuItem::ID_STRING_PROPERTY);
    std::string id = PKVALUE_STRING(i);

    if(prop->getName() == PKMenuItem::ENABLED_BOOL_PROPERTY)
	{
        if(this->menu != NULL)
        {
            this->menu->setItemEnabled(id, PKVALUE_BOOL(prop->getValue()));
        }
    }

    if(prop->getName() == PKMenuItem::CHECKED_BOOL_PROPERTY)
	{
        if(this->menu != NULL)
        {
            this->menu->setItemChecked(id, PKVALUE_BOOL(prop->getValue()));
        }
    }

    if(prop->getName() == PKMenuItem::TEXT_WSTRING_PROPERTY)
    {
        if(this->menu != NULL)
        {
            this->menu->updateItemText(id);
        }
    }
}
