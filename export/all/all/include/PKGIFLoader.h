//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_GIFLOADER_H
#define PK_GIFLOADER_H

#include "PKByteReader.h"

#ifdef WIN32
#include <olectl.h>
#include <Rpc.h>
#endif

#ifdef MACOSX
#include <CGImage.h>
#include <CGBitmapContext.h>
#include <Movies.h>
#include <ImageCompression.h>
#endif

#ifdef LINUX
#include <stdlib.h>
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

//
// PKGIFLoader
//

class PKGIFLoader 
{
public:

	PKGIFLoader();
	virtual ~PKGIFLoader();

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

#endif // PK_GIFLOADER_H

