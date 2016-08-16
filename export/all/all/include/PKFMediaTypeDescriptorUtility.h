//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#ifndef PKFMEDIA_TYPE_DESCRIPTOR_UTILITY_H
#define PKFMEDIA_TYPE_DESCRIPTOR_UTILITY_H

#include "IPKFMediaTypeDescriptor.h"
#include "PKFImageMediaTypeDescriptor.h"
#include "PKFDataMediaTypeDescriptor.h"
#include "PKFCustomMediaTypeDescriptor.h"
#include "PKFImageChannel.h"

//
// PKFMediaTypeDescriptorUtility
//

class PKFMediaTypeDescriptorUtility {

public:

	static bool matchMediaTypes(IPKFMediaTypeDescriptor *t1, IPKFMediaTypeDescriptor *t2);

	static IPKFMediaTypeDescriptor *create8BitARGBImageMediaType(uint32_t width = 0, uint32_t height = 0);
	static IPKFMediaTypeDescriptor *create8BitRGBImageMediaType(uint32_t width = 0, uint32_t height = 0);
	static IPKFMediaTypeDescriptor *create8BitGrayscaleImageMediaType(uint32_t width = 0, uint32_t height = 0);

	static IPKFMediaTypeDescriptor *create16BitARGBImageMediaType(uint32_t width = 0, uint32_t height = 0);
	static IPKFMediaTypeDescriptor *create16BitRGBImageMediaType(uint32_t width = 0, uint32_t height = 0);
	static IPKFMediaTypeDescriptor *create16BitGrayscaleImageMediaType(uint32_t width = 0, uint32_t height = 0);

	static IPKFMediaTypeDescriptor *createFloatARGBImageMediaType(uint32_t width = 0, uint32_t height = 0);
	static IPKFMediaTypeDescriptor *createFloatRGBImageMediaType(uint32_t width = 0, uint32_t height = 0);
	static IPKFMediaTypeDescriptor *createFloatGrayscaleImageMediaType(uint32_t width = 0, uint32_t height = 0);
};

#endif // PKFMEDIA_TYPE_DESCRIPTOR_UTILITY_H
