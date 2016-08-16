//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
// 

#ifndef PKFDATAMEDIATYPEDESCRIPTOR_H
#define PKFDATAMEDIATYPEDESCRIPTOR_H

#include "IPKFMediaTypeDescriptor.h"

class PKFDataMediaTypeDescriptor : public IPKFMediaTypeDescriptor {

public:

	PKFDataMediaTypeDescriptor();
	~PKFDataMediaTypeDescriptor();

    PKFMediaType getMediaType();

	uint32_t getDataSize();
	void     setDataSize(uint32_t size);

private:
	uint32_t size;
};

#endif // PKFDATAMEDIATYPEDESCRIPTOR_H
