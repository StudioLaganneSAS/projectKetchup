//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAP_OPS_X86_H
#define PK_BITMAP_OPS_X86_H

#ifdef __cplusplus
extern "C" {
#endif

//
// delcaration of MMX routines
//
//

void _alphaBlitMMXNOGlobalAlpha(unsigned char *src, 
								unsigned char *dst, 
								unsigned int  w, 
								unsigned int  h, 
								unsigned int  s_line, 
								unsigned int  d_line);

void _alphaBlitMMXGlobalAlpha(unsigned char *src, 
							  unsigned char *dst, 
							  unsigned int  w, 
							  unsigned int  h, 
							  unsigned int  s_line, 
							  unsigned int  d_line,
							  unsigned int  alpha);

void _alphaBlitSSENOGlobalAlpha(unsigned char *src, 
								unsigned char *dst, 
								unsigned int  w, 
								unsigned int  h, 
								unsigned int  s_line, 
								unsigned int  d_line);

void _alphaBlitSSEGlobalAlpha(unsigned char *src, 
							  unsigned char *dst, 
							  unsigned int  w, 
							  unsigned int  h, 
							  unsigned int  s_line, 
							  unsigned int  d_line,
							  unsigned int  alpha);

void _blurMMX(unsigned char *dst, 
			  unsigned int  w, 
			  unsigned int  h);

void _gradientLineMMX(unsigned int *dst,
					  unsigned int  w,
					  unsigned char r1,
					  unsigned char g1,
					  unsigned char b1,
					  unsigned char r2,
					  unsigned char g2,
					  unsigned char b2);

void _gradientColorMMX(unsigned int *dst,
					   unsigned int  index,
					   unsigned int  width,
					   unsigned char r1,
					   unsigned char g1,
					   unsigned char b1,
					   unsigned char r2,
					   unsigned char g2,
					   unsigned char b2);

void _desaturateFullSSE(unsigned int *dst, 
						unsigned int  w, 
						unsigned int  h);

void _desaturateAmountSSE(unsigned int *dst, 
						  unsigned int  w, 
						  unsigned int  h,
						  unsigned char amount);

//
// Alpha blit wrapper for efficiency
//

void _alphaBlitMMX(unsigned char *src, 
				   unsigned char *dst, 
				   unsigned int  w, 
				   unsigned int  h, 
				   unsigned int  s_line, 
				   unsigned int  d_line,
				   unsigned char alpha)
{
	unsigned int asx = (s_line << 2) - (w << 2);
	unsigned int adx = (d_line << 2) - (w << 2);

	if(alpha == 255)
	{
		_alphaBlitMMXNOGlobalAlpha(src, dst, w, h, asx, adx);
	}
	else
	{
		unsigned int alp = (alpha + 1);
		_alphaBlitMMXGlobalAlpha(src, dst, w, h, asx, adx, alp);
	}
}

void _alphaBlitSSE(unsigned char *src, 
				   unsigned char *dst, 
				   unsigned int  w, 
				   unsigned int  h, 
				   unsigned int  s_line, 
				   unsigned int  d_line,
				   unsigned char alpha)
{
	unsigned int asx = (s_line << 2) - (w << 2);
	unsigned int adx = (d_line << 2) - (w << 2);

	if(alpha == 255)
	{
		_alphaBlitSSENOGlobalAlpha(src, dst, w, h, asx, adx);
	}
	else
	{
		unsigned int alp = (alpha + 1);
		_alphaBlitSSEGlobalAlpha(src, dst, w, h, asx, adx, alp);
	}
}

//
// Desaturation wrapper for efficiency
//

void _desaturateSSE(unsigned int *dst, 
					unsigned int  w, 
					unsigned int  h,
					unsigned char amount)
{
	if(amount == 255)
	{
		_desaturateFullSSE(dst, w, h);
	}
	else
	{
		_desaturateAmountSSE(dst, w, h, amount);
	}
}


#ifdef __cplusplus
}
#endif


#endif PK_BITMAP_OPS_X86_H