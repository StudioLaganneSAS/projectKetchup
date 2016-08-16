//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKBitmapOpsPPC.h"

#ifdef MACOSX
#include <Accelerate/Accelerate.h>
//#include <altivec.h>
#endif

void _alphaBlitPPCGlobalAlpha(unsigned char *src, 
							  unsigned char *dst, 
							  unsigned int  w, 
							  unsigned int  h, 
							  unsigned int  s_line, 
							  unsigned int  d_line,
							  unsigned int  galpha)
{
/*
    vector unsigned char *vsrc = (vector unsigned char *) src;
    vector unsigned char *vdst = (vector unsigned char *) dst;
    
    vector unsigned char v1;
    vector unsigned char v2;
    vector unsigned char v3;
    vector unsigned char v4;
    
    vector unsigned char zero;
    zero = vec_xor(zero, zero);

    vector unsigned char src1;
    vector unsigned char dst1;
    vector unsigned char src2;
    vector unsigned char dst2;
        
    vector unsigned short mul1;
    vector unsigned short mul2;
    vector unsigned short mul3;

    vector unsigned char extractAlphaMask1 = (vector unsigned char) 
        ( 16, 0, 16, 0, 16, 0, 16, 0, 16, 4, 16, 4, 16, 4, 16, 4 );

    vector unsigned char extractAlphaMask2 = (vector unsigned char) 
        ( 16, 8, 16, 8, 16, 8, 16, 8, 16, 12, 16, 12, 16, 12, 16, 12 );

    vector unsigned char two55AlphaMask = (vector unsigned short) 
        ( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF );

    vector unsigned char fistTwoPixelsMask = (vector unsigned char) 
        ( 16, 0, 16, 1, 16, 2, 16, 3, 16, 4, 16, 5, 16, 6, 16, 7 );

    vector unsigned char scndTwoPixelsMask = (vector unsigned char) 
        ( 16, 8, 16, 9, 16, 10, 16, 11, 16, 12, 16, 13, 16, 14, 16, 15 );
 
    vector unsigned char mergeFourPixelsMask = (vector unsigned char) 
        ( 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31 );
    
    vector unsigned short shiftMask = (vector unsigned char) 
        ( 8, 8, 8, 8, 8, 8, 8, 8 );

    // Prepare alpha vector
    
    vector unsigned char alpha;
    vector unsigned char two55minusAlpha;

    int parts = w*4;
    
    int d = 4*d_line;
    int s = 4*s_line;
    
    for(unsigned int j=0; j<h; j++)
    {
        vector unsigned char *vlsrc = (vector unsigned char *) (src + j*s);
        vector unsigned char *vldst = (vector unsigned char *) (dst + j*d);
        
        for(unsigned int i=0; i<w/16; i++)
        {
            // Prefecth data
            
            vec_dst(vlsrc, 0x10010100, 0);
            vec_dst(vldst, 0x10010100, 0);

            // Extract 4 pixels ARGB 
            // from source and dest
            
            v1 = vlsrc[0];
            v2 = vlsrc[1];
            v3 = vlsrc[2];
            v4 = vlsrc[3];
                        
//            vsrc = vec_ld(i, src);
//            vdst = vec_ld(i, dst);
            
            /*
            // Do the first two pixels
            
            // Extract alpha values from
            // the first two pixels...
            
            alpha =  vec_perm(vsrc, zero, extractAlphaMask1);
            
            // Now alpha contains 0A0A0A0A0A0A0A0A
            // so let's calculate 255 - alpha
            
            two55minusAlpha = vec_sub(two55AlphaMask, alpha);
            
            // vsrc is : ARGBARGBARGBARGB
            // we want to extract bytes 0 to 7
            // and interleave with byte 0 from
            // the zero'ed vector:
            
            src1 = vec_perm(vsrc, zero, fistTwoPixelsMask);
            dst1 = vec_perm(vdst, zero, fistTwoPixelsMask);
            
            // Now source and dest both 
            // contain the first two pixels
            // in the form 0A0R0G0B0A0R0G0B
            // so let's multiply by alpha
            // and 225-alpha, then add them and 
            // divide by 255
            
            mul1 = vec_mulo(src1, alpha);
            mul2 = vec_mulo(dst1, two55minusAlpha);
            
            mul2 = vec_adds(mul1, mul2);
            mul2 = vec_sr(mul2, shiftMask);
            
            // Do the second two pixels
            
            // Extract alpha values from
            // the second two pixels...
            
            alpha =  vec_perm(vsrc, zero, extractAlphaMask2);
            
            // Now alpha contains 0A0A0A0A0A0A0A0A
            // so let's calculate 255 - alpha
            
            two55minusAlpha = vec_sub(two55AlphaMask, alpha);

            // vsrc is : ARGBARGBARGBARGB
            // we want to extract bytes 8 to 15
            // and interleave with byte 0 from
            // the zero'ed vector:

            src2 = vec_perm(vsrc, zero, scndTwoPixelsMask);
            dst2 = vec_perm(vdst, zero, scndTwoPixelsMask);

            // Now source and dest both 
            // contain the second two pixels
            // in the form 0A0R0G0B0A0R0G0B
            // so let's multiply by alpha
            // and 225-alpha, then add them and 
            // divide by 255
            
            mul1 = vec_mulo(src2, alpha);
            mul3 = vec_mulo(dst2, two55minusAlpha);
            
            mul3 = vec_adds(mul1, mul3);
            mul3 = vec_sr(mul3, shiftMask);
            
            // Merge back all four pixels
            
            vdst = vec_perm(mul2, mul3, mergeFourPixelsMask);
            
            // And store result
            
//            vec_st(vdst, i, dst);
            
            vldst[0] = v1;
            vldst[1] = v2;
            vldst[2] = v3;
            vldst[3] = v4;
            
            vlsrc+=4;
            vldst+=4;
        }                
    }
*/
}

void _alphaBlitPPC(unsigned char *sp, 
                   unsigned char *dp, 
                   unsigned int  w, 
                   unsigned int  h, 
                   unsigned int  s_line, 
                   unsigned int  d_line,
                   unsigned char galpha)
{
/*    if(galpha == 255)
    {
        vImage_Buffer src;
        vImage_Buffer dst;
        
        src.data     = sp;
        src.width    = w;
        src.height   = h;
        src.rowBytes = s_line*4;
        
        dst.data     = dp;
        dst.width    = w;
        dst.height   = h;
        dst.rowBytes = d_line*4;
        
        vImageAlphaBlend_ARGB8888(&src, &dst, &dst, 0);                
    }
    else
    {
        _alphaBlitPPCGlobalAlpha(sp, dp, w, h, s_line, d_line, galpha);
    }*/
}
