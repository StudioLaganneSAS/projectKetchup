//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKPath.h"
#include "PKStr.h"
#include "PKOS.h"
#include "PKFile.h"

#ifdef WIN32
#include <shlobj.h>
#endif

#ifdef LINUX
#include <sys/statvfs.h>
#endif

#ifdef MACOSX
#include <sys/param.h>
#include <sys/mount.h>
#endif

#if(defined(LINUX) || defined(MACOSX))
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#endif

#ifdef LINUX
#include <unistd.h>
#endif

#include <set>

std::wstring PKPath::appSupportPath = L"./";

void PKPath::setAppSupportPath(std::wstring path)
{
    PKPath::appSupportPath = path;
}

std::wstring PKPath::getAppSupportPath()
{
    return PKPath::appSupportPath;    
}

std::wstring PKPath::getPathSeparator()
{
#ifdef WIN32
    return L"\\";
#endif

#ifdef LINUX
    return L"/";
#endif

#ifdef MACOSX
    return L"/";
#endif
}

uint64_t PKPath::getSpaceAvailable(std::wstring folder)
{
    uint64_t available = 0;

#ifdef WIN32

    ULARGE_INTEGER free       = { 0 };
    ULARGE_INTEGER totalBytes = { 0 };
    ULARGE_INTEGER totalFree  = { 0 };

    GetDiskFreeSpaceEx(folder.c_str(),
                       &free,
                       &totalBytes,
                       &totalFree);

    available = free.QuadPart;

#endif

#if(defined(LINUX))

    struct statvfs stat = { 0 };
    std::string path = PKStr::wStringToString(folder);

    int result = statvfs(path.c_str(), &stat);

    if(result == 0)
    {
        uint64_t bfree = stat.f_bfree;
        uint64_t bsize = stat.f_bsize;
        available = bfree * bsize;
    }

#endif

#ifdef MACOSX

    struct statfs stat = { 0 };
    std::string path = PKStr::wStringToString(folder);

    int result = statfs(path.c_str(), &stat);

    if(result == 0)
    {
        uint64_t bfree = stat.f_bfree;
        uint64_t bsize = stat.f_bsize;
        available = bfree * bsize;
    }

#endif

    return available;
}

uint64_t PKPath::getFileSize(std::wstring filename)
{
    uint64_t size = 0;

#ifdef WIN32
    WIN32_FILE_ATTRIBUTE_DATA fileInformation;
    if(!GetFileAttributesEx(filename.c_str(), GetFileExInfoStandard, &fileInformation))
    {
        return 0;
    }

    uint64_t higher = fileInformation.nFileSizeHigh;
    higher <<= 32;
    size = (fileInformation.nFileSizeLow & 0xFFFFFFFF) + higher;
#endif

#if(defined(MACOSX) || defined(LINUX))
    
	PKFile *file = new PKFile();

    if(file->open(filename, PKFILE_MODE_READ) == PKFILE_OK)
    {
        size = file->getFileSize();
        file->close();
    }

	delete file;

#endif

    return size;
}

std::wstring PKPath::getTempFileName(std::wstring folder)
{

#ifdef WIN32

    wchar_t tmp[MAX_PATH];
    GetTempFileName(folder.c_str(), L"mod", 0, tmp);

    return tmp;

#endif

#if(defined(MACOSX) || defined(LINUX))

    std::string temp = PKStr::wStringToUTF8string(folder) + "/pktXXXXXX";
    int fd;

    fd = mkstemp((char *)temp.c_str());

    if(fd != -1)
    {
        close(fd);
    }
    else
    {
        return L"";
    }

    return PKStr::stringToWString(temp);

#endif

}

std::wstring PKPath::getTempPath()
{
#ifdef WIN32

    std::wstring path;

    BSTR bsPath = SysAllocStringLen(NULL, _MAX_PATH);
    DWORD length = GetTempPathW(_MAX_PATH, bsPath);
    path = bsPath;
    SysFreeString(bsPath);

    return path;

#endif

#if(defined(MACOSX) || defined(LINUX))
    return L"/tmp";
#endif

}

bool PKPath::deleteFile(std::wstring filename)
{
#ifdef WIN32
    return (DeleteFile(filename.c_str()) > 0);
#endif

#if(defined(MACOSX) || defined(LINUX))
    return (unlink(PKStr::wStringToUTF8string(filename).c_str()) == 0);
#endif
}

