//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKNeck.h"

//
// PKNeck
//

PKNeck::PKNeck(uint32_t numberOfStrings)
{
    // Create the strings

    for(uint32_t i=0; i < numberOfStrings; i++)
    {
        PKNote note;
        memset(&note, 0, sizeof(note));
        this->strings.push_back(new PKString(note));
    }
}

PKNeck::~PKNeck()
{
    // Clear

    for(uint32_t i=0; i < this->strings.size(); i++)
    {
        delete this->strings[i];
    }    
}

uint32_t PKNeck::getNumberOfStrings()
{
    return this->strings.size();
}

void PKNeck::applyTuning(PKTuning *tuning)
{
    if(tuning == NULL)
    {
        return;
    }

    if(tuning->getNumberOfStrings() != this->getNumberOfStrings())
    {
        return;
    }

    for(uint32_t i=0; i < tuning->getNumberOfStrings(); i++)
    {
        this->setOpenNoteForStringAt(i, tuning->getNoteForString(i)); 
    }
}

void PKNeck::setOpenNoteForStringAt(uint32_t index, PKNote note)
{
    if(index >= this->strings.size())
    {
        return;
    }

    this->strings[index]->setOpenNote(note);
}

PKNote PKNeck::getOpenNoteForStringAt(uint32_t index)
{
    if(index >= this->strings.size())
    {
        PKNote note;
        memset(&note, 0, sizeof(note));
        return note;
    }

    return this->strings[index]->getOpenNote();
}

PKString *PKNeck::getStringAt(uint32_t index)
{
    if(index >= this->strings.size())
    {
        return NULL;
    }

    return this->strings[index];
}

