//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKFDataBuffer.h"
#include "PKMemBuffer.h"
#include "PKScratchBuffer.h"
#include "PKFSampleManager.h"

//
// PKDataBuffer
//

PKFDataBuffer::PKFDataBuffer(PKFDataBufferType type)
{
	this->type = type;

	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			this->internalBuffer = (void *) new PKMemBuffer();
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			this->internalBuffer = (void *) new PKScratchBuffer();
		}
		break;
	}
}

PKFDataBuffer::~PKFDataBuffer()
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			delete (PKMemBuffer *) this->internalBuffer;
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			delete (PKScratchBuffer *) this->internalBuffer;
		}
		break;
	}
}

PKFDataBufferType PKFDataBuffer::getBufferType()
{
	return this->type;
}

PKFReturnCode PKFDataBuffer::alloc(uint32_t size)
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			if(((PKMemBuffer *) this->internalBuffer)->alloc(size))
			{
				return PKF_SUCCESS;
			}
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			if(((PKScratchBuffer *) this->internalBuffer)->alloc(size, PKFSampleManager::getScratchPath()))
			{
				return PKF_SUCCESS;
			}
		}
		break;
	}

	return PKF_ERR_NOT_READY;
}

PKFReturnCode PKFDataBuffer::reAlloc(uint32_t newSize)
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			if(((PKMemBuffer *) this->internalBuffer)->reAlloc(newSize))
			{
				return PKF_SUCCESS;
			}
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			if(((PKScratchBuffer *) this->internalBuffer)->reAlloc(newSize))
			{
				return PKF_SUCCESS;
			}			
		}
		break;
	}

	return PKF_ERR_NOT_READY;
}

PKFReturnCode PKFDataBuffer::free()
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			if(((PKMemBuffer *) this->internalBuffer)->free())
			{
				return PKF_SUCCESS;
			}
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			if(((PKScratchBuffer *) this->internalBuffer)->free())
			{
				return PKF_SUCCESS;
			}
		}
		break;
	}

	return PKF_ERR_NOT_READY;
}

bool PKFDataBuffer::isValid()
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			return ((PKMemBuffer *) this->internalBuffer)->isValid();
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			return ((PKScratchBuffer *) this->internalBuffer)->isValid();
		}
		break;
	}

	return false;
}

bool PKFDataBuffer::lock(void **buffer)
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			return ((PKMemBuffer *) this->internalBuffer)->lock(buffer);
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			return ((PKScratchBuffer *) this->internalBuffer)->lock(buffer);
		}
		break;
	}

	return false;
}

bool PKFDataBuffer::unlock()
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			return ((PKMemBuffer *) this->internalBuffer)->unlock();
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			return ((PKScratchBuffer *) this->internalBuffer)->unlock();
		}
		break;
	}

	return false;
}

uint32_t PKFDataBuffer::getBufferSize()
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			return ((PKMemBuffer *) this->internalBuffer)->getSize();
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			return ((PKScratchBuffer *) this->internalBuffer)->getSize();
		}
		break;
	}

	return PKF_ERR_NOT_READY;
}

uint32_t PKFDataBuffer::getDataSize()
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			return ((PKMemBuffer *) this->internalBuffer)->getDataSize();
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			return ((PKScratchBuffer *) this->internalBuffer)->getDataSize();
		}
		break;
	}

	return PKF_ERR_NOT_READY;
}

void PKFDataBuffer::setDataSize(uint32_t size)
{
	switch(this->type)
	{
	case PKF_DATA_BUFFER_MEMORY:
		{
			((PKMemBuffer *) this->internalBuffer)->setDataSize(size);
		}
		break;
	
	case PKF_DATA_BUFFER_SCRATCH:
		{
			((PKScratchBuffer *) this->internalBuffer)->setDataSize(size);
		}
		break;
	}
}
