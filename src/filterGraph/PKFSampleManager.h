//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKFSAMPLEMANAGER_H
#define PKFSAMPLEMANAGER_H

#include "PKFTypes.h"

#include "IPKFSample.h"
#include "PKFImageSample.h"
#include "PKFDataSample.h"
#include "PKFCustomSample.h"

#include "IPKFMediaTypeDescriptor.h"
#include "PKFImageMediaTypeDescriptor.h"
#include "PKFDataMediaTypeDescriptor.h"
#include "PKFCustomMediaTypeDescriptor.h"

#include "PKCritSec.h"
#include <map>

//
// PKFSampleManager
//

class PKFSampleManager 
{
public:

	static PKFReturnCode setScratchPath(std::wstring path);
	static std::wstring  getScratchPath();

	static PKFReturnCode           setDefaultAllocationType(PKFSampleAllocationType type);
	static PKFSampleAllocationType getDefaultAllocationType();

    static PKFReturnCode allocateSample(IPKFMediaTypeDescriptor *desc, 
										uint32_t *sampleId,
										PKFSampleAllocationType type = PKF_SAMPLE_ALLOCATE_DEFAULT);

    static PKFReturnCode deallocateSample(uint32_t sampleId);

    static PKFReturnCode lockSample(uint32_t sampleId, 
									IPKFSample **sample);

    static PKFReturnCode unlockSample(uint32_t sampleId);

private:
	static PKCritSec mutex;
	static int sampleCount;
	static std::map <uint32_t, IPKFSample *> samples;
	static std::wstring scratchPath;
	static PKFSampleAllocationType allocType;
};

#endif // PKFSAMPLEMANAGER_H
