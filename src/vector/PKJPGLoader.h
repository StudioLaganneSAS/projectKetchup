//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_JPGLOADER_H
#define PK_JPGLOADER_H

#include "PKByteReader.h"

#if(defined(WIN32) || defined(LINUX))
#include <stdlib.h>
extern "C" {
#include "jpeglib.h"
}	
#endif

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
#  define MIN(a,b)  ((a) < (b)? (a) : (b))
#endif

/*
 * 
 * 
 */

class PKJPGLoader {

public:

	PKJPGLoader();
	virtual ~PKJPGLoader();

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

	unsigned int width;
	unsigned int height;
	
	int channels;
	int bit_depth;
	int color_type;

	bool load(PKByteReader *reader);
	
#ifdef MACOSX
	CGImageRef cgImageRef;
#endif
};

#endif // PK_JPGLOADER_H

