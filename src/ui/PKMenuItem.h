//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_MENUITEM_H
#define PK_MENUITEM_H

#include "PKObject.h"
#include "PKUI.h"
#include "PKImage.h"

class PKMenu;

//
// PKMenuItem
//

class PKMenuItem : public PKObject
{
public:

    // PROPERTIES

    static std::string ID_STRING_PROPERTY;
    static std::string TEXT_WSTRING_PROPERTY;
    static std::string SHORTCUT_STRING_PROPERTY;
    static std::string ENABLED_BOOL_PROPERTY;
    static std::string NEEDS_CHECKBOX_BOOL_PROPERTY;
    static std::string CHECKED_BOOL_PROPERTY;
    static std::string RIGHT_BOOL_PROPERTY;
    static std::string ICON_WSTRING_PROPERTY;

public:
    PKMenuItem();

protected:
    virtual ~PKMenuItem();

public:
    void    setId(int32_t id);
    int32_t getId();

    void setIcon(PKImage *icon);
    PKImage *getIcon();

    void    setMenu(PKMenu *menu);
    PKMenu *getMenu();

#ifdef WIN32
    void  setHIcon(HICON h);
    HICON getHIcon();
#endif

    // Watch our properties

    void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

    int32_t id;
    PKImage *icon;
    PKMenu *menu;

#ifdef WIN32
    HICON hicon;
#endif
};

#endif // PK_MENUITEM_H
