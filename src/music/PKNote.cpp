//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKNote.h"

//
// Helper Functions
//

//
// PKNoteOffset
//

void PKNoteOffset(PKNote  noteIn, 
                  int8_t  interval,
                  PKNote *noteOut)
{
    if(noteOut == NULL)
    {
        return;
    }

    int32_t newNote = (int32_t) noteIn.note + interval;
    int32_t offset  = newNote / PKNOTE_NUMBER_OF_NOTES;

    newNote = newNote % 12;

    noteOut->note   = (int8_t) newNote;
    noteOut->octave = noteIn.octave + offset;
}

//
// PKNoteGetInterval
//

int32_t PKNoteGetInterval(PKNote note1, PKNote note2)
{
    int32_t octaveDiff = note2.octave - note1.octave;
    int32_t diff       = (note2.note - note1.note + 
                          PKNOTE_NUMBER_OF_NOTES*octaveDiff);

    return diff; 
}

//
// PKNoteGetIntervalName
//

std::wstring PKNoteGetIntervalName(int8_t interval)
{
    switch(interval)
    {
    case PK_INTERVAL_BASE:
        {
            return PK_i18n(L"Fundamental");
        }
        break;
    case PK_INTERVAL_2ND_MIN:
        {
            return PK_i18n(L"Minor Second");
        }
        break;
    case PK_INTERVAL_2ND_MAJ:
        {
            return PK_i18n(L"Major Second");
        }
        break;
    case PK_INTERVAL_3RD_MIN:
        {
            return PK_i18n(L"Minor Third");
        }
        break;
    case PK_INTERVAL_3RD_MAJ:
        {
            return PK_i18n(L"Major Third");
        }
        break;
    case PK_INTERVAL_4TH:
        {
            return PK_i18n(L"Fourth");
        }
        break;
    case PK_INTERVAL_5TH_DIM:
        {
            return PK_i18n(L"Diminished Fifth");
        }
        break;
    case PK_INTERVAL_5TH:
        {
            return PK_i18n(L"Fifth");
        }
        break;
    case PK_INTERVAL_5TH_AUG:
        {
            return PK_i18n(L"Augmented Fifth");
        }
        break;
    case PK_INTERVAL_6TH:
        {
            return PK_i18n(L"Sixth");
        }
        break;
    case PK_INTERVAL_7TH_MIN:
        {
            return PK_i18n(L"Minor Seventh");
        }
        break;
    case PK_INTERVAL_7TH_MAJ:
        {
            return PK_i18n(L"Major Seventh");
        }
        break;
    case PK_INTERVAL_OCTAVE:
        {
            return PK_i18n(L"Octave");
        }
        break;
	case PK_INTERVAL_9TH_DIM:
        {
            return PK_i18n(L"Diminished Ninth");
        }
        break;
	case PK_INTERVAL_9TH:     
        {
            return PK_i18n(L"Ninth");
        }
        break;
	case PK_INTERVAL_9TH_AUG: 
        {
            return PK_i18n(L"Augmented Ninth");
        }
        break;
	case PK_INTERVAL_10TH:    
        {
            return PK_i18n(L"Tenth");
        }
        break;
	case PK_INTERVAL_11TH:    
        {
            return PK_i18n(L"Eleventh");
        }
        break;
	case PK_INTERVAL_12TH_DIM:
        {
            return PK_i18n(L"Diminished Twelvth");
        }
        break;
	case PK_INTERVAL_12TH:    
        {
            return PK_i18n(L"Twelvth");
        }
        break;
	case PK_INTERVAL_12TH_AUG:
        {
            return PK_i18n(L"Augmented Twelvth");
        }
        break;
	case PK_INTERVAL_13TH:
        {
            return PK_i18n(L"Thirteenth");
        }
        break;
    }

    return PK_i18n(L"Unknown");
}

