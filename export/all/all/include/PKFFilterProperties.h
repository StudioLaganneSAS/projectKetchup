//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKFFILTERPROPERTIES_H
#define PKFFILTERPROPERTIES_H

#include "PKFTypes.h"

//
// PKFFilterProperties
//

class PKFFilterProperties 
{
public:

    PKFFilterProperties();
    ~PKFFilterProperties();

	std::string    getFilterId();
    void           setFilterId(std::string filterId);

    std::wstring   getFilterName();
    void           setFilterName(std::wstring filterName);

    std::wstring   getDescription();
    void           setDescription(std::wstring description);

    std::wstring   getAuthors();
    void           setAuthors(std::wstring authors);

    std::string    getVersion();
    void           setVersion(std::string version);

    std::wstring   getVendor();
    void           setVendor(std::wstring vendor);

    std::string    getHomepage();
    void           setHomepage(std::string homepage);

    std::string    getEmail();
    void           setEmail(std::string email);

    PKFFilterCategory getFilterCategory();
    void              setFilterCategory(PKFFilterCategory filterCategory);

private:
    std::string         filterId;
    std::wstring        filterName;
    std::wstring        description;
    std::wstring        authors;
    std::wstring        vendor;
    std::string         version;
    std::string         homepage;
    std::string         email;
    PKFFilterCategory   filterCategory;
};

#endif // PKFFILTERPROPERTIES_H
