//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_ANIAMTION_CONTROL_H
#define PK_ANIAMTION_CONTROL_H

#include "PKDoubleBufferedControl.h"
#include "PKImage.h"
#include "PKThread.h"

//
// PKAnimationControl
//

class PKAnimationControl : public PKDoubleBufferedControl, public IPKRunnable
{
public:

    static std::string FRAMERATE_DOUBLE_PROPERTY;
	static std::string IMAGE_WSTRING_PROPERTY;
	static std::string NUMFRAMES_UINT32_PROPERTY;

public:
	PKAnimationControl();

protected:
	virtual ~PKAnimationControl();

public:
    virtual void build();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

    // Start / Stop

    void start();
    void stop();

    // PKThread
    virtual void runThread(PKThread *thread);

    // From PKCustomControl
	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

protected:

    virtual void draw32bpp(uint8_t *buffer);

private:

	uint32_t width;
	uint32_t height;
	PKImage *bitmap;
	bool built;   
    uint32_t currentFrame;
    PKThread *thread;
};

#endif // PK_DB_CONTROL_H