bool PKPath::directoryExists(std::wstring directory)
{
#ifdef WIN32

    DWORD result = GetFileAttributes(directory.c_str());

    if(result == 0xFFFFFFFF)
    {
        return false;
    }

    return ((result & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

#endif

#ifdef LINUX

    struct stat buf;
    return (stat(PKStr::wStringToUTF8string(directory.c_str()).c_str(), &buf) == 0);

#endif


#ifdef MACOSX

    std::string str = PKStr::wStringToUTF8string(directory);

    FSRef    ref;
    Boolean  b;

    OSStatus s = FSPathMakeRef((const UInt8 *) str.c_str(), &ref, &b);

    if(s != noErr)
    {
        return false;
    }

    return (b == true);

#endif

    return false;
}

bool PKPath::directoryExists(std::string directory)
{
#ifdef WIN32

    DWORD result = GetFileAttributesA(directory.c_str());

    if(result == 0xFFFFFFFF)
    {
        return false;
    }

    return ((result & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

#endif

#ifdef LINUX

    struct stat buf;
    return (stat(directory.c_str(), &buf) == 0);

#endif


#ifdef MACOSX

    struct stat buf;
    return (stat(directory.c_str(), &buf) == 0);

#endif

    return false;
}

bool PKPath::fileExists(std::wstring filename)
{
    if(filename.size() == 0)
    {
        return false;
    }

#ifdef WIN32

    HANDLE hFile = CreateFile(filename.c_str(), 0,
                              FILE_SHARE_READ, NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        return true;
    }
    else
    { 
        return false;
    }

#endif

#ifdef MACOSX

	FSRef fileRef;
	
	if(!PKPath::getFSRefForPath(filename, &fileRef))
	{
		return false;
	}
	
	// Get info on the file
	
	FSCatalogInfo info;
	
	OSErr err = FSGetCatalogInfo(&fileRef,
								 kFSCatInfoNodeFlags,
								 &info,
								 NULL, NULL, NULL);
								 
	if(err != noErr)
	{
		// Doesn't exist
		return false;
	}				
	
	if(info.nodeFlags & (1 << kFSNodeIsDirectoryBit))
	{
		// This is a directory
		return false;
	}
	
	return true;	

#endif

#ifdef LINUX

    std::string str = PKStr::wStringToString(filename);
	
	struct stat statb;
	int err = stat(str.c_str(), &statb);

	
	bool ok = err == 0 && ! statb.st_mode & S_IFDIR; // check that the file was found and it wasn't a directory
	
	return ok;
	
	/*
    if(0 == access(str.c_str(), R_OK))
    {
        return true;
    }
    else
    {
        return false;
    }*/

#endif

    return false;
}

bool PKPath::createDirectory(std::wstring directory)
{
#ifdef WIN32

    if(0 == CreateDirectory(directory.c_str(), NULL))
    {
        return false;
    }

    return true;

#endif

#ifdef LINUX

// The call to mkdir doesn't handle making directories recursively.  So we will change it to do things the hard way.
    int pos = directory.find_first_of(PKPath::getPathSeparator());
    while(pos != -1)
    {
    std::wstring currentDirectory = directory.substr(0, pos + 1);
    std::string curDir = PKStr::wStringToString(currentDirectory.c_str());
        int created = mkdir( curDir.c_str(), 0x1ed);
    int error = errno;
    if((created == 0) || (error == EEXIST) || (EACCES == error) || (EROFS == error) || (EPERM == error))
    {
        pos += 1;
        pos = directory.find_first_of(PKPath::getPathSeparator(), pos);
    }
    else
    {
        return false;
    }
    }

    return true;

#endif

#ifdef MACOSX

    // Get parent directory

    int pos = directory.find_last_of(PKPath::getPathSeparator());
    std::wstring parent = directory.substr(0, pos);

    FSRef parentRef;

    if(!PKPath::getFSRefForPath(parent, &parentRef))
    {
        return false;
    }

    std::wstring newDir = directory.substr(pos+1);

    uint32_t ndSize;
    UniChar *ndPtr;

    PKStr::wStringToUniChar(newDir, &ndSize, &ndPtr);

    if(ndPtr == NULL)
    {
        return false;
    }

    OSErr err = FSCreateDirectoryUnicode(&parentRef,
                                         ndSize,
                                         ndPtr,
                                         kFSCatInfoNone,
                                         NULL, NULL, NULL, NULL);

    delete [] ndPtr;

    return (err == noErr);

#endif

    return false;
}

bool PKPath::moveFile(std::wstring source, std::wstring dest)
{
#ifdef WIN32

    if(true == PKPath::fileExists(dest))
    {
        PKPath::deleteFile(dest);
    }

    int result = MoveFile(source.c_str(),
                          dest.c_str());

    return (result > 0);

#endif

#ifdef MACOSX

    // Extract destination directory
    // and destination filename

    int poss = source.find_last_of(PKPath::getPathSeparator());
    int posd = dest.find_last_of(PKPath::getPathSeparator());

    if(poss == std::wstring::npos || posd == std::wstring::npos)
    {
        return false;
    }

    std::wstring srcDir  = source.substr(0, poss);

    std::wstring destDir = dest.substr(0, posd);
    std::wstring destFln = dest.substr(posd+1);

    FSRef srcRef;
    FSRef dstDirRef;
    FSRef dstRef;

    if(!PKPath::getFSRefForPath(source, &srcRef))
    {
        return false;
    }

    if(srcDir != destDir)
    {
        // We need to move the file to
        // the new directory first...

        if(!PKPath::getFSRefForPath(destDir, &dstDirRef))
        {
            return false;
        }

        OSErr err = FSMoveObject(&srcRef, &dstDirRef, &dstRef);

        if(err != noErr) return false;

        // Now rename

        uint32_t size = 0;
        UniChar *ptr  = NULL;

        PKStr::wStringToUniChar(destFln, &size, &ptr);

        if(ptr != NULL)
        {
            err = FSRenameUnicode(&dstRef, size, ptr,
                                  kTextEncodingUnknown, NULL);
            delete [] ptr;

            if(err != noErr)
            {
                return false;
            }
        }
    }
    else
    {
        // Just rename the file since the
        // destination is in the same folder

        uint32_t size = 0;
        UniChar *ptr  = NULL;

        PKStr::wStringToUniChar(destFln, &size, &ptr);

        if(ptr != NULL)
        {
            OSErr err = FSRenameUnicode(&srcRef, size, ptr,
                                        kTextEncodingUnknown, NULL);
            delete [] ptr;

            if(err != noErr)
            {
                return false;
            }
        }
    }

    return true;

#endif

}

bool PKPath::copyFile(std::wstring source, std::wstring dest)
{
#ifdef WIN32

    int result = CopyFile(source.c_str(),
                          dest.c_str(), FALSE);

    return (result > 0);

#endif

#ifdef LINUX
        std::wstring copyCommand = L"cp";
        std::wstring chmod = L"chmod 755";
        std::wstring command = copyCommand + L" \"" + source + L"\"  \"" + dest + L"\"";
    std::wstring chmodCommand = chmod + L" \"" + dest;

        bool success= system(PKStr::wStringToString(command.c_str()).c_str());
    if (success == true )
    {
        return system(PKStr::wStringToString(chmodCommand).c_str());
    }
    else
    {
        return false;
    }
#endif

#ifdef MACOSX

    CFStringRef srcRef = PKStr::wStringToCFString(source);
    CFStringRef dstRef = PKStr::wStringToCFString(dest);

    int srcStringLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(srcRef),
                                                            kCFStringEncodingUTF8);

    int dstStringLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(dstRef),
                                                            kCFStringEncodingUTF8);

    char *srcString = new char[srcStringLength];
    char *dstString = new char[dstStringLength];

    if(srcString == NULL || dstString == NULL)
    {
        return false;
    }

    CFStringGetCString(srcRef, (char *)srcString,
                       srcStringLength, kCFStringEncodingUTF8);

    CFStringGetCString(dstRef, (char *)dstString,
                       dstStringLength, kCFStringEncodingUTF8);

    std::string command = "cp \"";
    command += srcString;
    command += "\" \"";
    command += dstString;
    command += "\"";

    system(command.c_str());

    CFRelease(srcRef);
    CFRelease(dstRef);

    delete [] srcString;
    delete [] dstString;

    return true;

#endif

}

std::wstring PKPath::truncatePathForDisplay(std::wstring filename,
                                               int          maxSize)
{
    int filePartCount = 1;
    unsigned int pos = 0;
    int i;

    pos = filename.find(L"/", pos);
    while(pos < filename.size())
    {
        filePartCount++;
        pos = filename.find(L"/", pos + 1);
    }

    int * partSizeArray = new int[filePartCount];
    int * partStartPos= new int[filePartCount];

    int previousPos = 0;

    for (i = 0; i < filePartCount; i ++)
    {
        if(i == (filePartCount - 1))
        {
            partSizeArray[i] = filename.size() - previousPos;
            partStartPos[i] = previousPos;

        }
        else
        {
            pos = filename.find(L"/", previousPos);
            partSizeArray[i] = pos - previousPos;
            partStartPos[i] = previousPos;
            previousPos = pos + 1;
        }
    }

    std::wstring result = L"";
    if(filename.size() <= (unsigned int) maxSize)
    {
        result = filename;
    }
    else if(partSizeArray[filePartCount - 1] > maxSize)
    {
        result = filename.substr(filename.size() - partSizeArray[filePartCount - 1], maxSize - 3);
        result.append(L"...");
    }
    else if (partSizeArray[filePartCount - 1] == maxSize)
    {
        result = filename.substr(filename.size() - partSizeArray[filePartCount - 1], maxSize);
    }
    else
    {
        // figure out how much of the path to cut off.
        int titleSize = partSizeArray[filePartCount - 1];
        int amountLeft = maxSize - titleSize - 4;
        int frontPathParts = 0;

        for(i=0; i < filePartCount; i++)
        {
            if(partSizeArray[i] < amountLeft)
            {
                frontPathParts++;
                amountLeft -= ( partSizeArray[i] + 1);
            }
            else
            {
                break;
            }
        }

        //Put it all together
        for(int i=0; i < frontPathParts; i++)
        {
            result.append(filename.substr(partStartPos[i], partSizeArray[i]));
            result.append(L"/");
        }

        result.append(L".../");
        result.append(filename.substr(partStartPos[filePartCount - 1]));
    }

    delete [] partSizeArray;
    delete [] partStartPos;

    return result;
}

std::wstring PKPath::getDesktopFolder()
{
#ifdef WIN32

    std::wstring      wdir;
    LPITEMIDLIST pidlItem;

    if(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP , &pidlItem) == NOERROR)
    {
        TCHAR folder[MAX_PATH];
        if( SHGetPathFromIDList(pidlItem, folder))
        {
            wdir = folder;
        }
    }

    return wdir;

#endif

#ifdef MACOSX

    FSRef ref;

    OSErr err = FSFindFolder(kOnAppropriateDisk,
                             kDesktopFolderType,
                             kCreateFolder, &ref);

    if(err != noErr)
    {
        return L"";
    }

    UInt8 path[32768];
    OSStatus s = FSRefMakePath(&ref, path, 32768);

    if(s != noErr)
    {
        return L"";
    }

    std::string strPath = (const char *) path;

    return PKStr::utf8StringToWstring(strPath);

#endif

#ifdef LINUX

	char *homeDir = getenv("HOME");

	std::string  homeStr  = std::string(homeDir);
	std::wstring homeWStr = PKStr::utf8StringToWstring(homeStr);
	
	return (homeWStr + L"/Desktop");

#endif

    return L"";
}

void PKPath::navigateToFolder(std::wstring folder)
{
#ifdef WIN32
    ShellExecute(NULL, L"open", folder.c_str(), L"", L"", SW_NORMAL);
#endif

#ifdef MACOSX
	
    std::string sfolder = PKStr::wStringToUTF8string(folder);
    std::string command = "open \"" + sfolder + "\"";

    system(command.c_str());

#endif
}

// GetFiles Stuff

struct caseInsensitiveStruct
{
    bool operator()(const std::wstring& arg0, const std::wstring& arg1) const
    {
#ifdef WIN32
        return _wcsicmp(arg0.c_str(), arg1.c_str()) < 0;
#endif

#if(defined(MACOSX) || defined(LINUX))
      return wcscmp(arg0.c_str(), arg1.c_str()) < 0;
#endif

    }
};

static void getFilesRecursiveInternal(std::vector<std::wstring>& result,
                                      std::wstring directory,
                                      const std::set<std::wstring, caseInsensitiveStruct>& fileTypeMap,
                                      bool recursive)
{
    // Construct the path/fileName

    std::wstring filePath = directory;

    wchar_t separator = PKPath::getPathSeparator()[0];

    if(filePath[filePath.size() - 1] != separator)
    {
        filePath += separator;
    }

    std::wstring fileNamePattern;
    std::wstring fileName;
    bool         isDir;

#ifdef WIN32
    HANDLE          handle;
    BOOL            ok;
    WIN32_FIND_DATA fd;
#endif

#ifdef LINUX
    struct dirent *nextDir = NULL;
    bool            ok;
    DIR *dirPtr = NULL;
#endif

#ifdef MACOSX
    struct dirent *nextDir = NULL;
    bool            ok;
    DIR *dirPtr = NULL;
#endif

    fileNamePattern = filePath + L'*';

    ok = 1;
    int filesFound = 0;

#ifdef WIN32
    handle = FindFirstFile(fileNamePattern.c_str(), &fd);

    if(handle != INVALID_HANDLE_VALUE)
    {
        fileName = fd.cFileName;
        isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#endif

#ifdef LINUX
    dirPtr = opendir(PKStr::wStringToString(directory.c_str()).c_str());
    nextDir = NULL;

    if(dirPtr != NULL)
    {
        nextDir = readdir(dirPtr);
    }

    if(  nextDir != NULL )
    {
        fileName = PKStr::stringToWString(nextDir->d_name);
        isDir = ((nextDir->d_type & DT_DIR) == DT_DIR);

#endif

#ifdef MACOSX

    CFStringRef stringRef = PKStr::wStringToCFString(directory);

    int directoryStringLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(stringRef),
                                                                  kCFStringEncodingUTF8);

    char *directoryString = new char[directoryStringLength];

    CFStringGetCString(stringRef, (char *)directoryString,
                       directoryStringLength, kCFStringEncodingUTF8);

    dirPtr = opendir(directoryString);
    CFRelease(stringRef);
    delete [] directoryString;

    nextDir = NULL;

    if(dirPtr != NULL)
    {
        nextDir = readdir(dirPtr);
    }
        
	if(  nextDir != NULL )
    {
        fileName = PKStr::stringToWString(nextDir->d_name);
        isDir = ((nextDir->d_type & DT_DIR) == DT_DIR);
#endif

        std::wstring fullPath = filePath + fileName;

        // check for . or ..

        if(fullPath[ (fullPath.size() - 1) ] != L'.')
        {
            if(isDir && recursive)
            {
                getFilesRecursiveInternal(result, fullPath, fileTypeMap, recursive);
            }
            else
            {
                // Get extension
                std::wstring::size_type dotIndex = fullPath.rfind(L".");

                if (dotIndex != std::wstring::npos)
                {
                    std::wstring ext = fullPath.substr(dotIndex + 1);

                    if (fileTypeMap.find(ext) != fileTypeMap.end())
                    {
                        result.push_back(fullPath);
                        filesFound++;
                    }
                }
            }
        }

        do
        {
#ifdef WIN32
            ok = FindNextFile(handle, &fd);
            if(ok == 1)
            {
                fileName = fd.cFileName;
                isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            }
#endif

#ifdef LINUX
            nextDir = readdir(dirPtr);
			
            if(nextDir != NULL)
            {
                fileName = PKStr::stringToWString(nextDir->d_name);
                isDir = ((nextDir->d_type & DT_DIR) == DT_DIR);

                ok = 1;
            }
            else
            {
                ok = 0;
            }
#endif

#ifdef MACOSX
            nextDir = readdir(dirPtr);
			
            if(nextDir != NULL)
            {
                stringRef = CFStringCreateWithCString(NULL,
                                                     (const char *) nextDir->d_name,
                                                      kCFStringEncodingUTF8);

                fileName = PKStr::cfStringToWString(stringRef);

                isDir = ((nextDir->d_type & DT_DIR) == DT_DIR);
                CFRelease(stringRef);

                ok = 1;
            }
            else
            {
                ok = 0;
            }
#endif

            if(ok != 1)
            {
                break;
            }

            fullPath = filePath + fileName;

            // check for . or ..
            if(fullPath[ (fullPath.size() - 1) ] != L'.')
            {
                if (isDir && recursive)
                {
                    getFilesRecursiveInternal(result, fullPath, fileTypeMap, recursive);
                }
                else
                {
                    // Get extension
                    std::wstring::size_type dotIndex = fullPath.rfind(L".");

                    if (dotIndex != std::wstring::npos)
                    {
                        std::wstring ext = fullPath.substr(dotIndex + 1);

                        if (fileTypeMap.find(ext) != fileTypeMap.end())
                        {
                            result.push_back(fullPath);
                            filesFound++;
                        }
                    }
                }
            }
        }
        while (true);
    }
		
#ifdef MACOSX
	if(dirPtr != NULL)
	{
		closedir(dirPtr);
	}
#endif

#ifdef LINUX
	if(dirPtr != NULL)
	{
		closedir(dirPtr);
	}
#endif		
		
#ifdef WIN32
    if(handle != INVALID_HANDLE_VALUE)
    {
        int closeStatus = FindClose(handle);
    }
#endif

}

std::vector<std::wstring> PKPath::getFiles(std::wstring directory,
                                              std::wstring fileTypes,
                                              bool recursive /* = false*/)
{
    std::vector<std::wstring> result;


    std::wstring delimiter = L",";
    std::vector<std::wstring> fileTypesList;

    PKStr::explode(fileTypes, delimiter[0], &fileTypesList);

    std::set <std::wstring, caseInsensitiveStruct> fileTypeMap;

    for (unsigned int i = 0; i < fileTypesList.size(); ++i)
    {
        fileTypeMap.insert(fileTypesList[i]);
    }

    // Normalize path in case incoming
    // directory's path separators are
    // switched

    std::wstring dir = PKStr::replaceChar(L'/', PKPath::getPathSeparator()[0], directory);

    getFilesRecursiveInternal(result, dir, fileTypeMap, recursive);

    return result;
}

std::wstring PKPath::getExtension(std::wstring fullPath)
{

#ifdef WIN32
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];

    _wsplitpath_s(fullPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );

    std::wstring extension = ext;

    return extension;
#endif

#if(defined(MACOSX) || defined(LINUX))

    int offset = fullPath.find_last_of(L'.');

    if(offset != std::string::npos)
    {
        return fullPath.substr(offset);
    }

    return fullPath;

#endif
}

std::wstring PKPath::getFileName(std::wstring fullPath)
{
#ifdef WIN32
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];

    _wsplitpath_s( fullPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext,_MAX_EXT );

    std::wstring fileName = fname;

    return fileName;
#endif

#if(defined(MACOSX) || defined(LINUX))

    int offset = fullPath.find_last_of(L'/');

    if(offset != std::string::npos)
    {
        std::wstring filename = fullPath.substr(offset+1);

        int dotoffset = filename.find_last_of(L'.');

        if(dotoffset != std::string::npos)
        {
            return filename.substr(0, dotoffset);
        }

        return filename;
    }

    return fullPath;

#endif
}

