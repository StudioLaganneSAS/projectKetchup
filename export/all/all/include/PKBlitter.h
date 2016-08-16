//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BLITTER_H
#define PK_BLITTER_H

//
// Includes
//

#include "PKTypes.h"
#include "PKUI.h"

//
// PKBlitter
//

class PKBlitter
{
public:

    //
    // Blit
    //

	static void blitRGB32At(PKDrawingDevice device, 
						    void *bitmap,
						    unsigned int x,
						    unsigned int y,
						    unsigned int w,
						    unsigned int h);
};

#endif // PK_BLITTER_H