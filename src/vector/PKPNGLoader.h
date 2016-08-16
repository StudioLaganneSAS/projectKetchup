//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_PNGLOADER_H
#define PK_PNGLOADER_H

#include "PKByteReader.h"

#include <png.h>
#include <stdlib.h>

#ifdef MACOSX
#include <CGImage.h>
#include <CGBitmapContext.h>
#endif

#include <string>

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) > (b)? (a) : (b))
#endif 

#ifndef MIN
#  define MIN(a,b)  ((a) < (b)? (a) : (b))
#endif

typedef unsigned char   uch;
typedef unsigned short  ush;
typedef unsigned long   ulg;

//
// PKPNGLoader
//

class PKPNGLoader {

public:

	PKPNGLoader();
	virtual ~PKPNGLoader();

	bool     load(std::wstring filename);
	bool     load(char *buffer, int size);

	bool     unload();

	char    *getPixels();

	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getChannels();
	
#ifdef MACOSX
	CGImageRef getCGImageRef();
#endif

private:

	char   *pixels;

	png_structp png_ptr;
	png_infop   info_ptr;

	png_uint_32 width;
	png_uint_32 height;

#ifdef MACOSX
	CGImageRef cgImage;
#endif
	
	int channels;
	int bit_depth;
	int color_type;

	bool load(PKByteReader *reader);
};

#endif // PK_PNGLOADER_H
