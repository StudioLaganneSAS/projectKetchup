//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_IMAGE_VECTOR_CONTROL_H
#define PK_IMAGE_VECTOR_CONTROL_H

#include "PKVectorControl.h"
#include "PKImage.h"

//
// PKImageVectorControl
//

class PKImageVectorControl : public PKVectorControl
{
public:

    //
    // PROPERTIES
    //

    static std::string IMAGE_WSTRING_PROPERTY;
	
public:
	PKImageVectorControl();

protected:
	virtual ~PKImageVectorControl();

public:

    // From PKControl
    void build();

    // From PKVectorControl
    virtual void drawVector(PKVectorEngine *engine,
		                    int32_t x, int32_t y);
		
private:
    PKImage *image;	
    bool built;
};

#endif // PK_IMAGE_VECTOR_CONTROL_H