std::wstring PKNoteGetShortIntervalName(int8_t interval)
{
    switch(interval)
    {
    case PK_INTERVAL_BASE:
        {
            return PK_i18n(L"Root");
        }
        break;
    case PK_INTERVAL_2ND_MIN:
        {
            return PK_i18n(L"2nd min");
        }
        break;
    case PK_INTERVAL_2ND_MAJ:
        {
            return PK_i18n(L"2nd");
        }
        break;
    case PK_INTERVAL_3RD_MIN:
        {
            return PK_i18n(L"3rd min");
        }
        break;
    case PK_INTERVAL_3RD_MAJ:
        {
            return PK_i18n(L"3rd");
        }
        break;
    case PK_INTERVAL_4TH:
        {
            return PK_i18n(L"4th");
        }
        break;
    case PK_INTERVAL_5TH_DIM:
        {
            return PK_i18n(L"5th dim");
        }
        break;
    case PK_INTERVAL_5TH:
        {
            return PK_i18n(L"5th");
        }
        break;
    case PK_INTERVAL_5TH_AUG:
        {
            return PK_i18n(L"5th aug");
        }
        break;
    case PK_INTERVAL_6TH:
        {
            return PK_i18n(L"6th");
        }
        break;
    case PK_INTERVAL_7TH_MIN:
        {
            return PK_i18n(L"7th min");
        }
        break;
    case PK_INTERVAL_7TH_MAJ:
        {
            return PK_i18n(L"7th maj");
        }
        break;
    case PK_INTERVAL_OCTAVE:
        {
            return PK_i18n(L"Octave");
        }
        break;
	case PK_INTERVAL_9TH_DIM:
        {
            return PK_i18n(L"9th dim");
        }
        break;
	case PK_INTERVAL_9TH:     
        {
            return PK_i18n(L"9th");
        }
        break;
	case PK_INTERVAL_9TH_AUG: 
        {
            return PK_i18n(L"9th aug");
        }
        break;
	case PK_INTERVAL_10TH:    
        {
            return PK_i18n(L"10th");
        }
        break;
	case PK_INTERVAL_11TH:    
        {
            return PK_i18n(L"11th");
        }
        break;
	case PK_INTERVAL_12TH_DIM:
        {
            return PK_i18n(L"12th dim");
        }
        break;
	case PK_INTERVAL_12TH:    
        {
            return PK_i18n(L"12th");
        }
        break;
	case PK_INTERVAL_12TH_AUG:
        {
            return PK_i18n(L"12th aug");
        }
        break;
	case PK_INTERVAL_13TH:
        {
            return PK_i18n(L"13th");
        }
        break;
    }

    return PK_i18n(L"??");
}

//
// PKNoteName
//

std::wstring PKNoteGetName(PKNote note, 
                           PKNotationName notation,
                           bool preferSharp)
{
    if(notation == PKNOTATION_LATIN ||
       notation == PKNOTATION_NAMES)
    {
        switch(note.note)
        {
        case PKNOTE_C:
            {
                return PK_i18n(L"Do");
            }
            break;
        case PKNOTE_C_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"Do#");
                }
                else
                {
                    return PK_i18n(L"Reb");
                }
            }
            break;
        case PKNOTE_D:
            {
                return PK_i18n(L"Re");
            }
            break;
        case PKNOTE_D_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"Re#");
                }
                else
                {
                    return PK_i18n(L"Mib");
                }
            }
            break;
        case PKNOTE_E:
            {
                return PK_i18n(L"Mi");
            }
            break;
        case PKNOTE_F:
            {
                return PK_i18n(L"Fa");
            }
            break;
        case PKNOTE_F_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"Fa#");
                }
                else
                {
                    return PK_i18n(L"Solb");
                }
            }
            break;
        case PKNOTE_G:
            {
                return PK_i18n(L"Sol");
            }
            break;
        case PKNOTE_G_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"Sol#");
                }
                else
                {
                    return PK_i18n(L"Lab");
                }
            }
            break;
        case PKNOTE_A:
            {
                return PK_i18n(L"La");
            }
            break;
        case PKNOTE_A_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"La#");
                }
                else
                {
                    return PK_i18n(L"Sib");
                }
            }
            break;
        case PKNOTE_B:
            {
                return PK_i18n(L"Si");
            }
            break;
        }
    }

    if(notation == PKNOTATION_US_UK ||
       notation == PKNOTATION_LETTERS)
    {
        switch(note.note)
        {
        case PKNOTE_C:
            {
                return PK_i18n(L"C");
            }
            break;
        case PKNOTE_C_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"C#");
                }
                else
                {
                    return PK_i18n(L"Cb");
                }
            }
            break;
        case PKNOTE_D:
            {
                return PK_i18n(L"D");
            }
            break;
        case PKNOTE_D_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"D#");
                }
                else
                {
                    return PK_i18n(L"Eb");
                }
            }
            break;
        case PKNOTE_E:
            {
                return PK_i18n(L"E");
            }
            break;
        case PKNOTE_F:
            {
                return PK_i18n(L"F");
            }
            break;
        case PKNOTE_F_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"F#");
                }
                else
                {
                    return PK_i18n(L"Gb");
                }
            }
            break;
        case PKNOTE_G:
            {
                return PK_i18n(L"G");
            }
            break;
        case PKNOTE_G_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"G#");
                }
                else
                {
                    return PK_i18n(L"Ab");
                }
            }
            break;
        case PKNOTE_A:
            {
                return PK_i18n(L"A");
            }
            break;
        case PKNOTE_A_SHARP:
            {
                if(preferSharp)
                {
                    return PK_i18n(L"A#");
                }
                else
                {
                    return PK_i18n(L"Bb");
                }
            }
            break;
        case PKNOTE_B:
            {
                return PK_i18n(L"B");
            }
            break;
        }
    }

    // No match

    return L"";
}
