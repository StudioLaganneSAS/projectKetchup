//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_NOTE_H
#define PK_NOTE_H

#include "PKTypes.h"
#include "PKNotes.h"
#include "PKi18n.h"

#include <string>

//
// PKNote structure
//

typedef struct _PKNote
{
    int8_t note;
    int8_t octave;

} PKNote;

//
// Helper functions
//

// Offset a note with a given interval (see PKNotes.h)

void PKNoteOffset(PKNote  noteIn, 
                  int8_t  interval,
                  PKNote *noteOut);

// Finds the interval in between two notes (can be negative !!)

int32_t PKNoteGetInterval(PKNote note1, PKNote note2);

// Gets a readable name for an interval

std::wstring PKNoteGetIntervalName(int8_t interval);

std::wstring PKNoteGetShortIntervalName(int8_t interval);

// Gets a readable short name for the Note

std::wstring PKNoteGetName(PKNote note, 
                           PKNotationName notation,
                           bool preferSharp = true);

#endif // PK_NOTE_H
