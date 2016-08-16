//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_IMAGE_H_
#define PK_IMAGE_H_

#include "PKTypes.h"

class PKPNGLoader;
class PKJPGLoader;
class PKGIFLoader;

#include <string>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

#ifdef LINUX
#include <gtk/gtk.h>
#include <stdlib.h>
#endif


//
// PKImageURLType
//

typedef enum {
	
	PKIMAGE_URI_FILENAME,
	PKIMAGE_URI_RESOURCE,
	PKIMAGE_URI_MPOINTER,
	PKIMAGE_URI_UNKNOWN,
	
} PKImageURIType;

//
// PKImage
//

class PKImage {

public:

	PKImage();
	PKImage(PKImage *copy);
    
	virtual ~PKImage();

	void	 copy(PKImage *source);

	bool     create(unsigned int width, 
					unsigned int height);

	bool     destroy();

	bool     load(std::wstring filename);
	bool     load(char *buffer, int size);

	bool     unload();

	char    *getPixels();
	
	unsigned int getWidth();
	unsigned int getHeight();
	
    void copy(uint32_t  fromX,
              uint32_t  fromY,
              char     *toBuffer,
              uint32_t  toX,
              uint32_t  toY,
              uint32_t  width,
              uint32_t  height);

#ifdef MACOSX
	CGImageRef getCGImageRef();
#endif

public:
	
	static void processURI(std::wstring    URI,
						   PKImageURIType *type,
						   std::wstring   *img);
	
private:

	PKPNGLoader *pngLoader;
	PKJPGLoader *jpgLoader;
	PKGIFLoader *gifLoader;

	char              *pixels;
	unsigned int	   width;
	unsigned int	   height;
};

#endif // PK_IMAGE_H_
