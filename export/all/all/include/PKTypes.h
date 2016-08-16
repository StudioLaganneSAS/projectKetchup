//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKTYPES_H
#define PKTYPES_H

#ifdef LINUX
#include <string.h>
#endif

//
// This header defines the
// basic C-type types used
// in the project ketchup
// project tree and objects
//

//
// WINDOWS int types
//

#if(defined(WIN32) && defined(_MSC_VER))

#if(_MSC_VER >= 600) // VS6, VS2003, VS2005+

	// unsigned

    typedef unsigned __int64 uint64_t;
    typedef unsigned int     uint32_t;
    typedef unsigned __int16 uint16_t;
    typedef unsigned char    uint8_t;

	// signed

    typedef __int64 int64_t;
    typedef __int32 int32_t;
    typedef __int16 int16_t;
    typedef __int8  int8_t;

#pragma warning ( disable : 4267 4244 )

#else
#error "We don't support MSC compiler prior to VS6"
#endif // _MSV_VER

#endif // WIN32 & _MSC_VER

//
// MACOSX & LINUX int types
//

#if(defined(MACOSX) || defined(LINUX))

#ifdef __GNUC__

#include <sys/types.h>

    typedef u_int64_t uint64_t;

#ifndef __uint32_t_defined
#define __uint32_t_defined
    typedef u_int32_t uint32_t;
#endif

#ifndef __uint16_t_defined
#define __uint16_t_defined
    typedef u_int16_t uint16_t;
#endif

#ifndef __uint8_t_defined
#define __uint8_t_defined
    typedef u_int8_t uint8_t;
#endif

#else 
#error "Compiler not supported!"
#endif // __GNUC__

#endif // MACOSX || LINUX

// 
// Point, Rect and Color types
//

typedef struct _PKPoint {

	int32_t x;
	int32_t y;

} PKPoint;

typedef struct _PKRect {

	int32_t x;
	int32_t y;
	
	uint32_t w;
	uint32_t h;

} PKRect;

typedef struct _PKColor {

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

} PKColor;

typedef struct _PKInt32Range {

    int32_t min;
    int32_t max;

} PKInt32Range;

typedef struct _PKInt64Range {

    int64_t min;
    int64_t max;

} PKInt64Range;

typedef struct _PKDoubleRange {

    double min;
    double max;

} PKDoubleRange;

// 
// Utilities
//

PKPoint PKPointCreate(int32_t x, int32_t y);

PKRect PKRectCreate(int32_t x, int32_t y, uint32_t w, uint32_t h);

PKColor PKColorCreate(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

PKInt32Range PKInt32RangeCreate(int32_t min, int32_t max);

PKInt64Range PKInt64RangeCreate(int64_t min, int64_t max);

PKDoubleRange PKDoubleRangeCreate(double min, double max);

//
// Macros
//

#ifndef pk_min 
#define pk_min(a,b) ((a<b)?a:b)
#endif

#ifndef pk_max 
#define pk_max(a,b) ((a>b)?a:b)
#endif

#endif // PKTYPES_H

