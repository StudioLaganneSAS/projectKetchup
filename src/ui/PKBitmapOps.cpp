//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCpu.h"
#include "PKBitmapOps.h"
#include "PKImage.h"

#include <math.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef LINUX
#define abs(a) (a < 0 ? -a : a)
#endif
/*
#ifndef x64
#if(defined(X86) || defined(__i386__))
#include "PKBitmapOpsX86.h"
#endif
#endif

#ifdef __ppc__
#include "PKBitmapOpsPPC.h"
#endif
*/
//
// PKBitmapOps
//

/*
 * Initialize to the default C
 * functions (unoptimized)
 *
 */

AlphaBlitFunction     *PKBitmapOps::alphaBlitFunc     = PKBitmapOps::alphaBlitCFunction;
BlurFunction	      *PKBitmapOps::blurFunc          = PKBitmapOps::blurCFunction;
GradientLineFunction  *PKBitmapOps::gradientLineFunc  = PKBitmapOps::gradientLineCFunction; 
GradientColorFunction *PKBitmapOps::gradientColorFunc = PKBitmapOps::gradientColorCFunction;
DesaturateFunction    *PKBitmapOps::desaturateFunc    = PKBitmapOps::desaturateCFunction;

/*
 *
 */

bool PKBitmapOps::initialize()
{
	PKCpuType  type  = PKCpu::getCpuType();
	PKCpuFlags flags = PKCpu::getCpuFlags();

	/*

#ifndef x64
#ifdef WIN32
#if(defined(__i386__) || defined(X86))

	if(type == PK_CPU_TYPE_X86)
	{
		if(flags & PK_CPU_FLAGS_MMX)
		{
			PKBitmapOps::alphaBlitFunc     = _alphaBlitMMX;
			PKBitmapOps::blurFunc	       = _blurMMX;
			PKBitmapOps::gradientLineFunc  = _gradientLineMMX;
			PKBitmapOps::gradientColorFunc = _gradientColorMMX;
		}

		if(flags & PK_CPU_FLAGS_SSE)
		{
			PKBitmapOps::alphaBlitFunc     = _alphaBlitSSE;
			PKBitmapOps::desaturateFunc    = _desaturateSSE;
		}
	}

#endif // __i386__
#endif // WIN32
#endif // x64

	*/

	return true;
}

//
// clearBitmap()
// ------------
//
// Fills an PKImage with 
// transparent pixels
//

void PKBitmapOps::clearBitmap(PKImage *bitmap)
{
    if(bitmap == NULL)
    {
        return;
    }
    
	unsigned int w = bitmap->getWidth();
	unsigned int h = bitmap->getHeight();

    char *dst = bitmap->getPixels();
    
	if(dst == NULL)
	{
		return;
	}

	memset(dst, 0, w*h*4);
}

//
// setPixel()
// ----------
//

void PKBitmapOps::setPixel(unsigned int *pixel, PKColor color)
{
    if(pixel == NULL)
    {
        return;
    }

    unsigned char *dd = (unsigned char *) pixel;

#ifdef MACOSX
    dd[0] = color.a;
    dd[1] = color.r;
    dd[2] = color.g;
    dd[3] = color.b;
#endif

#ifdef WIN32
    dd[3] = color.a;
    dd[2] = color.r;
    dd[1] = color.g;
    dd[0] = color.b;
#endif
}
	
//
// fillCircle()
// ------------
//

void PKBitmapOps::fillCircle(void     *buffer,
                             unsigned int wb,
                             unsigned int wh,
                             PKColor   color,
                             unsigned int  x,
                             unsigned int  y,
                             unsigned int  r)
{
    if(buffer == NULL)
    {
        return;
    }

    int tx = x - r;
    int ty = y - r;
    int tw = 2*r;
    int th = 2*r;

    char *dst = (char *) buffer;
    
	if(dst == NULL)
	{
		return;
	}
    
	dst += (ty*wb*4 + tx*4);
    
	unsigned char ri = color.r;
	unsigned char gi = color.g;
	unsigned char bi = color.b;
	unsigned char ai = color.a;

    for(int32_t j=0; j < th; j++)
    {
        for(int32_t i=0; i < tw; i++)
        {
	    	unsigned char *dd = (unsigned char *) dst;

            int xx = tx - r + i;
            int yy = ty - r + j;

            double dx = abs(xx - tx);
            double dy = abs(yy - ty);

            double length = sqrt(dx*dx + dy*dy);

            if(length < (double) r-0.2)
            {
                {
#ifdef MACOSX
		            dd[0] = ai;
		            dd[1] = ri;
		            dd[2] = gi;
		            dd[3] = bi;
#endif

#ifdef WIN32
		            dd[3] = ai;
		            dd[2] = ri;
		            dd[1] = gi;
		            dd[0] = bi;
#endif
                }
            }

            dst += 4;
        }

        dst += 4*(wb - tw);
    }
}

