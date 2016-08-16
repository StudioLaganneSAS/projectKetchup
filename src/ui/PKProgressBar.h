//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_PROGRESSBAR_H
#define PK_PROGRESSBAR_H

#include "PKControl.h"

//
// PKProgressBar
//

class PKProgressBar : public PKControl
{
public:

    static std::string VALUE_INT32_PROPERTY;
    static std::string RANGE_INT32RANGE_PROPERTY;

public:
	PKProgressBar();
protected:
	~PKProgressBar();

public:
    void build();

	void setMinMax(int32_t min, 
				   int32_t max);
	
	void	setValue(int32_t value);
	int32_t getValue();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

};

#endif // PK_PRORGESSBAR_H
