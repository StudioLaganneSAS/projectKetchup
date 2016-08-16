//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKFCUSTOMMEDIATYPEDESCRIPTOR_H
#define PKFCUSTOMMEDIATYPEDESCRIPTOR_H

#include "IPKFMediaTypeDescriptor.h"

// PKFCustomMediaTypeDescriptor
// ----------------------------
//
// This class defines a custom media type
// and since it inherits from PKObject
// it holds an indentifier to the custom`
// media type as a string property called
// ::IDENTIFIER_PROPERTY.
//

class PKFCustomMediaTypeDescriptor : public IPKFMediaTypeDescriptor {

public:
    PKFCustomMediaTypeDescriptor(std::string typeDescriptor);
    ~PKFCustomMediaTypeDescriptor();

    PKFMediaType getMediaType();

public:
	static std::string TYPE_IDENTIFIER_PROPERTY;
};

#endif // PKFCUSTOMMEDIATYPEDESCRIPTOR_H
