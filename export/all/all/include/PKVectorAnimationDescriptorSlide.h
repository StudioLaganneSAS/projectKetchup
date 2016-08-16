//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_VECTOR_ANIMATION_DESCRIPTOR_SLIDE_H
#define PK_VECTOR_ANIMATION_DESCRIPTOR_SLIDE_H

#include "PKVectorAnimationDescriptor.h"

//
// PKVectorAnimationDescriptorSlide
//

class PKVectorAnimationDescriptorSlide : public PKVectorAnimationDescriptor
{
public:

	PKVectorAnimationDescriptorSlide(double xStart, double yStart,
									 double xEnd,   double yEnd);

	~PKVectorAnimationDescriptorSlide();

	double getXStart();
	double getYStart();

	double getXEnd();
	double getYEnd();

protected:

	double _xStart;
	double _yStart;

	double _xEnd;
	double _yEnd;
};

#endif // PK_VECTOR_ANIMATION_DESCRIPTOR_SLIDE_H