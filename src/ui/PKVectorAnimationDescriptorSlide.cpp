//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKVectorAnimationDescriptorSlide.h"

//
// PKVectorAnimationDescriptorSlide
//

PKVectorAnimationDescriptorSlide::PKVectorAnimationDescriptorSlide(double xStart, double yStart,
																   double xEnd,   double yEnd)
: PKVectorAnimationDescriptor(PK_VECTOR_ANIMATION_TYPE_SLIDE)
{
	this->_xStart = xStart;
	this->_yStart = yStart;

	this->_xEnd = xEnd;
	this->_yEnd = yEnd;
}

PKVectorAnimationDescriptorSlide::~PKVectorAnimationDescriptorSlide()
{
}

double PKVectorAnimationDescriptorSlide::getXStart()
{
	return this->_xStart;
}

double PKVectorAnimationDescriptorSlide::getYStart()
{
	return this->_yStart;
}

double PKVectorAnimationDescriptorSlide::getXEnd()
{
	return this->_xEnd;
}

double PKVectorAnimationDescriptorSlide::getYEnd()
{
	return this->_yEnd;
}
