//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKVectorAnimation.h"

//
// PKVectorAnimation
//

PKVectorAnimation::PKVectorAnimation()
{
	this->handler   = NULL;
	this->eventName = "";
	this->context   = NULL;
}

PKVectorAnimation::~PKVectorAnimation()
{
	for(uint32_t i=0; i<this->blocks.size(); i++)
	{
		if(this->blocks[i])
		{
			delete this->blocks[i];
		}
	}

	this->blocks.clear();
}

void PKVectorAnimation::addAnimationBlock(PKVectorAnimationBlock *block)
{
	this->blocks.push_back(block);
}

uint32_t PKVectorAnimation::getNumberOfAnimationBlocks()
{
	return this->blocks.size();
}

PKVectorAnimationBlock *PKVectorAnimation::getAnimationBlockAt(uint32_t i)
{
	if(i >= this->blocks.size())
	{
		return NULL;
	}

	return this->blocks[i];
}

void PKVectorAnimation::setCompletionHandler(IPKSafeEventHandler *handler, std::string eventName, void *context)
{
	this->handler   = handler;
	this->eventName = eventName;
	this->context   = context;
}

IPKSafeEventHandler *PKVectorAnimation::getCompletionHandler()
{
	return this->handler;
}

std::string PKVectorAnimation::getCompletionEventName()
{
	return this->eventName;
}

void *PKVectorAnimation::getCompletionContext()
{
	return this->context;
}

