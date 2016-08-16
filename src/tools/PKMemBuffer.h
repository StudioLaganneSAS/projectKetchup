//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_MEMORYBUFFER_H
#define PK_MEMORYBUFFER_H

#include "PKTypes.h"
#include "PKCritSec.h"

class PKMemBuffer
{
public:    

    PKMemBuffer();
    ~PKMemBuffer();

    bool alloc(uint32_t size);
    bool reAlloc(uint32_t newSize);
    bool free();

    bool isValid();

    bool lock(void **buffer);
	bool unlock();

    uint32_t   getSize();

    uint32_t   getDataSize();
    void       setDataSize(uint32_t size);

private:    
    uint32_t   size;
    uint32_t   dataSize;
    uint8_t   *buffer;
	PKCritSec  mutex;
	bool	   locked;
};

#endif // PK_MEMORYBUFFER_H
