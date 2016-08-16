//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_VECTOR_ANIMATION_DESCRIPTOR_FADE_TO_H
#define PK_VECTOR_ANIMATION_DESCRIPTOR_FADE_TO_H

#include "PKVectorAnimationDescriptor.h"
#include "PKVectorControl.h"

//
// PKVectorAnimationDescriptorFadeTo
//

class PKVectorAnimationDescriptorFadeTo : public PKVectorAnimationDescriptor
{
public:

	PKVectorAnimationDescriptorFadeTo(PKVectorControl *control, double alphaEnd);
	~PKVectorAnimationDescriptorFadeTo();

	double getAlphaStartValue();
	double getAlphaEndValue();

protected:

    double _alphaStart;
	double _alphaEnd;
};

#endif // PK_VECTOR_ANIMATION_DESCRIPTOR_FADE_TO_H