//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_CRITSEC_H
#define PK_CRITSEC_H

#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#endif

#include "PKTypes.h"

class PKCritSec
{
public:

    PKCritSec();
    virtual ~PKCritSec();

    virtual void wait();
    virtual bool tryWait();
    virtual void release();

protected:
    void* critSection;
};

//
// Auto-lock for PKCritSec
//

class PKCritLock
{
public:

    PKCritLock(PKCritSec *section);
    virtual ~PKCritLock();

private:
    PKCritSec *section;
};

#endif // PK_CRITSEC_H
