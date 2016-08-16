//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_STRING_H
#define PK_STRING_H

#include "PKNote.h"

//
// PKString
//

class PKString
{
public:

    PKString(PKNote openNote);
    ~PKString();

    PKNote getOpenNote();
    void   setOpenNote(PKNote note);

    uint32_t getMaximumNumberOfFrets();
    void     setMaximumNumberOfFrets(uint32_t max);

    //
    // Utils
    //

    PKNote getNoteAtFret(uint32_t fret);

    // Finds a note by matching the note name only

    bool findFirstNoteOnString(PKNote note, int32_t *outFret);
    bool findFirstNoteOnStringStartingAtFret(PKNote note, int32_t inFret, int32_t *outFret);

	// Finds a note by matching the note name AND the octave

    bool findFullNoteOnString(PKNote note, int32_t *outFret);
    bool findFullNoteOnStringStartingAtFret(PKNote note, int32_t inFret, int32_t *outFret);

private:

    PKNote   openNote;
    uint32_t maxFrets;
};

#endif // PK_STRING_H
