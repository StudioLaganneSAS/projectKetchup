//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKTime.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <CoreServices/CoreServices.h>
#include <unistd.h>
#endif

#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>
#endif

int64_t PKTime::getTime()
{
#ifdef WIN32

    LARGE_INTEGER time;

    if(QueryPerformanceCounter(&time))
    {
        return time.QuadPart;
    }

#endif

#ifdef MACOSX

    Nanoseconds  nanosec;
    AbsoluteTime time = UpTime();

    nanosec = AbsoluteToNanoseconds(time);

    return UnsignedWideToUInt64(nanosec);

#endif

#ifdef LINUX


    const long long TICKS_PER_SECOND = 1000000L;

    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);    

    long long quadPart = tv.tv_sec * TICKS_PER_SECOND + tv.tv_usec;

    return quadPart;
    
#endif

    return 0;
}

int64_t PKTime::getTimeScale()
{
#ifdef WIN32

    LARGE_INTEGER freq;

    if(QueryPerformanceFrequency(&freq))
    {
        return freq.QuadPart;
    }

#endif

#ifdef MACOSX

    return 1000000000; // nanoseconds

#endif

#ifdef LINUX

    return 1000000; // microseconds

#endif

    return 1;
}

void PKTime::sleep(unsigned int milliseconds)
{
#ifdef WIN32
    Sleep(milliseconds);
#endif

#ifdef LINUX
    milliseconds *= 1000;
    usleep(milliseconds);
#endif

#ifdef MACOSX
    milliseconds *= 1000;
#ifndef __ppc__
    usleep(milliseconds);
#else
	// TODO: !!!!!
#endif
#endif
}

double PKTime::getTimeInSeconds()
{
    double t = (double) PKTime::getTime();
    double s = (double) PKTime::getTimeScale();

    if(s != 0)
    {
        return t / s;
    }

    return 0.0;
}