//
// fillRect()
// ----------
//
// Fills a rectangle with 
// a single color...
//

void PKBitmapOps::fillRect(void     *buffer,
                           unsigned int w,
                           unsigned int h,
                           PKColor   color,
                           unsigned int x,
                           unsigned int y,
                           unsigned int wi,
                           unsigned int hi)
{
    if(buffer == NULL)
    {
        return;
    }
    
	if(wi > w || hi > h)
	{
		return;
	}
    
	if(wi == 0)
	{
		wi = w;
	}
	
	if(hi == 0)
	{
		hi = h;
	}
    
    if(x + wi > w)
    {
        wi = w - x;
    }
    
    if(y + hi > h)
    {
        hi = h - y;
    }
	
    char *dst = (char *) buffer;
    
	if(dst == NULL)
	{
		return;
	}
    
	dst += (y*w*4 + x*4);
    
	unsigned char ri = color.r;
	unsigned char gi = color.g;
	unsigned char bi = color.b;
	unsigned char ai = color.a;
	
	unsigned int *line = new unsigned int[wi];
    
	if(line == NULL)
	{
		return;
	}
    
	for(unsigned int i=0; i<wi; i++)
	{
		unsigned char *dd = (unsigned char *) &(line[i]);

#ifdef MACOSX
		dd[0] = ai;
		dd[1] = ri;
		dd[2] = gi;
		dd[3] = bi;
#endif

#ifdef WIN32
		dd[3] = ai;
		dd[2] = ri;
		dd[1] = gi;
		dd[0] = bi;
#endif
	}
    
	int l  = 4*w;
	int li = 4*wi;
    
	for(unsigned int j=0; j<hi; j++)
	{
		memcpy(dst, line, li);
		dst += l;
	}
    
	delete [] line;
}

void PKBitmapOps::fillRect(PKImage *bitmap,
                           PKColor   color,
                           unsigned int  x,
                           unsigned int  y,
                           unsigned int  wi,
                           unsigned int  hi)
{
    if(bitmap == NULL)
    {
        return;
    }

    PKBitmapOps::fillRect(bitmap->getPixels(),
                          bitmap->getWidth(),
                          bitmap->getHeight(),
                          color, x, y, wi, hi);
}

//
// gradientFill()
// -------------
//
// Fills a rectangle with a
// vertical or horizontal, 
// two colors gradient...
//

void PKBitmapOps::gradientFill(void        *buffer,
                               unsigned int w,
                               unsigned int h,
                               int       orientation,
                               PKColor   color1,
                               PKColor   color2,
                               unsigned int  x,
                               unsigned int  y,
                               unsigned int  wi,
                               unsigned int  hi)
{
    if(buffer == NULL)
    {
        return;
    }

    unsigned int *dst = (unsigned int *) buffer;
    
	if(wi > w || hi > h)
	{
		return;
	}
    
	if(wi == 0)
	{
		wi = w;
	}
	
	if(hi == 0)
	{
		hi = h;
	}
    
    if(x + wi > w)
    {
        wi = w - x;
    }
    
    if(y + hi > h)
    {
        hi = h - y;
    }
	
    dst += (y*w + x);

    if(orientation == PK_ORIENTATION_VERTICAL)
    {
		unsigned int color;

        for(unsigned int j=0; j<hi; j++)
        {
			PKBitmapOps::gradientColorFunc(&color, j, hi, 
										   color1.r, color1.g, color1.b,
										   color2.r, color2.g, color2.b);

			for(unsigned int i=0; i < wi; i++)
			{
				dst[i] = color;
			}

            dst += w;
        }
        
    }
    else if(orientation == PK_ORIENTATION_HORIZONTAL)
    {
        unsigned int *line = new unsigned int[wi];
        
        if(line == NULL)
        {
            return;
        }

		PKBitmapOps::gradientLineFunc(line, wi, 
									  color1.r, color1.g, color1.b,
									  color2.r, color2.g, color2.b);
        
        int li = 4*wi;
        
        for(unsigned int j=0; j<hi; j++)
        {
            memcpy(dst, line, li);
            dst += w;
        }
    }
}


void PKBitmapOps::gradientFill(PKImage *bitmap,
                               int       orientation,
                               PKColor   color1,
                               PKColor   color2,
                               unsigned int  x,
                               unsigned int  y,
                               unsigned int  wi,
                               unsigned int  hi)
{
    if(bitmap == NULL)
    {
        return;
    }

    unsigned int w = bitmap->getWidth();
	unsigned int h = bitmap->getHeight();

    PKBitmapOps::gradientFill((void *) bitmap->getPixels(),
                              w, h, 
                              orientation,
                              color1,
                              color2,
                              x, y, wi, hi);
}

