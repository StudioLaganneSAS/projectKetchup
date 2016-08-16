//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFile.h"
#include "PKStr.h"

//
// PKFile
//

PKFile::PKFile()
{
    this->file = NULL;
}

PKFileError PKFile::open(std::wstring fileName, PKFileOpenMode mode)
{
	if(this->file != 0)
	{
		return PKFILE_ERROR_ALREADYOPEN;
	}

#ifdef WIN32

    DWORD access;
    DWORD share;
    DWORD create;
    DWORD flags = FILE_ATTRIBUTE_NORMAL;

    switch(mode)
    {
    case PKFILE_MODE_READ:
        {
            access = GENERIC_READ;
            share  = FILE_SHARE_READ | FILE_SHARE_WRITE;
            create = OPEN_EXISTING;
        }
        break;

    case PKFILE_MODE_WRITE:
        {
            access = GENERIC_WRITE;
            share  = FILE_SHARE_READ;
            create = CREATE_ALWAYS;
        }
        break;

    case PKFILE_MODE_READWRITE:
        {
            access = GENERIC_READ | GENERIC_WRITE;
            share  = 0;
            create = CREATE_ALWAYS;
        }
        break;

    case PKFILE_MODE_APPEND:
        {
            access = GENERIC_WRITE;
            share  = FILE_SHARE_READ;
            create = OPEN_ALWAYS;
        }
        break;

    case PKFILE_MODE_READAPPEND:
        {
            access = GENERIC_READ | GENERIC_WRITE;
            share  = 0;
            create = OPEN_ALWAYS;
        }
        break;

    default:
        {
            return PKFILE_ERROR_INVALIDPARAM;
        }
        break;
    }

    this->file = CreateFile(fileName.c_str(), access, share, NULL, create, flags, NULL);

    if(this->file == INVALID_HANDLE_VALUE)
    {
        return PKFILE_ERROR_CANTOPEN;
    }

    if(mode == PKFILE_MODE_READAPPEND ||
       mode == PKFILE_MODE_APPEND)
    {
        this->seek(0, PKFILE_SEEK_END);
    }

    return PKFILE_OK;

#endif

#if(defined(MACOSX) || defined(LINUX))

    std::string modeStr;
    std::string fileStr = PKStr::wStringToUTF8string(fileName);

    switch(mode)
    {
    case PKFILE_MODE_READ:
        {
            modeStr = "rb";
        }
        break;

    case PKFILE_MODE_WRITE:
        {
            modeStr = "wb";
        }
        break;

    case PKFILE_MODE_READWRITE:
        {
            modeStr = "w+b";
        }
        break;

    case PKFILE_MODE_APPEND:
        {
            modeStr = "ab";
        }
        break;

    case PKFILE_MODE_READAPPEND:
        {
            modeStr = "a+b";
        }
        break;

    default:
        {
            return PKFILE_ERROR_INVALIDPARAM;
        }
        break;
    }

    this->file = fopen(fileStr.c_str(), modeStr.c_str());

    if(this->file == NULL)
    {
        return PKFILE_ERROR_CANTOPEN;
    }

    if(mode == PKFILE_MODE_READAPPEND ||
       mode == PKFILE_MODE_APPEND)
    {
        this->seek(0, PKFILE_SEEK_END);
    }

    return PKFILE_OK;

#endif
}

PKFileError PKFile::close()
{	
#ifdef WIN32

    if(this->file != 0)
    {
        CloseHandle(this->file);
        this->file = 0;
    }

#endif

#if(defined(MACOSX) || defined(LINUX))

    if(this->file != NULL)
    {
        fclose(this->file);
        this->file = NULL;
    }

#endif

	return PKFILE_OK;
}

bool PKFile::isOpen()
{
    return (this->file != 0);	
}

uint32_t PKFile::read(void *buffer, uint32_t size)
{
    if(this->file == 0)
    {
        return 0;
    }

#ifdef WIN32

    DWORD bytesRead = 0;

    if(ReadFile(this->file, buffer, size, &bytesRead, NULL))
    {
        return bytesRead;
    }

    return 0;

#endif

#if(defined(MACOSX) || defined(LINUX))

    return fread(buffer, 1, size, this->file);

#endif
}

uint32_t PKFile::write(void *buffer, uint32_t size)
{
    if(this->file == 0)
    {
        return 0;
    }

#ifdef WIN32

    DWORD bytesWritten = 0;

    if(WriteFile(this->file, buffer, size, &bytesWritten, NULL))
    {
        return bytesWritten;
    }

    return 0;

#endif

#if(defined(MACOSX) || defined(LINUX))

    uint32_t amnt = fwrite(buffer, 1, size, this->file);
    fflush(this->file);

    return amnt;

#endif
}

uint64_t PKFile::tell()
{
    if(this->file == 0)
    {
        return -1;
    }

#ifdef WIN32

    LONG low;
    LONG high;

    low  = 0;
    high = 0;

    low = SetFilePointer(this->file, low, &high, FILE_CURRENT);

    uint64_t h = high;
    uint64_t l = low;

    h = (h << 32);
    l = l & 0xFFFFFFFF;
    h = h + l;

    return h;

#endif

#if(defined(MACOSX) || defined(LINUX))

    return (uint64_t) ftello(this->file);

#endif	
}

