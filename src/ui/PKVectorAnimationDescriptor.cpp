//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKVectorAnimationDescriptor.h"

//
// PKVectorAnimationDescriptor
//

PKVectorAnimationDescriptor::PKVectorAnimationDescriptor(PKVectorAnimationType type)
{
	this->_type = type;
}

PKVectorAnimationDescriptor::~PKVectorAnimationDescriptor()
{
}

PKVectorAnimationType PKVectorAnimationDescriptor::getAnimationType()
{
	return this->_type;
}
