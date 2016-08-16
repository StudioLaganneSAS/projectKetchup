//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_MUTEX_H
#define PK_MUTEX_H

#include "PKTypes.h"

// Mutex class

class PKMutex
{
public:

    PKMutex();
    virtual ~PKMutex();

    virtual void wait();
	virtual bool tryWait();
    virtual void release();

protected:

    void* mutex;
};

// Auto-lock class

class PKMutexLock
{
public:

    PKMutexLock(PKMutex* mutex);

    virtual ~PKMutexLock();

    void release();

protected:

    PKMutex* mutex;
};


#endif // PK_MUTEX_H
