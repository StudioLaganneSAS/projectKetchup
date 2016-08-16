//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SPACER_H
#define PK_SPACER_H

#include "PKControl.h"

//
// PKSpacerType
//

typedef enum {

	PK_SPACER_HORIZONTAL = 0,
	PK_SPACER_VERTICAL   = 1,

} PKSpacerType;

//
// PKSpacer
//

class PKSpacer : public PKControl
{
public:

	static std::string TYPE_INT32_PROPERTY;

public:
	PKSpacer();
protected:
	~PKSpacer();

public:
    void build();

    void setSpacerType(PKSpacerType type);
	PKSpacerType getSpacerType();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

private:

};

#endif // PK_SPACER_H
