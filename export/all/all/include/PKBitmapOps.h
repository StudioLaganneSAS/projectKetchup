//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAPOPS_H
#define PK_BITMAPOPS_H

#include "PKTypes.h"
#include "PKUI.h"

#include "PKBitmapOpsFunctions.h"

#include <vector>

class PKImage;

//
// PKBitmapOps
//

class PKBitmapOps 
{
public:

	//
	// initialize()
	// ------------
	//
	// Initializes the bitmap ops class
	// regarding possible runtime CPU
	// optimizations...MUST be called first
	//
    
	static bool initialize();

    //
    // clearBitmap()
    // ------------
    //
    // Fills a PKImage with 
    // transparent pixels
    //

	static void clearBitmap(PKImage *bitmap);

    //
    // setPixel()
    // ----------
    //

    static void setPixel(unsigned int *pixel, PKColor color);

    //
    // fillCircle()
    // ------------
    //

	static void fillCircle(void     *buffer,
                           unsigned int wb,
                           unsigned int wh,
                           PKColor   color,
                           unsigned int  x,
                           unsigned int  y,
                           unsigned int  r);

	//
    // fillRect()
    // ----------
    //
    // Fills a rectangle with 
    // a single color...
    //

	static void fillRect(void     *buffer,
                         unsigned int wb,
                         unsigned int wh,
                         PKColor   color,
                         unsigned int  x = 0,
                         unsigned int  y = 0,
                         unsigned int  w = 0,
                         unsigned int  h = 0);

    static void fillRect(PKImage  *bitmap,
                         PKColor   color,
                         unsigned int  x = 0,
                         unsigned int  y = 0,
                         unsigned int  w = 0,
                         unsigned int  h = 0);
    
    //
    // gradientFil()
    // -------------
    //
    // Fills a rectangle with a
    // vertical or horizontal, 
    // two colors gradient...
    //

    static void gradientFill(void        *buffer,
                             unsigned int w,
							 unsigned int h,
							 int       orientation,
							 PKColor   color1,
							 PKColor   color2,
							 unsigned int  x  = 0,
							 unsigned int  y  = 0,
							 unsigned int  wi = 0,
							 unsigned int  hi = 0);
	
	static void gradientFill(PKImage  *bitmap,
                             int       orientation,
                             PKColor   color1,
                             PKColor   color2,
                             unsigned int  x = 0,
                             unsigned int  y = 0,
                             unsigned int  w = 0,
                             unsigned int  h = 0);
    
    
	//
	// flip()
	// ------
	//
	// Flips the bitmap horizontally
	// or vertically...
	//

	static void flip(PKImage  *source,
					 int       orientation);

	//
    // desaturate()
    // ------------
    //
    // Desaturate the bitmap with
    // an optional strength parameter
    //
    
    static void desaturate(PKImage *bitmap,
                           unsigned char strength = 255); // 0 - 255
    
    //
    // blur()
    // -----
    //
    // Alters the whole bitmap
	// to apply a 1-pixel blur effect...
    //

    static void blur(PKImage *bitmap); // in pixels

    //
    // blurRadius()
    // ------------
    //
    // Alters the whole bitmap
	// to apply a blur effect...
    //
    
    static void blurRadius(PKImage *bitmap,	
						   unsigned int radius); // in pixels

	//
	// blit()
	// ------
	//
	// Does a simple 24 bit blit
	//

	static void blit(void *bitmap,
                     unsigned int wBitmap,
                     unsigned int hBitmap,
                     int x,
                     int y,
                     PKImage *source, 
                     int sx = 0,
                     int sy = 0,
                     int sw = 0x00FFFFFF,
                     int sh = 0x00FFFFFF);

    static void blit(PKImage *bitmap,
                     int x,
                     int y,
                     PKImage *source, 
                     int sx = 0,
                     int sy = 0,
                     int sw = 0x00FFFFFF,
                     int sh = 0x00FFFFFF);

	//
	// alphaBlit(alpha)
	// ----------------
	//
	// Does a full 32 bit blit
	// with a global alpha modifier
	//

    static void alphaBlit(void *buffer,
						  unsigned char al,
						  int x,
						  int y,
						  int bw,
						  int bh,
						  PKImage *source, 
						  int sx = 0,
						  int sy = 0,
						  int sw = 0x00FFFFFF,
						  int sh = 0x00FFFFFF);
	
    static void alphaBlit(PKImage *bitmap,
                          unsigned char alpha,
                          int x,
                          int y,
                          PKImage *source, 
                          int sx = 0,
                          int sy = 0,
                          int sw = 0x00FFFFFF,
                          int sh = 0x00FFFFFF);
    
	//
	// alphaDataBlit(alpha)
	// --------------------
	//
	// Does a full 32 bit blit
	// with a global alpha modifier
	//

	static void alphaDataBlit(PKImage *bitmap,
							  unsigned char alpha,
							  int x,
							  int y,
                              void *pixels, 
                              int sx,
                              int sy,
                              int sw,
                              int sh);

    //
    // fillStretchWidth()
    // ------------------
    //
    // Stretches the source bitmap
    // in width by repeatingly filling
	// the destination with source pixels
	// from one or more offsets...
    //
    
	static void fillStretchWidth(void *dest,
                                 unsigned int w,
                                 unsigned int h,
								 PKImage *source,
								 std::vector <unsigned int> offsets);

	static void fillWidthWithStart(void *dest,
                                   unsigned int wToFill,
                                   unsigned int w,
                                   unsigned int h,
								   PKImage *source,
								   unsigned int offset);

	static void fillWidthWithEnd(void *dest,
								 unsigned int start,
                                 unsigned int wToFill,
                                 unsigned int w,
                                 unsigned int h,
								 PKImage *source,
								 unsigned int offset);

	static void fillStretchWidth(PKImage *dest,
								 PKImage *source,
								 std::vector <unsigned int> offsets);

    //
    // stretch()
    // ---------
    //
    // Stretches the source bitmap
    // to fill the destination...
    //
    
    static void stretch(PKImage *dest,
                        PKImage *source);
    
	//
    // Optimized function pointers
	//
	
	static AlphaBlitFunction     *alphaBlitFunc;
	static BlurFunction		     *blurFunc;
	static GradientLineFunction  *gradientLineFunc;
	static GradientColorFunction *gradientColorFunc;
	static DesaturateFunction    *desaturateFunc;

private:

	//
	// (Default) C Functions
	// 

	static void alphaBlitCFunction(unsigned char *src, 
								   unsigned char *dst, 
								   unsigned int  w, 
								   unsigned int  h, 
								   unsigned int  s_line, 
								   unsigned int  d_line,
								   unsigned char alpha);

	static void blurCFunction(unsigned char *dst, 
							  unsigned int  w, 
							  unsigned int  h);


	static void gradientLineCFunction(unsigned int *dst,
									  unsigned int  w,
									  unsigned char r1,
									  unsigned char g1,
									  unsigned char b1,
									  unsigned char r2,
									  unsigned char g2,
									  unsigned char b2);

	static void gradientColorCFunction(unsigned int *dst,
									   unsigned int  index,
								       unsigned int  width,
								       unsigned char r1,
								       unsigned char g1,
								       unsigned char b1,
								       unsigned char r2,
								       unsigned char g2,
								       unsigned char b2);

	static void desaturateCFunction(unsigned int *dst, 
									unsigned int  w, 
									unsigned int  h,
									unsigned char amount);

};

#endif // PK_BITMAPOPS_H