//
// flip()
// ------
//
// Flips the bitmap horizontally
// or vertically...
//

void PKBitmapOps::flip(PKImage    *bitmap,
					   int       orientation)
{
    if(bitmap == NULL)
    {
        return;
    }

	unsigned int w = bitmap->getWidth();
	unsigned int h = bitmap->getHeight();

    unsigned int *dst = (unsigned int *) bitmap->getPixels();
    
	if(dst == NULL)
	{
		return;
	}

	switch(orientation)
	{
	case PK_ORIENTATION_VERTICAL:
		{
			unsigned int *temp = new unsigned int[w];
			int tot			   = (h -1)*w;
			int ww             = 4*w;
			
			for(unsigned int j=0; j < h/2; j++)
			{
				int off = j*w;

				memcpy(temp, dst + off, ww);
				memcpy(dst + off, dst + tot - off, ww);
				memcpy(dst + tot - off, temp, ww);
			}
		}
		break;

	case PK_ORIENTATION_HORIZONTAL:
	default:
		{
			unsigned int temp;

			for(unsigned int j=0; j < h; j++)
			{
				int offset = j*w;

				for(unsigned int i=0; i < w/2; i++)
				{
					temp = dst[offset + i];
					dst[offset + i] = dst[offset + w - i - 1];
					dst[offset + w - i - 1] = temp;
				}
			}
		}
		break;
	}
}

//
// desaturate()
// ------------
//
// Desaturate the bitmap with
// an optional strength parameter
//

void PKBitmapOps::desaturate(PKImage *bitmap,
                             unsigned char strength) // 0 - 255
{
    if(bitmap == NULL)
    {
        return;
    }

	if(strength == 0)
	{
		return;
	}
    
	unsigned int w = bitmap->getWidth();
	unsigned int h = bitmap->getHeight();

    unsigned int *dst = (unsigned int *) bitmap->getPixels();
    
	if(dst == NULL)
	{
		return;
	}

	PKBitmapOps::desaturateFunc(dst, w, h, strength);
}

//
// blur()
// ------------
//
// Alters the whole bitmap
// to apply a 1-pixel blur effect...
//

void PKBitmapOps::blur(PKImage *bitmap) 
{
	if(bitmap == NULL)
	{
		return;
	}
    
    unsigned int bw = bitmap->getWidth();
    unsigned int bh = bitmap->getHeight();

    unsigned char *dst = (unsigned char *) bitmap->getPixels();
    
	if(dst == NULL)
	{
		return;
	}

	PKBitmapOps::blurFunc(dst, bw, bh);
}


//
// blurRadius()
// ------------
//
// Alters the whole bitmap
// to apply a blur effect...
//

void PKBitmapOps::blurRadius(PKImage *bitmap,
							 unsigned int radius) // in pixels
{
	if(bitmap == NULL)
	{
		return;
	}
    
    unsigned int bw = bitmap->getWidth();
    unsigned int bh = bitmap->getHeight();

    unsigned int *dst = (unsigned int *) bitmap->getPixels();
    
	if(dst == NULL)
	{
		return;
	}

	unsigned int i;
	unsigned int j;

	// blur horizontally

	for(j=0; j < bh; j++)
	{
		for(i=0; i < bw; i++)
		{
			unsigned int pixel = dst[i];
			unsigned int total = 1;

			unsigned int a = (pixel >> 24) & 0xFF;
			double		 r = (pixel >> 16) & 0xFF;
			double		 g = (pixel >>  8) & 0xFF;
			double		 b = (pixel      ) & 0xFF;

			for(unsigned int k=1; k <= radius; k++)
			{
				if(i >= k)
				{
					unsigned int p1 = dst[i - k];

					double		 r1 = (p1 >> 16) & 0xFF;
					double		 g1 = (p1 >>  8) & 0xFF;
					double		 b1 = (p1      ) & 0xFF;

					r += r1;
					g += g1;
					b += b1;

					total++;
				}

				if((i+k) < bw)
				{
					unsigned int p2 = dst[i + k];

					double		 r2 = (p2 >> 16) & 0xFF;
					double		 g2 = (p2 >>  8) & 0xFF;
					double		 b2 = (p2      ) & 0xFF;

					r += r2;
					g += g2;
					b += b2;

					total++;
				}
			}

			r /= total;
			g /= total;
			b /= total;

			dst[i] = ((a << 24) | ((unsigned int) r << 16) | 
					 ((unsigned int) g << 8) | (unsigned int) b);

		}

		dst += bw;
	}

	// blur vertically

    dst = (unsigned int *) bitmap->getPixels();

	for(j=0; j < bh; j++)
	{
		for(i=0; i < bw; i++)
		{
			unsigned int pixel = dst[i];
			unsigned int total = 1;

			unsigned int a = (pixel >> 24) & 0xFF;
			double		 r = (pixel >> 16) & 0xFF;
			double		 g = (pixel >>  8) & 0xFF;
			double		 b = (pixel      ) & 0xFF;

			for(unsigned int k=1; k <= radius; k++)
			{
				if(j >= k)
				{
					unsigned int p1 = dst[i - bw*k];

					double		 r1 = (p1 >> 16) & 0xFF;
					double		 g1 = (p1 >>  8) & 0xFF;
					double		 b1 = (p1      ) & 0xFF;

					r += r1;
					g += g1;
					b += b1;

					total++;
				}

				if((j+k) < bh)
				{
					unsigned int p2 = dst[i + bw*k];

					double		 r2 = (p2 >> 16) & 0xFF;
					double		 g2 = (p2 >>  8) & 0xFF;
					double		 b2 = (p2      ) & 0xFF;

					r += r2;
					g += g2;
					b += b2;

					total++;
				}
			}

			r /= total;
			g /= total;
			b /= total;

			dst[i] = ((a << 24) | ((unsigned int) r << 16) | 
					 ((unsigned int) g << 8) | (unsigned int) b);

		}

		dst += bw;
	}
}

