//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKCrc.h"
#include "PKFile.h"

// Statics

bool PKCrc::inited = false;

// CRC table constants

#define PKCRC32_POLYNOMIAL 0xEDB88320L
unsigned long PKCRCTable[256];

// Static init

void PKCrc::buildCRCTable()
{
	if(!PKCrc::inited)
	{
    	//
    	// Build CRC Table
    	//

    	int i;
    	int j;

    	unsigned long c;

    	for ( i = 0; i <= 255 ; ++ i )
    	{
        	c = i;

        	for ( j = 8 ; j > 0; -- j )
        	{
            	if(c & 1)
            	{
                	c = ( c >> 1 ) ^ PKCRC32_POLYNOMIAL;
            	}
            	else
            	{
                	c >>= 1;
            	}
        	}

        	PKCRCTable[ i ] = c;
    	}
		
		PKCrc::inited = true;
	}	
}


// PKCrc class

uint32_t PKCrc::getFileCRC(std::wstring filename,
                           uint64_t maxSize)
{
    PKFile *file = new PKFile();

    if(file->open(filename, PKFILE_MODE_READ) != PKFILE_OK)
    {
		delete file;
        return 0;
    }

	// Init

	PKCrc::buildCRCTable();

    // Limit to first X bytes

    uint64_t fsize = file->getFileSize();
    uint32_t  size = (uint32_t) fsize;

    if(maxSize != 0)
    {
        size = (uint32_t) (maxSize < fsize ? maxSize : fsize);
    }

    //
    // Read file
    //

    uint32_t      count = 0;
    uint32_t      total = 0;

    unsigned char buffer[512];
    uint32_t      crc = 0xFFFFFFFL;

    while(total < size)
    {
        count = file->read(buffer, 512);

        if(count == 0)
        {
            break;
        }

        crc = PKCrc::getBufferCRC(crc, count, buffer);
        total += count;
    }

    file->close();
	delete file;

    return (crc ^= 0xFFFFFFFFL);
}

uint32_t PKCrc::getBufferCRC(uint32_t  crc,
                             uint32_t  bufferSize,
                             void      *buffer)
{
	if(buffer == NULL)
	{
		return 0;
	}
	
    unsigned char *p;
    unsigned long temp1;
    unsigned long temp2;

	// Init

	PKCrc::buildCRCTable();

    p = (unsigned char*) buffer;

    for(unsigned int i=0; i < bufferSize; i++)
    {
        temp1 = ( crc >> 8 ) & 0x00FFFFFFL;

        temp2 = PKCRCTable[((int) crc ^ *p++ ) & 0xFF];

        crc = temp1 ^ temp2;
    }

    return crc;
}
