//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_IMAGE_LOADER_H
#define PK_IMAGE_LOADER_H

#include "PKImage.h"
#include <string>
#include "PKCritSec.h"

//
// PKImageLoader
//

class PKImageLoader 
{
public:

	static PKImage *loadBitmapFromResource(std::wstring bitmapId);
	static PKImage *loadBitmapFromFilename(std::wstring filename);
	
private:

	static PKCritSec mutex;
};

#endif // PK_IMAGE_LOADER_H