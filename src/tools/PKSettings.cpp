//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKSettings.h"
#include "PKStr.h"

//
// PKSettings
//

PKSettings::PKSettings()
{
    this->persist        = NULL;
	this->maxRecentFiles = 5;
}

PKSettings::~PKSettings()
{
    if(this->persist != NULL)
    {
        delete this->persist;
    }
}

void PKSettings::setAppName(std::string appName)
{
    this->appName = appName;

    if(this->persist != NULL)
    {
        delete this->persist;
    }

    this->persist = new PKPersistTool(appName);
}

void PKSettings::setMaxNumberOfRecentFiles(uint32_t max)
{
	this->maxRecentFiles = max;
}

void PKSettings::addRecentFile(std::wstring fileName)
{
	if(this->persist == NULL)
	{
		return;
	}

	std::vector<std::wstring> recent_files;
	uint32_t num = this->getNumberOfRecentFiles();

	for(uint32_t i=0; i<num; i++)
	{
		recent_files.push_back(this->getRecentFileAt(i));
	}

	// Check if it's already there or not

	bool found = false;

	for(uint32_t i=0; i<recent_files.size(); i++)
	{
		if(recent_files[i] == fileName)
		{
			recent_files.erase(recent_files.begin() + i);
			found = true;
			break;
		}
	}

	if(!found)
	{
		// Check if we are at capacity
		if(recent_files.size() == this->maxRecentFiles)
		{
			// Remove the last one
			recent_files.erase(recent_files.begin() + recent_files.size() - 1);
		}
	}

	// Now write back

	for(uint32_t i=0; i<this->maxRecentFiles; i++)
	{
		std::string keyName = "RecentFile" + PKStr::uint32ToString(i+1);

		if(i==0)
		{
			this->persist->setWString(keyName, fileName);
		}
		else
		{
			if(i <= recent_files.size())
			{
				this->persist->setWString(keyName, recent_files[i-1]);
			}
			else
			{
				this->persist->removeValue(keyName);
			}
		}
	}

}

void PKSettings::removeRecentFile(std::wstring fileName)
{
	if(this->persist == NULL)
	{
		return;
	}

	std::vector<std::wstring> recent_files;
	uint32_t num = this->getNumberOfRecentFiles();

	for(uint32_t i=0; i<num; i++)
	{
		recent_files.push_back(this->getRecentFileAt(i));
	}

	// Now remove the element

	for(uint32_t i=0; i<recent_files.size(); i++)
	{
		if(recent_files[i] == fileName)
		{
			recent_files.erase(recent_files.begin() + i);
			break;
		}
	}

	// Now write back

	for(uint32_t i=0; i<this->maxRecentFiles; i++)
	{
		std::string keyName = "RecentFile" + PKStr::uint32ToString(i+1);

		if(i < recent_files.size())
		{
			this->persist->setWString(keyName, recent_files[i]);
		}
		else
		{
			this->persist->removeValue(keyName);
		}
	}
}

void PKSettings::clearRecentFiles()
{
	for(uint32_t i=0; i<this->maxRecentFiles; i++)
	{
		std::string keyName = "RecentFile" + PKStr::uint32ToString(i+1);
		this->persist->removeValue(keyName);
	}
}

uint32_t PKSettings::getNumberOfRecentFiles()
{
	if(this->persist)
	{
		uint32_t i;

		for(i=1; i<=this->maxRecentFiles+1; i++)
		{
			std::string keyName = "RecentFile" + PKStr::uint32ToString(i);
			std::wstring val;

			if(!this->persist->getWString(keyName, val))
			{
				break;
			}
		}

		return (i-1);
	}

	return 0;
}

std::wstring PKSettings::getRecentFileAt(uint32_t i)
{
	if(this->persist)
	{
		std::string keyName = "RecentFile" + PKStr::uint32ToString(i+1);
		std::wstring val;

		if(this->persist->getWString(keyName, val))
		{
			return val;
		}
	}

	return L"";
}

bool PKSettings::load()
{
    if(this->persist == NULL)
    {
        return false;
    }

    uint32_t count = this->getProperties()->getNumberOfProperties();

    for(unsigned int i=0; i < count; i++)
    {
        PKProperty *prop = this->getProperties()->getPropertyAt(i);

        if(prop)
        {
            std::string name = prop->getName();

            switch(prop->getValue()->_type)
            {
            case PK_VARIANT_INT32:
                {
                    int32_t value;

                    if(this->persist->getInt32(name, &value))
                    {
                        PKVariantInt32 ival(value);
                        this->set(name, &ival);
                    }
                }
                break;

            case PK_VARIANT_BOOL:
                {
                    bool value;

                    if(this->persist->getBool(name, &value))
                    {
                        PKVariantBool bval(value);
                        this->set(name, &bval);
                    }
                }
                break;

            case PK_VARIANT_WSTRING:
                {
                    std::wstring value;

                    if(this->persist->getWString(name, value))
                    {
                        PKVariantWString wval(value);
                        this->set(name, &wval);
                    }
                }
                break;

            default:
                {
                    // All other property types
                    // are saved in their string
                    // representation

                    std::string value;

                    if(this->persist->getString(name, value))
                    {
                        this->loader.setObjectProperty(this, name, value);
                    }
                }
                break;
            }
        }
    }

    return true;
}

bool PKSettings::save()
{
    if(this->persist == NULL)
    {
        return false;
    }

    uint32_t count = this->getProperties()->getNumberOfProperties();

    for(unsigned int i=0; i < count; i++)
    {
        PKProperty *prop = this->getProperties()->getPropertyAt(i);

        if(prop)
        {
            std::string name = prop->getName();

            switch(prop->getValue()->_type)
            {
            case PK_VARIANT_INT32:
                {
                    PKVariant *v  = this->get(name);
                    int32_t value = PKVALUE_INT32(v);

                    this->persist->setInt32(name, value);
                }
                break;

            case PK_VARIANT_BOOL:
                {
                    PKVariant *v  = this->get(name);
                    bool value = PKVALUE_BOOL(v);

                    this->persist->setBool(name, value);
                }
                break;

            case PK_VARIANT_WSTRING:
                {
                    PKVariant *v  = this->get(name);
                    std::wstring value = PKVALUE_WSTRING(v);

                    this->persist->setWString(name, value);
                }
                break;

            default:
                {
                    // All other property types
                    // are saved in their string
                    // representation

                    std::string value;

                    PKVariant *v = this->get(name);
                    value = v->toString();

                    this->persist->setString(name, value);
                }
                break;
            }
        }
    }

    return true;
}
