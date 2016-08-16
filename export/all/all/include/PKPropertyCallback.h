//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKPROPERTY_CALLBACK_H
#define PKPROPERTY_CALLBACK_H

#include "PKTypes.h"

class PKProperty;

//
// PKPropertyCallback
//

class PKPropertyCallback {

public:

	virtual void onPropertyChanged(PKProperty *prop, PKVariant *oldValue) = 0;
};

#endif // PKPROPERTY_CALLBACK_H
