//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_VECTOR_ANIMATION_DESCRIPTOR_H
#define PK_VECTOR_ANIMATION_DESCRIPTOR_H

#include "PKUI.h"
#include "PKTypes.h"

//
// PKVectorAnimationType
// 

typedef enum {

	PK_VECTOR_ANIMATION_TYPE_NONE,
	PK_VECTOR_ANIMATION_TYPE_FADE,
	PK_VECTOR_ANIMATION_TYPE_SLIDE,
	PK_VECTOR_ANIMATION_TYPE_FADE_TO,

} PKVectorAnimationType;

//
// PKVectorAnimationDescriptor
//

class PKVectorAnimationDescriptor
{
public:

	PKVectorAnimationDescriptor(PKVectorAnimationType type);
	virtual ~PKVectorAnimationDescriptor();

	PKVectorAnimationType getAnimationType();

protected:

	PKVectorAnimationType _type;
};

#endif // PK_VECTOR_ANIMATION_DESCRIPTOR_H