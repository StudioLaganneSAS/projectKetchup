//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CPU_H
#define PK_CPU_H

// Includes

#include "PKTypes.h"
#include "PKX86.h"

// PKCpu types

typedef enum {

    PK_CPU_TYPE_X86,
    PK_CPU_TYPE_PPC,
    PK_CPU_TYPE_UNKNOWN,

} PKCpuType;

typedef int PKCpuFlags;

#define PK_CPU_FLAGS_NONE       0
#define PK_CPU_FLAGS_MMX        1
#define PK_CPU_FLAGS_MMXEXT    (1 << 1)
#define PK_CPU_FLAGS_3DNOW     (1 << 2)
#define PK_CPU_FLAGS_SSE       (1 << 3)
#define PK_CPU_FLAGS_SSE2      (1 << 4)
#define PK_CPU_FLAGS_ALTIVEC   (1 << 5)

//
// PKCpu
//

class PKCpu
{
public:

    //
    // Gets the type of CPU we are
    // running on at the moment...
    //

    static PKCpuType getCpuType();

    //
    // Gets the CPU flags, which can
    // tell you what accel instructions
    // you may use at runtime...
    //

    static PKCpuFlags getCpuFlags();
};

#endif // PK_CPU_H
