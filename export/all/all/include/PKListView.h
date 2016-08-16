//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_LISTVIEW_H
#define PK_LISTVIEW_H

#include "PKControl.h"

//
// PKListView
//

class PKListView : public PKControl
{
public:

public:
	PKListView();
protected:
	~PKListView();

public:
    virtual void build();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

};

#endif // PK_LISTVIEW_H
