//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKBASE64_H
#define PKBASE64_H

#include "PKTypes.h"
#include <string>

//
// PKBase64
//

class PKBase64 {

public:

	static std::string encode(char *str, int length);
	static unsigned char* decode(std::string str, int* size);

private:

	static std::string encodeBlock(char *block, int size);
    static void   decodeBlock(std::string block, char *out);
    
    static int charValue(char c);
};

#endif // PKBASE64_H

