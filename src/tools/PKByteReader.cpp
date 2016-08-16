//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//


#include "PKByteReader.h"

//
// PKByteReader
//

PKByteReader::PKByteReader()
{
	this->input      = NULL;
	this->buffer     = NULL;
	this->bufferSize = 0;
	this->bufferPos  = 0;
}

PKByteReader::~PKByteReader()
{
	// Safety catch
	this->close();
}

bool PKByteReader::open(std::wstring filename)
{
	this->input = new PKFile();

	if(this->input == NULL)
	{
		return false;
	}

	PKFileError err = this->input->open(filename, PKFILE_MODE_READ);

	if(err == PKFILE_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool PKByteReader::open(char *buffer, int size)
{
	this->buffer     = buffer;
	this->bufferSize = size;
	this->bufferPos  = 0;

	return true;	
}

bool PKByteReader::seek(uint64_t position)
{
	if(this->input != NULL)
	{
		uint64_t result = this->input->seek(position, PKFILE_SEEK_START);

		if(result == position)
		{
			return true;
		}
	}

	if(this->buffer != NULL)
	{
		if(position >= 0 && position < this->bufferSize)
		{
			this->bufferPos = (int) position;
			return true;
		}
	}

	return false;
}

void PKByteReader::skip(int bytes)
{
	if(this->input != NULL)
	{
		this->input->seek(bytes, PKFILE_SEEK_CURRENT);
	}

	if(this->buffer != NULL)
	{
		if((this->bufferPos + bytes) < this->bufferSize)
		{
			this->bufferPos += bytes;
		}
	}
}

int32_t PKByteReader::read(char *buf, int32_t size)
{
	if(this->input != NULL)
	{
		return this->input->read(buf, size);
	}

	if(this->buffer != NULL)
	{
		if(this->bufferPos + size < this->bufferSize)
		{
			memcpy(buf, this->buffer + this->bufferPos, size);
			this->bufferPos += size;
			
			return size;
		}
		else
		{
			int left = this->bufferSize - this->bufferPos;

			memcpy(buf, this->buffer + this->bufferPos, left);
			this->bufferPos += left;

			return left;
		}
	}

	return 0;
}

bool PKByteReader::close()
{
	if(this->input != NULL)
	{
		this->input->close();
		delete this->input;
		this->input = NULL;
	}	

	if(this->buffer != NULL)
	{
		this->buffer = NULL;
	}

	return true;
}

