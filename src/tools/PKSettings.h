//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SETTINGS_H
#define PK_SETTINGS_H

#include "PKObject.h"
#include "PKPersistTool.h"
#include "PKObjectLoader.h"

//
// PKSettings
//
// Extend this class and
// add properties to it,
// they will be persisted
// on a per-user basis
//

class PKSettings : public PKObject
{
public:
    
    PKSettings();

    void setAppName(std::string appName);

    bool load();
    bool save();

	void setMaxNumberOfRecentFiles(uint32_t max);

	void addRecentFile(std::wstring fileName);
	void removeRecentFile(std::wstring fileName);

	void clearRecentFiles();

	uint32_t     getNumberOfRecentFiles();
	std::wstring getRecentFileAt(uint32_t i);

protected:
    ~PKSettings();

private:

    std::string    appName;
    PKPersistTool *persist;
    PKObjectLoader loader;
	uint32_t maxRecentFiles;
};

#endif // PK_SETTINGS_H 
