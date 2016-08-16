//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_VECTOR_ANIMATION_DESCRIPTOR_FADE_H
#define PK_VECTOR_ANIMATION_DESCRIPTOR_FADE_H

#include "PKVectorAnimationDescriptor.h"

//
// PKVectorAnimationDescriptorFade
//

class PKVectorAnimationDescriptorFade : public PKVectorAnimationDescriptor
{
public:

	PKVectorAnimationDescriptorFade(double alphaStart, double alphaEnd);
	~PKVectorAnimationDescriptorFade();

	double getAlphaStartValue();
	double getAlphaEndValue();

protected:

	double _alphaStart;
	double _alphaEnd;
};

#endif // PK_VECTOR_ANIMATION_DESCRIPTOR_FADE_H