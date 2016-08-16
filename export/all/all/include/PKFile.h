//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_FILE_H
#define PK_FILE_H

#include "PKObject.h"

#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64
#endif

#include <string>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

//
// PKFileError
//

typedef enum 
{
	PKFILE_OK,
	
    PKFILE_ERROR_INVALIDPARAM,
    PKFILE_ERROR_INVALIDFILE,
	PKFILE_ERROR_NOTFOUND,
	PKFILE_ERROR_CANTOPEN,
	PKFILE_ERROR_ALREADYOPEN,
	
} PKFileError;

//
// PKFileOpenMode
//

typedef enum 
{
	PKFILE_MODE_READ,
	PKFILE_MODE_WRITE,
	PKFILE_MODE_READWRITE,
	PKFILE_MODE_APPEND,
	PKFILE_MODE_READAPPEND,
	
} PKFileOpenMode;

// Seeking method

typedef enum {

    PKFILE_SEEK_START,
    PKFILE_SEEK_CURRENT,
    PKFILE_SEEK_END,

} PKFileSeekMethod;

// Internal Handle

#ifdef WIN32
typedef HANDLE FileObject;
#endif

#ifdef LINUX
typedef FILE * FileObject;
#endif

#ifdef MACOSX
typedef FILE * FileObject;
#endif

//
// PKFile
//

class PKFile {

public:

	// PKFile + 

    PKFile();
    virtual ~PKFile(); 

	// Handle a file

	PKFileError open(std::wstring fileName, PKFileOpenMode mode);
	PKFileError close();

    bool isOpen();

    uint32_t read(void *buffer, uint32_t size);
    uint32_t write(void *buffer, uint32_t size);

    uint64_t tell();
    uint64_t seek(int64_t amount, PKFileSeekMethod method);

	uint64_t getFileSize();

	// Static Helpers
	
	static std::string readFile(std::wstring fileName);

	static bool saveStringToFile(std::string str, std::wstring fileName);
	static bool saveWStringToFile(std::wstring wstr, std::wstring fileName);

	static bool saveBufferToFile(void *buffer, 
								 uint32_t bufferSize, 
								 std::wstring fileName);

	static bool appendStringToFile(std::string str, std::wstring fileName);
	static bool appendWStringToFile(std::wstring wstr, std::wstring fileName);

private:
	
    FileObject   file;
};

#endif // PK_FILE_H

