//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_BITMAP_OPS_PPC_H
#define PK_BITMAP_OPS_PPC_H

//
// delcaration of AltiVec routines
//
//

void _alphaBlitPPC(unsigned char *sp, 
                   unsigned char *dp, 
                   unsigned int  w, 
                   unsigned int  h, 
                   unsigned int  s_line, 
                   unsigned int  d_line,
                   unsigned char galpha);

#endif PK_BITMAP_OPS_PPC_H
