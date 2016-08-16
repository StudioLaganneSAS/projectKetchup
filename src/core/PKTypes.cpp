//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKTypes.h"

// 
// Utilities
//

PKPoint PKPointCreate(int32_t x, int32_t y)
{
	PKPoint result;

	result.x = x;
	result.y = y;

	return result;
}

PKRect PKRectCreate(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	PKRect result;

	result.x = x;
	result.y = y;
	result.w = w;
	result.h = h;

	return result;
}

PKColor PKColorCreate(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	PKColor result;

	result.r = r;
	result.g = g;
	result.b = b;
	result.a = a;

	return result;
}

PKInt32Range PKInt32RangeCreate(int32_t min, int32_t max)
{
    PKInt32Range result;

    result.min = min;
    result.max = max;

    return result;
}

PKInt64Range PKInt64RangeCreate(int64_t min, int64_t max)
{
    PKInt64Range result;

    result.min = min;
    result.max = max;

    return result;
}

PKDoubleRange PKDoubleRangeCreate(double min, double max)
{
    PKDoubleRange result;

    result.min = min;
    result.max = max;

    return result;
}
