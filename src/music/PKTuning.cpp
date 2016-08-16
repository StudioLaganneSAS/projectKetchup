//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKTuning.h"

//
// Static helpers
//

PKTuning *PKTuning::getStandardGuitarTuning()
{
    PKNote note;
    PKTuning *tuning = new PKTuning(PK_i18n(L"Standard"));

    if(tuning == NULL)
    {
        return NULL;
    }

    note.note   = PKNOTE_E;
    note.octave = 2;
    tuning->addOpenString(note);

    note.note   = PKNOTE_A;
    note.octave = 2;
    tuning->addOpenString(note);

    note.note   = PKNOTE_D;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_G;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_B;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_E;
    note.octave = 4;
    tuning->addOpenString(note);

    return tuning;
}

PKTuning *PKTuning::getHalfStepDownGuitarTuning()
{
    PKNote note;
    PKTuning *tuning = new PKTuning(PK_i18n(L"Half-Step Down"));
	
    if(tuning == NULL)
    {
        return NULL;
    }
	
    note.note   = PKNOTE_D_SHARP;
    note.octave = 2;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_G_SHARP;
    note.octave = 2;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_C_SHARP;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_F_SHARP;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_A_SHARP;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_D_SHARP;
    note.octave = 4;
    tuning->addOpenString(note);
	
    return tuning;	
}

PKTuning *PKTuning::getWholeStepDownGuitarTuning()
{
    PKNote note;
    PKTuning *tuning = new PKTuning(PK_i18n(L"Whole-Step Down"));
	
    if(tuning == NULL)
    {
        return NULL;
    }
	
    note.note   = PKNOTE_D;
    note.octave = 2;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_G;
    note.octave = 2;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_C;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_F;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_A;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_D;
    note.octave = 4;
    tuning->addOpenString(note);
	
    return tuning;
}

PKTuning *PKTuning::getDropDGuitarTuning()
{
    PKNote note;
    PKTuning *tuning = new PKTuning(PK_i18n(L"Drop D"));

    if(tuning == NULL)
    {
        return NULL;
    }

    note.note   = PKNOTE_D;
    note.octave = 2;
    tuning->addOpenString(note);

    note.note   = PKNOTE_A;
    note.octave = 2;
    tuning->addOpenString(note);

    note.note   = PKNOTE_D;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_G;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_B;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_E;
    note.octave = 4;
    tuning->addOpenString(note);

    return tuning;
}

PKTuning *PKTuning::getOpenGGuitarTuning()
{
    PKNote note;
    PKTuning *tuning = new PKTuning(PK_i18n(L"Open G"));
	
    if(tuning == NULL)
    {
        return NULL;
    }
	
    note.note   = PKNOTE_D;
    note.octave = 2;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_G;
    note.octave = 2;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_D;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_G;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_B;
    note.octave = 3;
    tuning->addOpenString(note);
	
    note.note   = PKNOTE_D;
    note.octave = 4;
    tuning->addOpenString(note);
	
    return tuning;
}

PKTuning *PKTuning::getDadgadGuitarTuning()
{
    PKNote note;
    PKTuning *tuning = new PKTuning(PK_i18n(L"Dadgad"));

    if(tuning == NULL)
    {
        return NULL;
    }

    note.note   = PKNOTE_D;
    note.octave = 2;
    tuning->addOpenString(note);

    note.note   = PKNOTE_A;
    note.octave = 2;
    tuning->addOpenString(note);

    note.note   = PKNOTE_D;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_G;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_A;
    note.octave = 3;
    tuning->addOpenString(note);

    note.note   = PKNOTE_D;
    note.octave = 4;
    tuning->addOpenString(note);

    return tuning;
}

std::vector<PKTuning*> PKTuning::getAllGuitarTunings()
{
    std::vector<PKTuning*> tunings;

	tunings.push_back(PKTuning::getStandardGuitarTuning());
	tunings.push_back(PKTuning::getHalfStepDownGuitarTuning());
	tunings.push_back(PKTuning::getWholeStepDownGuitarTuning());
	tunings.push_back(PKTuning::getDropDGuitarTuning());
	tunings.push_back(PKTuning::getOpenGGuitarTuning());
	tunings.push_back(PKTuning::getDadgadGuitarTuning());

	return tunings;
}

//
// PKTuning
//

PKTuning::PKTuning(std::wstring name)
{
	this->name = name;
}

PKTuning::~PKTuning()
{

}

std::wstring PKTuning::getName()
{
	return this->name;
}

void PKTuning::addOpenString(PKNote note)
{
    this->openStrings.push_back(note);
}

uint32_t PKTuning::getNumberOfStrings()
{
    return this->openStrings.size();
}

PKNote PKTuning::getNoteForString(uint32_t index)
{
    if(index >= this->getNumberOfStrings())
    {
        PKNote note;
        memset(&note, 0, sizeof(note));
        return note;
    }

    return this->openStrings[index];
}

