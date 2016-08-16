//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKVectorAnimationDescriptorFade.h"

//
// PKVectorAnimationDescriptorFade
//

PKVectorAnimationDescriptorFade::PKVectorAnimationDescriptorFade(double alphaStart, double alphaEnd)
: PKVectorAnimationDescriptor(PK_VECTOR_ANIMATION_TYPE_FADE)
{
	this->_alphaStart = alphaStart;
	this->_alphaEnd   = alphaEnd;
}

PKVectorAnimationDescriptorFade::~PKVectorAnimationDescriptorFade()
{

}

double PKVectorAnimationDescriptorFade::getAlphaStartValue()
{
	return this->_alphaStart;
}

double PKVectorAnimationDescriptorFade::getAlphaEndValue()
{
	return this->_alphaEnd;
}
