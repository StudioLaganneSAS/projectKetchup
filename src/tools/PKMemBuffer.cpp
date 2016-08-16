//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKMemBuffer.h"
#include <stdio.h>
#include <string.h>

//
// PKMemBuffer
//

PKMemBuffer::PKMemBuffer()
{
	this->size     = 0;
	this->buffer   = NULL;
	this->dataSize = 0;
	this->locked   = false;
}

PKMemBuffer::~PKMemBuffer()
{
	// by precaution
	this->unlock();
	this->free();
}

bool PKMemBuffer::alloc(uint32_t size)
{
	if(size == 0)
	{
		return false;
	}

	if(this->locked)
	{
		return false;
	}

	if(this->buffer != NULL)
	{
		this->free();
	}

	this->buffer = new uint8_t[size];

	if(this->buffer == NULL)
	{
		return false;
	}

	memset(buffer, 0, size);
	this->size = size;

	return true;
}

bool PKMemBuffer::reAlloc(uint32_t newSize)
{
	if(newSize == 0)
	{
		return false;
	}

	if(this->locked)
	{
		return false;
	}

	if(this->buffer == NULL)
	{
		return false;
	}

	uint8_t *newBuffer = new uint8_t[newSize];

	if(newBuffer == NULL)
	{
		return false;
	}

	int s = newSize > this->size ? this->size : newSize;

	memcpy(newBuffer, this->buffer, s);

	if(newSize > this->size)
	{
		memset(newBuffer + s, 0, (newSize - this->size));
	}

	delete [] this->buffer;

	this->buffer = newBuffer;
	this->size   = newSize;

	return true;
}

bool PKMemBuffer::free()
{
	if(this->locked)
	{
		return false;
	}

	if(this->buffer == NULL)
	{
		return false;
	}

	delete [] this->buffer;
	this->buffer = NULL;

	return true;
}

bool PKMemBuffer::isValid()
{
	return ((this->size != 0) && (this->buffer != NULL));
}

bool PKMemBuffer::lock(void **buffer)
{
	this->mutex.wait();

	if(this->locked)
	{
		this->mutex.release();
		return false;
	}

	this->locked = true;

	*buffer = this->buffer;
	this->mutex.release();

	return true;
}

bool PKMemBuffer::unlock()
{
	this->mutex.wait();
	this->locked = false;
	this->mutex.release();

	return true;
}

uint32_t PKMemBuffer::getSize()
{
	return this->size;
}

uint32_t PKMemBuffer::getDataSize()
{
	return this->dataSize;
}

void PKMemBuffer::setDataSize(uint32_t size)
{
	this->dataSize = size;
}
