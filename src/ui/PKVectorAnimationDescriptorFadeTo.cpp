//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKVectorAnimationDescriptorFadeTo.h"

//
// PKVectorAnimationDescriptorFadeTo
//

PKVectorAnimationDescriptorFadeTo::PKVectorAnimationDescriptorFadeTo(PKVectorControl *control, double alphaEnd)
: PKVectorAnimationDescriptor(PK_VECTOR_ANIMATION_TYPE_FADE_TO)
{
    if(control == NULL)
    {
        this->_alphaStart = this->_alphaEnd = 0;
    }

	this->_alphaEnd   = alphaEnd;
    this->_alphaStart = PKVALUE_DOUBLE(control->get(PKVectorControl::ALPHA_DOUBLE_PROPERTY));
}

PKVectorAnimationDescriptorFadeTo::~PKVectorAnimationDescriptorFadeTo()
{

}

double PKVectorAnimationDescriptorFadeTo::getAlphaStartValue()
{
    return this->_alphaStart;
}

double PKVectorAnimationDescriptorFadeTo::getAlphaEndValue()
{
	return this->_alphaEnd;
}
