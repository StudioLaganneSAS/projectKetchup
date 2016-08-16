//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_X86_H
#define PK_X86_H

#include "PKTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Checks whether the CPU we're running
// on is at least an i386...
//

int isCpuX86();

//
// Executes the cpuid instruction
// with provided parameters
//

void doCpuId(uint32_t  operation,
             uint32_t *registers);

#ifdef __cplusplus
}
#endif

#endif // PK_X86_H
