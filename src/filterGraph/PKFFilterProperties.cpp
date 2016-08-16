//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#include "PKFFilterProperties.h"

//
// PKFFilterProperties
//

PKFFilterProperties::PKFFilterProperties()
{
	this->filterCategory = PKF_FILTER_CATEGORY_NONE;
}

PKFFilterProperties::~PKFFilterProperties()
{

}

std::string PKFFilterProperties::getFilterId()
{
	return this->filterId;
}

void PKFFilterProperties::setFilterId(std::string filterId)
{
	this->filterId = filterId;
}

std::wstring PKFFilterProperties::getFilterName()
{
	return this->filterName;
}

void PKFFilterProperties::setFilterName(std::wstring filterName)
{
	this->filterName = filterName;
}

std::wstring PKFFilterProperties::getDescription()
{
	return this->description;
}

void PKFFilterProperties::setDescription(std::wstring description)
{
	this->description = description;
}

std::wstring PKFFilterProperties::getAuthors()
{
	return this->authors;
}

void PKFFilterProperties::setAuthors(std::wstring authors)
{
	this->authors = authors;
}

std::string PKFFilterProperties::getVersion()
{
	return this->version;
}

void PKFFilterProperties::setVersion(std::string version)
{
	this->version = version;
}

std::wstring PKFFilterProperties::getVendor()
{
	return this->vendor;
}

void PKFFilterProperties::setVendor(std::wstring vendor)
{
	this->vendor = vendor;
}

std::string PKFFilterProperties::getHomepage()
{
	return this->homepage;
}

void PKFFilterProperties::setHomepage(std::string homepage)
{
	this->homepage = homepage;
}

std::string PKFFilterProperties::getEmail()
{
	return this->email;
}

void PKFFilterProperties::setEmail(std::string email)
{
	this->email = email;
}

PKFFilterCategory PKFFilterProperties::getFilterCategory()
{
	return this->filterCategory;
}

void PKFFilterProperties::setFilterCategory(PKFFilterCategory filterCategory)
{
	this->filterCategory = filterCategory;
}