//
// blit()
// ------
//
// Does a simple 32 bit blit
//

void PKBitmapOps::blit(void *bitmap,
                       unsigned int wBitmap,
                       unsigned int hBitmap,
                       int x,
                       int y,
                       PKImage *source, 
                       int sx,
                       int sy,
                       int sw,
                       int sh)
{
	if(source == NULL || 
	   bitmap == NULL)
	{
		return;
	}
    
    int bw = wBitmap;
    int bh = hBitmap;
    
	if(x >= bw || y >= bh)
	{
		return;
	}
    
	int w = source->getWidth();
	int h = source->getHeight();
    
	if(x < 0) { w = w + x; sx -= x; sw += x; x = 0; }
	if(y < 0) { h = h + y; sy -= y; sh += y; y = 0; }
    
	if(x + w > bw)
	{
		w = (bw - x);
	}
    
	if(y + h > bh)
	{
		h = (bh - y);
	}
    
	sw = pk_min(w, sw);
	sh = pk_min(h, sh);
    
	if(sx + sw > (int) source->getWidth())
	{
		sw = (w - sx);
	}
    
	if(sy + sh > (int) source->getHeight())
	{
		sh = (h - sy);
	}
    
	int aline = sw << 2;
	int sline = source->getWidth() << 2;
	int dline = wBitmap << 2;
    
	char *src = source->getPixels() + sy*sline + (sx << 2);
	char *dst = (char *) bitmap + y*dline + (x << 2);
    
	if(src == NULL ||
	   dst == NULL)
	{
		return;
	}
    
	for(int j=0; j < sh; j++)
	{
		memcpy(dst, src, aline);
        
		dst += dline;
		src += sline;
	}
}

void PKBitmapOps::blit(PKImage *bitmap,
                       int x,
                       int y,
                       PKImage *source, 
                       int sx,
                       int sy,
                       int sw,
                       int sh)
{
	if(source == NULL || 
	   bitmap == NULL)
	{
		return;
	}

    PKBitmapOps::blit(bitmap->getPixels(),
                      bitmap->getWidth(),
                      bitmap->getHeight(),
                      x, y, 
                      source, sx, sy, sw, sh);
}

//
// alphaBlit(alpha)
// ----------------
//
// Does a full 32 bit blit
// with a global alpha modifier
//

void PKBitmapOps::alphaBlit(void *buffer,
                            unsigned char al,
                            int x,
                            int y,
                            int bw,
                            int bh,
                            PKImage *source, 
                            int sx,
                            int sy,
                            int sw,
                            int sh)
{
	if(source == NULL || 
	   buffer == NULL ||
	   al     == 0)
	{
		return;
	}

	if(x >= bw || y >= bh)
	{
		return;
	}
    
	int w  = source->getWidth();
	int ow = source->getWidth();
	int h  = source->getHeight();
    
	if(x < 0) { w = w + x; sx -= x; sw += x; x = 0; }
	if(y < 0) { h = h + y; sy -= y; sh += y; y = 0; }
    
	if(x + w > bw)
	{
		w = (bw - x);
	}
    
	if(y + h > bh)
	{
		h = (bh - y);
	}
    
	sw = pk_min(w, sw);
	sh = pk_min(h, sh);
    
	if(sx + sw > (int) source->getWidth())
	{
		sw = (w - sx);
	}
    
	if(sy + sh > (int) source->getHeight())
	{
		sh = (h - sy);
	}
    
	if(sw == 0 || sh == 0)
	{
		return;
	}

	int dline = bw;
    
	unsigned int galpha = al;
    
	unsigned int *src = (unsigned int *) source->getPixels() + sy*ow + sx;
	unsigned int *dst = (unsigned int *) buffer + y*dline + x;
    
	PKBitmapOps::alphaBlitFunc((unsigned char *) src, 
							   (unsigned char *) dst, 
							   sw, sh, ow, dline, galpha);
}

