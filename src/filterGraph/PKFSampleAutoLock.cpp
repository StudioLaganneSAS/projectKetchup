//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFSampleAutoLock.h"

PKFSampleAutoLock::PKFSampleAutoLock(uint32_t sampleId)
{
	this->sampleId = 0;

	PKFReturnCode result = PKFSampleManager::lockSample(sampleId, &this->sample);

	if(result != PKF_SUCCESS)
	{
		this->sample   = NULL;
		this->sampleId = 0;

		return;
	}

	this->sampleId = sampleId;
}

PKFSampleAutoLock::~PKFSampleAutoLock()
{
	if(this->sample != NULL)
	{
		PKFSampleManager::unlockSample(this->sampleId);

		this->sample   = NULL;
		this->sampleId = 0;
	}
}

IPKFSample *PKFSampleAutoLock::getSample()
{
	return this->sample;
}

