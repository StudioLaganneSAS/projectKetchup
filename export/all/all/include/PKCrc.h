//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_CRC_H
#define PK_CRC_H

#include "PKTypes.h"
#include <string>

class PKCrc
{
public:

    static uint32_t getFileCRC(std::wstring filename,
                               uint64_t     maxSize = 0);

    static uint32_t getBufferCRC(uint32_t crc,
                                 uint32_t bufferSize,
                                 void *buffer);

private:
	
	static bool inited;
	static void buildCRCTable();
};

#endif // PK_CRC_H
