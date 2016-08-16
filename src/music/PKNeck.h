//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_NECK_H
#define PK_NECK_H

#include "PKString.h"
#include "PKTuning.h"

//
// PKNeck
//

class PKNeck
{
public:

    PKNeck(uint32_t numberOfStrings);
    ~PKNeck();

    void applyTuning(PKTuning *tuning);

    uint32_t getNumberOfStrings();

    void   setOpenNoteForStringAt(uint32_t index, PKNote note);
    PKNote getOpenNoteForStringAt(uint32_t index);

    PKString *getStringAt(uint32_t index);

private:

    std::vector<PKString *> strings;
};

#endif // PK_NECK_H