std::wstring PKPath::getFileNameAndExtension(std::wstring fullPath)
{

#ifdef WIN32
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];

    _wsplitpath_s( fullPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext,_MAX_EXT );

    std::wstring fileName = fname;
    std::wstring extension = ext;

    return fileName + ext;
#endif

#if(defined(MACOSX) || defined(LINUX))

    int offset = fullPath.find_last_of(L'/');

    if(offset != std::string::npos)
    {
        std::wstring filename = fullPath.substr(offset+1);
        return filename;
    }

    return fullPath;

#endif
}

std::wstring PKPath::getDirectoryName(std::wstring fullPath)
{
#ifdef WIN32
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];

    _wsplitpath_s( fullPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext,_MAX_EXT );

    std::wstring driveName = drive;
    std::wstring dirName = dir;

    return driveName + dirName;
#endif

#if(defined(MACOSX) || defined(LINUX))

    int offset = fullPath.find_last_of(L'/');

    if(offset != std::string::npos)
    {
        std::wstring filename = fullPath.substr(0, offset);
        return filename;
    }

    return L".";

#endif
}

//

#ifdef MACOSX

bool PKPath::getFSRefForPath(std::wstring path, FSRef *ref)
{
    std::vector <std::wstring> dirs;
    PKStr::explode(path, L'/', &dirs);

    OSStatus err = FSPathMakeRef((const UInt8 *) "/", ref, NULL);

    if(err != noErr)
    {
        return false;
    }

    for(unsigned int i=0; i < dirs.size(); i++)
    {
        uint32_t size;
        UniChar *ptr = NULL;

        PKStr::wStringToUniChar(dirs[i], &size, &ptr);

        if(ptr != NULL)
        {
            err = FSMakeFSRefUnicode(ref,
                                     size,
                                     ptr,
                                     kTextEncodingUnknown,
                                     ref);

            delete [] ptr;

            if(err != noErr)
            {
                return false;
            }
        }
    }

    return true;
}

#endif

