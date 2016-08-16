//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_LITTLEARROWS_H
#define PK_LITTLEARROWS_H

#include "PKControl.h"

//
// PKLittleArrows
//

class PKLittleArrows : public PKControl
{
public:

	static std::string RANGE_INT32RANGE_PROPERTY;

public:
	PKLittleArrows();
protected:
	~PKLittleArrows();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);

	void setBuddyControl(PKControl *buddy);

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

#ifdef MACOSX
	void handleValueUp();
	void handleValueDown();
#endif

private:

	PKControl *buddy;
};

#endif // PK_LITTLEARROWS_H
