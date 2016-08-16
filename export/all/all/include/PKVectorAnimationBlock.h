//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_VECTOR_ANIMATION_BLOCK_H
#define PK_VECTOR_ANIMATION_BLOCK_H

#include "PKVectorAnimationDescriptor.h"

class PKVectorControl;

//
// PKVectorAnimationBlock
//

class PKVectorAnimationBlock
{
public:

	PKVectorAnimationBlock(PKVectorControl *obj, double start, double end,
						   PKVectorAnimationDescriptor *descriptor);

	~PKVectorAnimationBlock();

	double getStartTime();
	double getEndTime();

	PKVectorControl *getVectorControl();
	PKVectorAnimationDescriptor *getAnimationDescriptor();

	bool getDone();
	void setDone(bool done);

protected:

	bool done;

	double _startTime;
	double _endTime;

	PKVectorControl *_object;
	PKVectorAnimationDescriptor *_desc;
};

#endif // PK_VECTOR_ANIMATION_BLOCK_H