void PKBitmapOps::alphaBlit(PKImage *bitmap,
                            unsigned char al,
                            int x,
                            int y,
                            PKImage *source, 
                            int sx,
                            int sy,
                            int sw,
                            int sh)
{
	if(source == NULL || 
	   bitmap == NULL ||
	   al     == 0)
	{
		return;
	}

    int bw = bitmap->getWidth();
    int bh = bitmap->getHeight();

    PKBitmapOps::alphaBlit(bitmap->getPixels(), al,
                           x, y, bw, bh,
                           source,
                           sx, sy, sw, sh);
}

//
// alphaDataBlit(alpha)
// --------------------
//
// Does a full 32 bit blit
// with a global alpha modifier
//

void PKBitmapOps::alphaDataBlit(PKImage *bitmap,
								  unsigned char alpha,
								  int x,
								  int y,
								  void *pixels, 
								  int sx,
								  int sy,
								  int sw,
								  int sh)
{
	if(pixels == NULL || 
	   bitmap == NULL ||
	   alpha  == 0)
	{
		return;
	}

    int bw = bitmap->getWidth();
    int bh = bitmap->getHeight();

	if(x >= bw || y >= bh)
	{
		return;
	}
    
	int w  = sw;
	int ow = sw;
	int h  = sh;
    
	int dline = bitmap->getWidth();
    
	unsigned int galpha = alpha;
    
	unsigned int *src = (unsigned int *) pixels + sy*ow + sx;
	unsigned int *dst = (unsigned int *) bitmap->getPixels() + y*dline + x;
    
	PKBitmapOps::alphaBlitFunc((unsigned char *) src, 
							   (unsigned char *) dst, 
							   sw, sh, ow, dline, galpha);
}

void PKBitmapOps::fillStretchWidth(void *dest,
                                   unsigned int w,
                                   unsigned int h,
							       PKImage *source,
							       std::vector <unsigned int> offsets)
{
	if(dest   == NULL || 
	   source == NULL)
	{
		return;
	}

	int sizeDiff = (w - source->getWidth());

	unsigned int srcOffset = 0;
	unsigned int dstOffset = 0;

	int count = offsets.size();

	if(count < 1)
	{
		return;
	}

	for(int i=0; i < count; i++)
	{
		int areaSize = 0;

		if(i == (count - 1))
		{
			areaSize = (sizeDiff / count) + (sizeDiff % count);
		}
		else
		{
			areaSize = (sizeDiff / count);
		}

		PKBitmapOps::blit(dest, w, h, dstOffset, 0, source, srcOffset, 0, offsets[i] - srcOffset);

		dstOffset += (offsets[i] - srcOffset);

		for(int j=0; j < areaSize; j++)
		{
			PKBitmapOps::blit(dest, w, h, dstOffset+j, 0, source, offsets[i], 0, 1);
		}

		srcOffset  = offsets[i];
		dstOffset += areaSize;
	}

	PKBitmapOps::blit(dest, w, h, dstOffset, 0, source, srcOffset, 0, 
					  source->getWidth() - offsets[offsets.size() - 1]);
}

void PKBitmapOps::fillWidthWithStart(void *dest,
									 unsigned int wToFill,
									 unsigned int w,
									 unsigned int h,
									 PKImage *source,
								     unsigned int offset)
{
	if(dest   == NULL || 
	   source == NULL)
	{
		return;
	}

	int sizeDiff = (wToFill - offset);

	if(sizeDiff < 0)
	{
		PKBitmapOps::blit(dest, w, h, 0, 0, source, 0, 0, wToFill);
		return;
	}

	unsigned int srcOffset = 0;
	unsigned int dstOffset = 0;

	int areaSize = sizeDiff;

	PKBitmapOps::blit(dest, w, h, dstOffset, 0, source, srcOffset, 0, offset);

	dstOffset += offset;

	for(int j=0; j < areaSize; j++)
	{
		PKBitmapOps::blit(dest, w, h, dstOffset+j, 0, source, offset, 0, 1);
	}
}

