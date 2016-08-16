//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
// 

#include "PKScratchBuffer.h"
#include "PKPath.h"

//
//
//

#define RW_BUFFER_SIZE 32768

//
//
//

PKScratchBuffer::PKScratchBuffer()
{
	this->size     = 0;
	this->buffer   = NULL;
	this->dataSize = 0;
	this->locked   = false;
	this->file     = NULL;
}

PKScratchBuffer::~PKScratchBuffer()
{
	// by precaution
	this->unlock();
	this->free();
}

bool PKScratchBuffer::alloc(uint32_t size, std::wstring scratch)
{
	if(size == 0)
	{
		return false;
	}

	if(this->locked)
	{
		return false;
	}

	if(this->file != NULL)
	{
		this->free();
	}

	this->buffer = NULL;

	std::wstring scratchPath = scratch;

	if(scratchPath == L"")
	{
		scratchPath = PKPath::getTempPath();	
	}

	if(PKPath::getSpaceAvailable(scratchPath) < size)
	{
		return false;
	}

	spath = scratchPath;

	// Create file object

	std::wstring tempFile = PKPath::getTempFileName(scratchPath);
	PKFile   *temp        = new PKFile();

	// Open the file

	if(temp->open(tempFile, PKFILE_MODE_READWRITE) != PKFILE_OK)
	{
		delete this->file;
		return false;
	}

	this->file = temp;

	// zero it

	uint8_t buffer[RW_BUFFER_SIZE];
	memset(buffer, 0, RW_BUFFER_SIZE);

	uint32_t blocks = size / RW_BUFFER_SIZE;
	uint32_t remain = size % RW_BUFFER_SIZE;

	for(uint32_t i=0; i < blocks; i++)
	{
		this->file->write(buffer, RW_BUFFER_SIZE); 
	}

	this->file->write(buffer, remain);

	// we're done

	this->filename = tempFile;
	this->size	   = size;

	return true;
}

bool PKScratchBuffer::reAlloc(uint32_t newSize)
{
	if(newSize == 0)
	{
		return false;
	}

	if(this->file == NULL)
	{
		return false;
	}

	if(this->locked == true)
	{
		return false;
	}

	uint32_t minsize = newSize > this->size ? this->size : newSize;

	// allocate new file

	std::wstring scratchPath = spath;

	if(scratchPath == L"")
	{
		scratchPath = PKPath::getTempPath();	
	}

	if(PKPath::getSpaceAvailable(scratchPath) < size)
	{
		return false;
	}

	// create file object

	std::wstring tempFile = PKPath::getTempFileName(scratchPath);
	PKFile   *temp        = new PKFile();

	// open the file

	if(temp->open(tempFile, PKFILE_MODE_READWRITE) != PKFILE_OK)
	{
		delete file;
		return false;
	}

	// copy over it

	uint8_t buffer[RW_BUFFER_SIZE];

	uint32_t blocks = minsize / RW_BUFFER_SIZE;
	uint32_t remain = minsize % RW_BUFFER_SIZE;

	uint32_t i;

	this->file->seek(0, PKFILE_SEEK_START);

	for(i=0; i < blocks; i++)
	{
		this->file->read(buffer, RW_BUFFER_SIZE);
		temp->write(buffer, RW_BUFFER_SIZE);
	}

	this->file->read(buffer, remain);
	temp->write(buffer, remain);

	if(newSize > this->size)
	{
		// zero the rest

		memset(buffer, 0, RW_BUFFER_SIZE);

		uint32_t blocks = (newSize - size) / RW_BUFFER_SIZE;
		uint32_t remain = (newSize - size) % RW_BUFFER_SIZE;

		uint32_t i;

		for(i=0; i < blocks; i++)
		{
			temp->write(buffer, RW_BUFFER_SIZE);
		}

		temp->write(buffer, remain);
	}

	// now clean up

	this->file->close();
	delete this->file;
	this->file = temp;

	PKPath::deleteFile(this->filename);

	this->filename = tempFile;
	this->size     = newSize;

	return true;
}

bool PKScratchBuffer::free()
{
	if(this->file == NULL)
	{
		return false;
	}

	if(this->locked)
	{
		return false;
	}

	this->file->close();
	delete this->file;
	this->file = NULL;

	if(!PKPath::deleteFile(this->filename))
	{
		return false;
	}

	this->filename = L"";
	this->size     = 0;
	this->dataSize = 0;
	this->locked   = false;

	return true;
}

bool PKScratchBuffer::isValid()
{
	return ((this->size != 0) && (this->file != NULL));
}

bool PKScratchBuffer::lock(void **buffer)
{
	this->mutex.wait();

	if(this->locked)
	{
		this->mutex.release();
		return false;
	}

	this->buffer = new uint8_t[this->size];

	if(this->buffer == NULL)
	{
		this->mutex.release();
		return false;
	}

	// read data

	this->file->seek(0, PKFILE_SEEK_START);
	this->file->read(this->buffer, this->size);

	this->locked = true;

	*buffer = this->buffer;
	this->mutex.release();

	return true;
}

bool PKScratchBuffer::unlock()
{
	this->mutex.wait();

	if(this->locked == false || 
	   this->buffer == NULL)
	{
	
		this->mutex.release();
		return false;
	}

	// write to file

	this->file->seek(0, PKFILE_SEEK_START);
	this->file->write(this->buffer, this->size);

	// delete our buffer

	delete [] (uint8_t *) this->buffer;
	this->buffer = NULL;

	this->locked = false;
	this->mutex.release();

	return true;
}

uint32_t PKScratchBuffer::getSize()
{
	return this->size;
}

uint32_t PKScratchBuffer::getDataSize()
{
	return this->dataSize;
}

void PKScratchBuffer::setDataSize(uint32_t size)
{
	this->dataSize = size;
}
