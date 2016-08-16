//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAPOPS_FUNCTIONS_H
#define PK_BITMAPOPS_FUNCTIONS_H

#include "PKTypes.h"

/*
 *
 *
 */

typedef void (AlphaBlitFunction(unsigned char *src, 
								unsigned char *dst, 
								unsigned int  w, 
								unsigned int  h, 
								unsigned int  s_line, 
								unsigned int  d_line,
								unsigned char alpha));

typedef void (BlurFunction(unsigned char *dst, 
						   unsigned int  w, 
						   unsigned int  h));


typedef void (GradientLineFunction(unsigned int *dst,
								   unsigned int  width,
								   unsigned char r1,
								   unsigned char g1,
								   unsigned char b1,
								   unsigned char r2,
								   unsigned char g2,
								   unsigned char b2));

typedef void (GradientColorFunction(unsigned int *dst,
								    unsigned int  index,
								    unsigned int  width,
								    unsigned char r1,
								    unsigned char g1,
								    unsigned char b1,
								    unsigned char r2,
								    unsigned char g2,
								    unsigned char b2));

typedef void (DesaturateFunction(unsigned int *dst, 
								 unsigned int  w, 
								 unsigned int  h,
								 unsigned char amount));


#endif // PK_BITMAPOPS_FUNCTIONS_H

