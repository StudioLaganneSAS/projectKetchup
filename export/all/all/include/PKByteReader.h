//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BYTE_READER_H
#define PK_BYTE_READER_H

#include "PKTypes.h"
#include "PKStr.h"
#include "PKFile.h"

class PKByteReader 
{
public:

    PKByteReader();
    ~PKByteReader();

    bool open(std::wstring filename);
    bool open(char *buffer, int size);

	void skip(int bytes);

    bool seek(uint64_t position);
    int32_t  read(char *buffer, int32_t size);

    bool close();

private:

    PKFile *input;
    char*   buffer;
    int     bufferSize;
    int     bufferPos;
};

#endif // PK_BYTE_READER_H
