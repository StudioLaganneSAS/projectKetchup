//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_UTFCONV_H
#define PK_UTFCONV_H

#include "PKTypes.h"
#include <string>

// Utilities

int UTF8toUTF16(const unsigned char *utf08, unsigned int length, unsigned short *utf16, unsigned int maxout, bool &hasBOM);

int UTF16toUTF8(const unsigned short *utf16, unsigned int length, unsigned char *utf08, unsigned int maxout, bool useBOM);

int LATIN1toUTF8(const unsigned char *latin1, unsigned int length, unsigned char **utf8, bool useBOM);

// MACOSX specific

int UTF8ToWchart(const unsigned char *utf08, unsigned int length, wchar_t *wchart, unsigned int maxout, bool &hasBOM);

int WchartToUTF8(const wchar_t *wchart, unsigned int length, unsigned char *utf08, unsigned int maxout, bool useBOM);

#endif // PK_UTFCONV_H
