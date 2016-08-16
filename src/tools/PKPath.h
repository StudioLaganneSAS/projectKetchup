//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_PATH_H
#define PK_PATH_H

#include "PKTypes.h"
#include <string>
#include <vector>

#ifdef MACOSX
#include <CoreServices/CoreServices.h>
#endif

class PKPath
{
public:

    static std::wstring getPathSeparator();

    static uint64_t getSpaceAvailable(std::wstring folder);

    static uint64_t getFileSize(std::wstring filename);

    static std::wstring getTempPath();

    static std::vector<std::wstring> getFiles(std::wstring directory,
                                              std::wstring fileTypes,
                                              bool recursive = false);

    static std::wstring getTempFileName(std::wstring intofolder);

    static bool deleteFile(std::wstring filename);
    static bool fileExists(std::wstring filename);

    static bool directoryExists(std::string directory);
    static bool directoryExists(std::wstring directory);
    static bool createDirectory(std::wstring dir);

    static bool moveFile(std::wstring source, std::wstring dest);
    static bool copyFile(std::wstring source, std::wstring dest);

    static std::wstring getDesktopFolder();

    static std::wstring truncatePathForDisplay(std::wstring filename,
                                               int          maxChars);

    // Opens the OS File Managet at folder

    static void navigateToFolder(std::wstring folder);

    static std::wstring getExtension(std::wstring fullPath);
    static std::wstring getFileName(std::wstring fullPath);
    static std::wstring getFileNameAndExtension(std::wstring fullPath);
    static std::wstring getDirectoryName(std::wstring fullPath);

#ifdef MACOSX
    static bool getFSRefForPath(std::wstring directory, FSRef *ref);
#endif

    static void         setAppSupportPath(std::wstring path);
    static std::wstring getAppSupportPath();

private:

    static std::wstring appSupportPath;
};

#endif // PK_PATH_H
