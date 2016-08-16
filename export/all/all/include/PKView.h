//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKVIEW_H
#define PKVIEW_H

#include "PKUI.h"
#include "PKObject.h"

//
// PKView Base Class
//

class PKView : public PKObject
{
public:

	PKView();

	virtual PKWindowHandle getWindowHandle() = 0;

#ifdef WIN32
    virtual HBRUSH getBackgroundBrush() = 0;
#endif

protected:
	virtual ~PKView();
};

#endif // PKVIEW_H
