//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_FONT_H
#define PK_FONT_H

#include "PKTypes.h"
#include "PKCritSec.h"
#include "PKImage.h"
#include "PKUI.h"

#include <string>
#include <vector>

#ifdef LINUX
#include <cairo.h>
#endif

//
// PKFont
//

class PKFont
{
public:

    static int32_t getSystemFontSize();

public:

    PKFont(); // Default system UI font

	PKFont(std::string  family,
		   unsigned int size,
		   bool			bold      = false,
		   bool			italic    = false,
		   bool			underline = false);

	PKFont(PKFont *copy);

	~PKFont();

	bool isValid();

	unsigned int getCharHeight();

	unsigned int getCharWidth(unsigned short c);
	unsigned int getTextWidth(std::wstring text);

	void setColor(unsigned char r, 
				  unsigned char g,
				  unsigned char b);

	void drawChar(unsigned short  c,
				  PKImage        *dst, 
				  int    x,
				  int    y,
                  unsigned char alpha = 255);

	void drawChar(unsigned short  c,
				  void           *dstBuffer, 
				  int    x,
				  int    y,
                  unsigned int bufferW,
                  unsigned int bufferH,
                  unsigned char alpha = 255);

    int drawText(std::wstring    text,
				 PKImage        *dst, 
				 int    x,
				 int    y,
                 unsigned char alpha = 255);

    int drawText(std::wstring    text,
				 void           *dstBuffer, 
				 int    x,
				 int    y,
                 unsigned int bufferW,
                 unsigned int bufferH,
                 unsigned char alpha = 255);

private:

	std::string  family;
	unsigned int size;
	bool		 bold;
	bool		 italic;
	bool		 underline;

#ifdef WIN32
	HFONT font;
#endif

#ifdef LINUX
    
#endif

	unsigned char r;
	unsigned char g;
	unsigned char b;
    
#ifdef MACOSX
    ATSUStyle      defaultStyle;
    ATSUTextLayout textLayout;
#endif

    PKCritSec mutex;
	unsigned int height;

	std::vector <unsigned short> cache_map;
	std::vector <PKImage *>      cache_bitmaps;

	std::vector <unsigned short> wcache_map;
	std::vector <unsigned int>   wcache_widths;

	int getCacheIndex(unsigned short c);
	int getWCacheIndex(unsigned short c);
};

#endif // PK_FONT_H