void PKBitmapOps::fillWidthWithEnd(void *dest,
                                   unsigned int start,
								   unsigned int wToFill,
							       unsigned int w,
								   unsigned int h,
								   PKImage *source,
								   unsigned int offset)
{
	if(dest   == NULL || 
	   source == NULL)
	{
		return;
	}

	int sizeDiff = (wToFill - (source->getWidth() - offset));

	if(sizeDiff < 0)
	{
		PKBitmapOps::blit(dest, w, h, start, 0, source, offset, 0, wToFill);
		return;
	}

	unsigned int srcOffset = offset;
	unsigned int dstOffset = start;

	int areaSize = sizeDiff;

	for(int j=0; j < areaSize; j++)
	{
		PKBitmapOps::blit(dest, w, h, dstOffset+j, 0, source, offset, 0, 1);
	}

	dstOffset += sizeDiff;

	PKBitmapOps::blit(dest, w, h, dstOffset, 0, source, srcOffset, 0, 
					  source->getWidth() - offset);
}


void PKBitmapOps::fillStretchWidth(PKImage *dest,
								   PKImage *source,
								   std::vector <unsigned int> offsets)
{
	if(dest   == NULL || 
	   source == NULL)
	{
		return;
	}

    PKBitmapOps::fillStretchWidth(dest->getPixels(),
                                  dest->getWidth(),
                                  dest->getHeight(),
                                  source, offsets);
}	

//
// stretch()
// ---------
//
// Stretches the source bitmap
// to fill the destination...
//

void PKBitmapOps::stretch(PKImage *dest,
                          PKImage *source)
{
	if(source == NULL || 
	   dest   == NULL)
	{
		return;
	}

	unsigned int *dst = (unsigned int *) dest->getPixels();
	unsigned int *src = (unsigned int *) source->getPixels();

	if(src == NULL ||
	   dst == NULL)
	{
		return;
	}

	double nXFactor = (double) source->getWidth() / (double) dest->getWidth();
	double nYFactor = (double) source->getHeight() / (double) dest->getHeight();

	unsigned int ws = source->getWidth();
	unsigned int hs = source->getHeight();

	unsigned int wd = dest->getWidth();
	unsigned int hd = dest->getHeight();

	double fraction_x, fraction_y, one_minus_x, one_minus_y;
    int ceil_x, ceil_y, floor_x, floor_y;
    unsigned char red, green, blue, alpha;
    unsigned char B1, B2;

    for(unsigned int j=0; j < hd; j++)
	{
		for(unsigned int i=0; i < wd; i++)
		{
            floor_x = (int)floor(i * nXFactor);
            floor_y = (int)floor(j * nYFactor);
            ceil_x = floor_x + 1;
            if (ceil_x >= (int) ws) ceil_x = floor_x;
            ceil_y = floor_y + 1;
            if (ceil_y >= (int) hs) ceil_y = floor_y;
            fraction_x = i * nXFactor - floor_x;
            fraction_y = j * nYFactor - floor_y;
            one_minus_x = 1.0 - fraction_x;
            one_minus_y = 1.0 - fraction_y;

			unsigned int p1 = src[floor_y*ws + floor_x];

			unsigned int a1 = (p1 >> 24) & 0xFF;
			double		 r1 = (p1 >> 16) & 0xFF;
			double		 g1 = (p1 >>  8) & 0xFF;
			double		 b1 = (p1      ) & 0xFF;

			unsigned int p2 = src[floor_y*ws + ceil_x];

			unsigned int a2 = (p2 >> 24) & 0xFF;
			double		 r2 = (p2 >> 16) & 0xFF;
			double		 g2 = (p2 >>  8) & 0xFF;
			double		 b2 = (p2      ) & 0xFF;

            unsigned int p3 = src[ceil_y*ws + floor_x];

			unsigned int a3 = (p3 >> 24) & 0xFF;
			double		 r3 = (p3 >> 16) & 0xFF;
			double		 g3 = (p3 >>  8) & 0xFF;
			double		 b3 = (p3      ) & 0xFF;

			unsigned int p4 = src[ceil_y*ws + ceil_x];

			unsigned int a4 = (p4 >> 24) & 0xFF;
			double		 r4 = (p4 >> 16) & 0xFF;
			double		 g4 = (p4 >>  8) & 0xFF;
			double		 b4 = (p4      ) & 0xFF;

            // Alpha
            B1 = (unsigned char)(one_minus_x * a1 + fraction_x * a2);

            B2 = (unsigned char)(one_minus_x * a3 + fraction_x * a4);
            
            alpha = (unsigned char)(one_minus_y * (double)(B1) + fraction_y * (double)(B2));

            // Blue
            B1 = (unsigned char)(one_minus_x * b1 + fraction_x * b2);

            B2 = (unsigned char)(one_minus_x * b3 + fraction_x * b4);
            
            blue = (unsigned char)(one_minus_y * (double)(B1) + fraction_y * (double)(B2));

            // Green
            B1 = (unsigned char)(one_minus_x * g1 + fraction_x * g2);

            B2 = (unsigned char)(one_minus_x * g3 + fraction_x * g4);
            
            green = (unsigned char)(one_minus_y * (double)(B1) + fraction_y * (double)(B2));

            // Red
            B1 = (unsigned char)(one_minus_x * r1 + fraction_x * r2);

            B2 = (unsigned char)(one_minus_x * r3 + fraction_x * r4);
            
            red = (unsigned char)(one_minus_y * (double)(B1) + fraction_y * (double)(B2));

            dst[j*wd+i] = ((alpha << 24) | (red << 16) | (green << 8) | blue);
		}
	}
}

