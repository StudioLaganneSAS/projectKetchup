//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_TIME_H
#define PK_TIME_H

#include "PKTypes.h"

class PKTime
{
public:

    static int64_t getTime();
    static int64_t getTimeScale();

    static void sleep(unsigned int milliseconds);

    static double getTimeInSeconds();
};

#endif // PK_TIME_H
