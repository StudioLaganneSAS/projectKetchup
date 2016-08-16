//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCpu.h"

#ifdef MACOSX
#include <sys/sysctl.h>
#endif

PKCpuType PKCpu::getCpuType()
{
//#if(defined(x64))
    return PK_CPU_TYPE_X86;
/*
#else
#if(defined(__i386__) || defined(X86))
    if(isCpuX86())
    {
        return PK_CPU_TYPE_X86;
    }
#endif
#endif

#ifdef __ppc__
    return PK_CPU_TYPE_PPC;
#endif
	*/
    return PK_CPU_TYPE_UNKNOWN;
}

PKCpuFlags PKCpu::getCpuFlags()
{
    return PK_CPU_FLAGS_NONE; //TEMP

	/*
#if(defined(X86) || defined(__i386__))
    if(PKCpu::getCpuType() == PK_CPU_TYPE_X86)
    {
        //
        // Get all the accel flags
        //

        PKCpuFlags flags = PK_CPU_FLAGS_NONE;
        bool     isAMD;
        uint32_t reg[4];

        //
        // Check cpuid
        //

        doCpuId(0x00000000, reg);

        if(!reg[0])
        {
            return PK_CPU_FLAGS_NONE;
        }

        isAMD = ((reg[1] == 0x68747541) &&
                 (reg[2] == 0x444d4163) &&
                 (reg[3] == 0x69746e65));

        doCpuId(0x00000001, reg);

        if(!(reg[3] & 0x00800000))
        {
            return PK_CPU_FLAGS_NONE;
        }

        flags |= PK_CPU_FLAGS_MMX;

        if(reg[3] & 0x02000000)
        {
            flags |= PK_CPU_FLAGS_SSE;
        }

        if(reg[3] & (0x01 << 26))
        {
            flags |= PK_CPU_FLAGS_SSE2;
        }

        doCpuId(0x80000000, reg);

        if(reg[0] < 0x80000001)
        {
            return flags;
        }

        doCpuId(0x80000001, reg);

        if(reg[3] & 0x80000000)
        {
            flags |= PK_CPU_FLAGS_3DNOW;
        }

        if(isAMD && (reg[3] & 0x00400000))
        {
            flags |= PK_CPU_FLAGS_MMXEXT;
        }

        return flags;
    }
    else
#endif

#ifdef __ppc__
    if(PKCpu::getCpuType() == PK_CPU_TYPE_PPC)
    {
        int altivec = 0;
        int sel[2]  = { CTL_HW, HW_VECTORUNIT };
        size_t size = sizeof(altivec);

        int err = sysctl(sel, 2, &altivec, &size, NULL, 0);

        if( err == 0 && altivec != 0 )
        {
            return PK_CPU_FLAGS_ALTIVEC;
        }
        else
        {
            return PK_CPU_FLAGS_NONE;
        }
    }
    else
#endif
    {
        return PK_CPU_FLAGS_NONE;
    }

#endif
	*/
}