/*
 * Default C Funtions
 * ------------------
 * 
 */

void PKBitmapOps::alphaBlitCFunction(unsigned char *sp, 
									 unsigned char *dp, 
									 unsigned int  w, 
									 unsigned int  h, 
									 unsigned int  s_line, 
									 unsigned int  d_line,
									 unsigned char galpha)
{
	unsigned int *src = (unsigned int *) sp;
	unsigned int *dst = (unsigned int *) dp;

    int diff;

	for(unsigned int j=0; j < h; j++)
	{
		for(unsigned int i=0; i < w; i++)
		{
			unsigned char *s = (unsigned char *) &(src[i]);
			unsigned char *d = (unsigned char *) &(dst[i]);
            
#ifdef MACOSX
			unsigned int alpha = s[0];

			unsigned int rs = s[1];
			unsigned int gs = s[2];
			unsigned int bs = s[3];
            
			unsigned int rd = d[1];
			unsigned int gd = d[2];
			unsigned int bd = d[3];
#endif            

#ifdef WIN32
			unsigned int alpha = s[3];

			unsigned int rs = s[2];
			unsigned int gs = s[1];
			unsigned int bs = s[0];
            
			unsigned int rd = d[2];
			unsigned int gd = d[1];
			unsigned int bd = d[0];
#endif            

#ifdef LINUX
			unsigned int alpha = s[3];

			unsigned int rs = s[2];
			unsigned int gs = s[1];
			unsigned int bs = s[0];
            
			unsigned int rd = d[2];
			unsigned int gd = d[1];
			unsigned int bd = d[0];
#endif            

			alpha = (alpha * galpha) / 255; 
	                diff = 255 - alpha;
            
			rd = (diff*rd + alpha*rs) / 255;
			gd = (diff*gd + alpha*gs) / 255;
			bd = (diff*bd + alpha*bs) / 255;
            
#ifdef MACOSX
			d[0] = 255;
			d[1] = rd;
			d[2] = gd;
			d[3] = bd;			
#endif

#ifdef WIN32
			d[3] = 255;
			d[2] = rd;
			d[1] = gd;
			d[0] = bd;			
#endif

#ifdef LINUX
			d[3] = 255;
			d[2] = rd;
			d[1] = gd;
			d[0] = bd;			
#endif
		}
        
		src += s_line;
		dst += d_line;
	}    
}

//
//
//