uint64_t PKFile::seek(int64_t amount, PKFileSeekMethod method)
{
    if(this->file == 0)
    {
        return -1;
    }

#ifdef WIN32

    LONG low;
    LONG high;

    low  =  amount        & 0xFFFFFFFF;
    high = (amount >> 32) & 0xFFFFFFFF;

    switch(method)
    {
    case PKFILE_SEEK_START:
        {
            low = SetFilePointer(this->file, low, &high, FILE_BEGIN);
        }
        break;

    case PKFILE_SEEK_CURRENT:
        {
            low = SetFilePointer(this->file, low, &high, FILE_CURRENT);
        }
        break;

    case PKFILE_SEEK_END:
        {
            low = SetFilePointer(this->file, low, &high, FILE_END);
        }
        break;

    default:
        {
            return -1;
        }
        break;
    }

    uint64_t h = high;
    uint64_t l = low;

    h = (h << 32);
    l = l & 0xFFFFFFFF;
    h = h + l;

    return h;

#endif

#if(defined(MACOSX) || defined(LINUX))

    switch(method)
    {
    case PKFILE_SEEK_START:
        {
            if(fseeko(this->file, (off_t) amount, SEEK_SET) == 0)
            {
                return this->tell();
            }
            else
            {
                return -1;
            }
        }
        break;

    case PKFILE_SEEK_CURRENT:
        {
            if(fseeko(this->file, (off_t) amount, SEEK_CUR) == 0)
            {
                return this->tell();
            }
            else
            {
                return -1;
            }
        }
        break;

    case PKFILE_SEEK_END:
        {
            if(fseeko(this->file, (off_t) amount, SEEK_END) == 0)
            {
                return this->tell();
            }
            else
            {
                return -1;
            }
        }
        break;

    default:
        {
            return -1;
        }
        break;
    }

#endif
}

uint64_t PKFile::getFileSize()
{
    uint64_t size;
    uint64_t current = this->tell();

    if(current == -1)
    {
        return PKFILE_ERROR_INVALIDFILE;
    }

    this->seek(0, PKFILE_SEEK_END);

    size = this->tell();

    this->seek(current, PKFILE_SEEK_START);

    return size;
}


PKFile::~PKFile()
{
    //
    // Safety catch
    //

    if(this->file != 0)
    {
        this->close();
    }

    this->file = 0;
}

//
// Static Helpers
//

std::string PKFile::readFile(std::wstring fileName)
{
	std::string result = "";

	PKFile *file = new PKFile();

	if(file == NULL)
	{
		return result;
	}

	if(file->open(fileName, PKFILE_MODE_READ) == PKFILE_OK)
	{
		uint64_t size = file->getFileSize();
	
		// Restrict this to file < 10MB
		if(size > 10*1024*1024)
		{
			file->close();
			return result;
		}
		
		char *buffer = new char[(uint32_t)size];
		file->read(buffer, (uint32_t)size);

		result = std::string(buffer, (uint32_t)size);

		file->close();
		delete [] buffer;
	}

	return result;
}

bool PKFile::saveStringToFile(std::string str, std::wstring fileName)
{
	PKFile *file = new PKFile();
	bool written = false;
	
	if(file == NULL) 
	{
		return false;
	}
	
	if(str.length() == 0)
	{
		delete file;
		return false;
	}
	
	if(file->open(fileName, PKFILE_MODE_WRITE) == PKFILE_OK)
	{
		uint32_t wrts = str.length()*sizeof(char);
		uint32_t size = file->write((void *) str.c_str(), wrts);
		
		if(size == wrts)
		{
			written = true;
		}
	}
	
	delete file;
	
	return written;
}

bool PKFile::saveWStringToFile(std::wstring wstr, std::wstring fileName)
{
	PKFile *file = new PKFile();
	bool written = false;
	
	if(file == NULL) 
	{
		return false;
	}
	
	if(wstr.length() == 0)
	{
		delete file;
		return false;
	}
	
	if(file->open(fileName, PKFILE_MODE_WRITE) == PKFILE_OK)
	{
		uint32_t wrts = wstr.length()*sizeof(wchar_t);
		uint32_t size = file->write((void *) wstr.c_str(), wrts);
		
		if(size == wrts)
		{
			written = true;
		}
	}
	
	delete file;
	
	return written;	
}

bool PKFile::saveBufferToFile(void *buffer, 
							  uint32_t bufferSize, 
							  std::wstring fileName)
{
	PKFile *file = new PKFile();
	bool written = false;
	
	if(file == NULL) 
	{
		return false;
	}
	
	if(buffer == NULL || bufferSize == 0)
	{
		delete file;
		return false;
	}
	
	if(file->open(fileName, PKFILE_MODE_WRITE) == PKFILE_OK)
	{
		uint32_t size = file->write((void *) buffer, bufferSize);
		
		if(size == bufferSize)
		{
			written = true;
		}
	}
	
	delete file;
	
	return written;
}

bool PKFile::appendStringToFile(std::string str, std::wstring fileName)
{
	PKFile *file = new PKFile();
	bool written = false;
	
	if(file == NULL) 
	{
		return false;
	}
	
	if(str.length() == 0)
	{
		delete file;
		return false;
	}
	
	if(file->open(fileName, PKFILE_MODE_APPEND) == PKFILE_OK)
	{
		uint32_t wrts = str.length()*sizeof(char);
		uint32_t size = file->write((void *) str.c_str(), wrts);
		
		if(size == wrts)
		{
			written = true;
		}
	}
	
	delete file;
	
	return written;
	
}

bool PKFile::appendWStringToFile(std::wstring wstr, std::wstring fileName)
{
	PKFile *file = new PKFile();
	bool written = false;
	
	if(file == NULL) 
	{
		return false;
	}
	
	if(wstr.length() == 0)
	{
		delete file;
		return false;
	}
	
	if(file->open(fileName, PKFILE_MODE_APPEND) == PKFILE_OK)
	{
		uint32_t wrts = wstr.length()*sizeof(wchar_t);
		uint32_t size = file->write((void *) wstr.c_str(), wrts);
		
		if(size == wrts)
		{
			written = true;
		}
	}
	
	delete file;
	
	return written;		
}

