//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//


#include "PKVectorAnimationBlock.h"
#include "PKVectorControl.h"

//
// PKVectorAnimationBlock
//

PKVectorAnimationBlock::PKVectorAnimationBlock(PKVectorControl *obj, double start, double end,
											   PKVectorAnimationDescriptor *descriptor)
{
	this->_startTime = start;
	this->_endTime   = end;
	this->_object    = obj;
	this->_desc      = descriptor;

	this->done = false;
}

PKVectorAnimationBlock::~PKVectorAnimationBlock()
{
	if(this->_desc != NULL)
	{
		delete this->_desc;
	}
}

double PKVectorAnimationBlock::getStartTime()
{
	return this->_startTime;
}

double PKVectorAnimationBlock::getEndTime()
{
	return this->_endTime;
}

PKVectorControl *PKVectorAnimationBlock::getVectorControl()
{
	return this->_object;
}

PKVectorAnimationDescriptor *PKVectorAnimationBlock::getAnimationDescriptor()
{
	return this->_desc;
}

bool PKVectorAnimationBlock::getDone()
{
	return this->done;
}

void PKVectorAnimationBlock::setDone(bool done)
{
	this->done = done;
}
