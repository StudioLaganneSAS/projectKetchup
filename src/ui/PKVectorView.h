//
// PKVectorView.h
// --------------
//

#ifndef PK_VECTOR_VIEW_H
#define PK_VECTOR_VIEW_H

#include "PKControl.h"
#include "PKVectorEngine.h"

//
// PKVectorView
//

class PKVectorView : public PKControl
{
public:

	//
	// PROPERTIES
	//

public:
	PKVectorView();

protected:
    ~PKVectorView();

	//
	// Vector draw
	// 

public:
    virtual void drawVector(PKVectorEngine *engine,
		                    int32_t x, int32_t y);

private:


};

#endif // PK_VECTOR_VIEW_H
