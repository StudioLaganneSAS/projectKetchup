//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_SCRATCHBUFFER_H
#define PK_SCRATCHBUFFER_H

#include "PKTypes.h"
#include "PKCritSec.h"
#include "PKFile.h"

//
// PKScratchBuffer
//

class PKScratchBuffer 
{
public:    

    PKScratchBuffer();
    ~PKScratchBuffer();

	bool alloc(uint32_t size, std::wstring scratchPath);
    bool reAlloc(uint32_t newSize);
    bool free();

    bool isValid();

    bool lock(void **buffer);
	bool unlock();

    uint32_t   getSize();
    uint32_t   getDataSize();
    void       setDataSize(uint32_t size);

private:    
    uint32_t     size;
    uint32_t	 dataSize;
    PKFile		*file;
	std::wstring filename;
	std::wstring spath;
	void		*buffer;
	PKCritSec    mutex;
	bool	     locked;
};

#endif //SCRATCHBUFFER_H
