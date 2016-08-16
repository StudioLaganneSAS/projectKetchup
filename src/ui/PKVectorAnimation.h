//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_VECTOR_ANIMATION_H
#define PK_VECTOR_ANIMATION_H

#include "PKVectorAnimationBlock.h"
#include "PKVectorAnimationDescriptorFade.h"
#include "PKVectorAnimationDescriptorFadeTo.h"
#include "PKVectorAnimationDescriptorSlide.h"
#include "PKVectorAnimationDescriptor.h"

#include "PKSafeEvents.h"

#include <vector>

//
// PKVectorAnimation
//

class PKVectorAnimation
{
public:
	
	PKVectorAnimation();
	~PKVectorAnimation();

	void addAnimationBlock(PKVectorAnimationBlock *block);
	void setCompletionHandler(IPKSafeEventHandler *handler, std::string eventName, void *context);

	uint32_t getNumberOfAnimationBlocks();
	PKVectorAnimationBlock *getAnimationBlockAt(uint32_t i);

	IPKSafeEventHandler *getCompletionHandler();
	std::string          getCompletionEventName();
	void                *getCompletionContext();

protected:

	std::vector<PKVectorAnimationBlock *> blocks;

	IPKSafeEventHandler *handler;
	std::string          eventName;
	void                *context;
};


#endif // PK_VECTOR_ANIMATION_H
