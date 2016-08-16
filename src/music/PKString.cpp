//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKString.h"

//
// PKString
//

PKString::PKString(PKNote openNote)
{
    this->openNote = openNote;
    this->maxFrets = 24; // Default value (Guitar)
}

PKString::~PKString()
{

}

PKNote PKString::getOpenNote()
{
    return this->openNote;
}

void PKString::setOpenNote(PKNote note)
{
    this->openNote = note;
}

uint32_t PKString::getMaximumNumberOfFrets()
{
    return this->maxFrets;
}

void PKString::setMaximumNumberOfFrets(uint32_t max)
{
    this->maxFrets = max;
}

PKNote PKString::getNoteAtFret(uint32_t fret)
{
    PKNote note;
    PKNoteOffset(this->openNote, fret, &note);
    return note;
}

bool PKString::findFirstNoteOnString(PKNote note, int32_t *outFret)
{
    return this->findFirstNoteOnStringStartingAtFret(note, 0, outFret); 
}

bool PKString::findFirstNoteOnStringStartingAtFret(PKNote note, int32_t inFret, int32_t *outFret)
{
    if(outFret == NULL)
    {
        return false;
    }

    for(uint32_t i=inFret; i <= this->maxFrets; i++)
    {
        PKNote current = this->getNoteAtFret(i);

        if(current.note == note.note)
        {
            *outFret = i;
            return true;
        }
    }

    return false;
}

bool PKString::findFullNoteOnString(PKNote note, int32_t *outFret)
{
    return this->findFullNoteOnStringStartingAtFret(note, 0, outFret); 
}

bool PKString::findFullNoteOnStringStartingAtFret(PKNote note, 
                                                  int32_t inFret, 
                                                  int32_t *outFret)
{
    if(outFret == NULL)
    {
        return false;
    }

    for(uint32_t i=inFret; i <= this->maxFrets; i++)
    {
        PKNote current = this->getNoteAtFret(i);

        if(current.note == note.note &&
           current.octave == note.octave)
        {
            *outFret = i;
            return true;
        }
    }

    return false;
}
