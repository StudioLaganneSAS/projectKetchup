//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFDATABUFFER_H
#define PKFDATABUFFER_H

#include "PKFTypes.h"

//
// PKFDataBuffer
// 

class PKFDataBuffer 
{
public:    

    PKFDataBuffer(PKFDataBufferType type = PKF_DATA_BUFFER_MEMORY);
    ~PKFDataBuffer();

	PKFDataBufferType getBufferType();

    PKFReturnCode alloc(uint32_t size);
    PKFReturnCode reAlloc(uint32_t newSize);
    PKFReturnCode free();

    bool isValid();

    bool lock(void **buffer);
	bool unlock();

    uint32_t   getBufferSize();

    uint32_t   getDataSize();
    void       setDataSize(uint32_t size);

private:    
	
	PKFDataBufferType type;
	void *internalBuffer;
};

#endif // PKFDATABUFFER_H