void PKBitmapOps::blurCFunction(unsigned char *d, 
							    unsigned int  w, 
							    unsigned int  h)
{
	unsigned int *dst = (unsigned int *) d;

	unsigned int i;
	unsigned int j;

	// blur horizontally

	for(j=0; j < h; j++)
	{
		for(i=0; i < w; i++)
		{
			unsigned int pixel = dst[i];
			unsigned int total = 1;

			unsigned int a = (pixel >> 24) & 0xFF;
			double		 r = (pixel >> 16) & 0xFF;
			double		 g = (pixel >>  8) & 0xFF;
			double		 b = (pixel      ) & 0xFF;

			if(i >= 1)
			{
				unsigned int p1 = dst[i - 1];

				double		 r1 = (p1 >> 16) & 0xFF;
				double		 g1 = (p1 >>  8) & 0xFF;
				double		 b1 = (p1      ) & 0xFF;

				r += r1;
				g += g1;
				b += b1;

				total++;
			}

			if((i+1) < w)
			{
				unsigned int p2 = dst[i + 1];

				double		 r2 = (p2 >> 16) & 0xFF;
				double		 g2 = (p2 >>  8) & 0xFF;
				double		 b2 = (p2      ) & 0xFF;

				r += r2;
				g += g2;
				b += b2;

				total++;
			}

			r /= total;
			g /= total;
			b /= total;

			dst[i] = ((a << 24) | ((unsigned int) r << 16) | 
					 ((unsigned int) g << 8) | (unsigned int) b);

		}

		dst += w;
	}

	// blur vertically

    dst = (unsigned int *) d;

	for(j=0; j < h; j++)
	{
		for(i=0; i < w; i++)
		{
			unsigned int pixel = dst[i];
			unsigned int total = 1;

			unsigned int a = (pixel >> 24) & 0xFF;
			double		 r = (pixel >> 16) & 0xFF;
			double		 g = (pixel >>  8) & 0xFF;
			double		 b = (pixel      ) & 0xFF;

			if(j >= 1)
			{
				unsigned int p1 = dst[i - w];

				double		 r1 = (p1 >> 16) & 0xFF;
				double		 g1 = (p1 >>  8) & 0xFF;
				double		 b1 = (p1      ) & 0xFF;

				r += r1;
				g += g1;
				b += b1;

				total++;
			}

			if((j+1) < h)
			{
				unsigned int p2 = dst[i + w];

				double		 r2 = (p2 >> 16) & 0xFF;
				double		 g2 = (p2 >>  8) & 0xFF;
				double		 b2 = (p2      ) & 0xFF;

				r += r2;
				g += g2;
				b += b2;

				total++;
			}

			r /= total;
			g /= total;
			b /= total;

			dst[i] = ((a << 24) | ((unsigned int) r << 16) | 
					 ((unsigned int) g << 8) | (unsigned int) b);

		}

		dst += w;
	}
}

void PKBitmapOps::gradientLineCFunction(unsigned int *dst,
									    unsigned int  w,
									    unsigned char r1,
										unsigned char g1,
										unsigned char b1,
										unsigned char r2,
										unsigned char g2,
										unsigned char b2)
{
    for(unsigned int i=0; i<w; i++)
    {
        double percent = (double)i/(double)w;
        
        double r = ((double)r1*(1.0 - percent) + (double)r2*percent);
        double g = ((double)g1*(1.0 - percent) + (double)g2*percent);
        double b = ((double)b1*(1.0 - percent) + (double)b2*percent);
        
		unsigned char *d = (unsigned char *) &(dst[i]);

#ifdef MACOSX
		d[0] = 255;
		d[1] = (int) r;
		d[2] = (int) g;
		d[3] = (int) b;
#endif

#ifdef WIN32
		d[3] = 255;
		d[2] = (int) r;
		d[1] = (int) g;
		d[0] = (int) b;
#endif
    }
}

void PKBitmapOps::gradientColorCFunction(unsigned int *dst,
									     unsigned int  index,
								         unsigned int  width,
										 unsigned char r1,
								         unsigned char g1,
								         unsigned char b1,
								         unsigned char r2,
								         unsigned char g2,
								         unsigned char b2)
{
    double percent = (double)index/(double)width;
    
    double r = ((double)r2*(1.0 - percent) + (double)r1*percent);
    double g = ((double)g2*(1.0 - percent) + (double)g1*percent);
    double b = ((double)b2*(1.0 - percent) + (double)b1*percent);

	unsigned char *d = (unsigned char *) dst;
	
#ifdef MACOSX
	d[0] = 255;
	d[1] = (int) r;
	d[2] = (int) g;
	d[3] = (int) b;
#endif

#ifdef WIN32
	d[3] = 255;
	d[2] = (int) r;
	d[1] = (int) g;
	d[0] = (int) b;
#endif
}


void PKBitmapOps::desaturateCFunction(unsigned int *dst, 
									  unsigned int  w, 
									  unsigned int  h,
									  unsigned char amount)
{
	for(unsigned int j=0; j < h; j++)
	{
		for(unsigned int i=0; i < w; i++)
		{
			unsigned int pixel = dst[i];

			int a = (pixel >> 24);
			int r = (pixel >> 16) & 0xFF;
			int g = (pixel >>  8) & 0xFF;
			int b = (pixel      ) & 0xFF;

			// aproximation of grey

			int gray = (38*r + 77*g + 13*b) >> 7;

			if(amount < 255)
			{
				r = r + (((gray - r)*amount) >> 8);
				g = g + (((gray - g)*amount) >> 8);
				b = b + (((gray - b)*amount) >> 8);
			}
			else
			{
				r = gray;
				g = gray;
				b = gray;
			}

			unsigned char *d = (unsigned char *) &(dst[i]);
	
#ifdef MACOSX
			d[0] = a;
			d[1] = (int) r;
			d[2] = (int) g;
			d[3] = (int) b;
#endif

#ifdef WIN32
			d[3] = a;
			d[2] = (int) r;
			d[1] = (int) g;
			d[0] = (int) b;
#endif
		}

		dst += w;
	}
}

