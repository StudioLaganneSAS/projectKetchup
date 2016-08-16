//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_TUNING_H
#define PK_TUNING_H

#include "PKNote.h"

//
// PKTuning
//

class PKTuning
{
public:

    static PKTuning *getStandardGuitarTuning();
    static PKTuning *getHalfStepDownGuitarTuning();
    static PKTuning *getWholeStepDownGuitarTuning();
    static PKTuning *getDropDGuitarTuning();
	static PKTuning *getOpenGGuitarTuning();
    static PKTuning *getDadgadGuitarTuning();

	// Accessor 

	static std::vector<PKTuning*> getAllGuitarTunings();

public:

	PKTuning(std::wstring name);
    ~PKTuning();

	std::wstring getName();

    void addOpenString(PKNote note);

    uint32_t getNumberOfStrings();
    PKNote   getNoteForString(uint32_t index);

private:

	std::wstring name;
    std::vector<PKNote> openStrings;
};

#endif // PK_TUNING_H
