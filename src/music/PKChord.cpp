//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKChord.h"

//
// Helper Functions
//

PKChordDescription PKChordGetMajorChordDescription()
{
    PKChordDescription desc;

    desc.shortName = L"";
    desc.longName  = PK_i18n(L"Major");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);

    return desc;
}

PKChordDescription PKChordGetMinorChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m");
    desc.longName  = PK_i18n(L"Minor");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);

    return desc;
}

PKChordDescription PKChordGetSus4ChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"sus4");
    desc.longName  = PK_i18n(L"Sus4");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_4TH);
    desc.intervals.push_back(PK_INTERVAL_5TH);

    return desc;
}

PKChordDescription PKChordGetSus2ChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"sus2");
    desc.longName  = PK_i18n(L"Sus2");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_2ND_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);

    return desc;
}

PKChordDescription PKChordGetSeventhChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"7");
    desc.longName  = PK_i18n(L"7th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);

    return desc;
}

PKChordDescription PKChordGetMajorSeventhChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"7M");
    desc.longName  = PK_i18n(L"Major7th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MAJ);

    return desc;
}

PKChordDescription PKChordGetMinorSeventhChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m7");
    desc.longName  = PK_i18n(L"Minor 7th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);

    return desc;
}

PKChordDescription PKChordGetMinorMajorSeventhChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m7M");
    desc.longName  = PK_i18n(L"Minor Major7th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MAJ);

    return desc;
}

PKChordDescription PKChordGetSixthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"6");
    desc.longName  = PK_i18n(L"6th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_6TH);

    return desc;
}

PKChordDescription PKChordGetMinorSixthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m6");
    desc.longName  = PK_i18n(L"Minor 6th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_6TH);

    return desc;
}

PKChordDescription PKChordGetAugmentedChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"aug");
    desc.longName  = PK_i18n(L"Augmented");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH_AUG);

    return desc;
}

PKChordDescription PKChordGetDiminishedChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"dim");
    desc.longName  = PK_i18n(L"Diminished");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH_DIM);

    return desc;
}

PKChordDescription PKChordGetDiminishedSeventhChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"dim7");
    desc.longName  = PK_i18n(L"Dim7th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH_DIM);
    desc.intervals.push_back(PK_INTERVAL_6TH);

    return desc;
}

PKChordDescription PKChordGetAddedNinthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"add9");
    desc.longName  = PK_i18n(L"Added 9th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_9TH);

    return desc;
}

PKChordDescription PKChordGetMinorAddedNinthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"madd9");
    desc.longName  = PK_i18n(L"Minor Add9th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_9TH);

    return desc;
}

PKChordDescription PKChordGetNinthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"9");
    desc.longName  = PK_i18n(L"9th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH);

    return desc;
}

PKChordDescription PKChordGetMajorNinthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"9M");
    desc.longName  = PK_i18n(L"Major9th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH_AUG);

	return desc;
}

PKChordDescription PKChordGetMinorNinthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m9");
    desc.longName  = PK_i18n(L"Minor 9th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH);

    return desc;
}

PKChordDescription PKChordGetMinorMajorNinthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m9M");
    desc.longName  = PK_i18n(L"Minor Major9th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH_AUG);

	return desc;
}

PKChordDescription PKChordGetEleventhChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"11");
    desc.longName  = PK_i18n(L"11th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH);
    desc.intervals.push_back(PK_INTERVAL_11TH);

    return desc;
}

PKChordDescription PKChordGetMinorEleventhChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m11");
    desc.longName  = PK_i18n(L"Minor 11th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH);
    desc.intervals.push_back(PK_INTERVAL_11TH);

    return desc;
}

PKChordDescription PKChordGetSixNineChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"6/9");
    desc.longName  = PK_i18n(L"Sixth/Ninth");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_6TH);
    desc.intervals.push_back(PK_INTERVAL_9TH);

    return desc;
}

PKChordDescription PKChordGetMinorSixNineChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m6/9");
    desc.longName  = PK_i18n(L"Minor 6/9");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_6TH);
    desc.intervals.push_back(PK_INTERVAL_9TH);

    return desc;
}

PKChordDescription PKChordGetThirteenthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"13");
    desc.longName  = PK_i18n(L"13th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH);
    desc.intervals.push_back(PK_INTERVAL_11TH);
    desc.intervals.push_back(PK_INTERVAL_13TH);

    return desc;	
}

PKChordDescription PKChordGetMinorThirteenthChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"m13");
    desc.longName  = PK_i18n(L"Minor 13th");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_3RD_MIN);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);
    desc.intervals.push_back(PK_INTERVAL_9TH);
    desc.intervals.push_back(PK_INTERVAL_11TH);
    desc.intervals.push_back(PK_INTERVAL_13TH);

    return desc;		
}

PKChordDescription PKChordGetSeventhSus2ChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"7sus2");
    desc.longName  = PK_i18n(L"Seventh Sus2");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_2ND_MAJ);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);

    return desc;
}

PKChordDescription PKChordGetSeventhSus4ChordDescription()
{
    PKChordDescription desc;

    desc.shortName = PK_i18n(L"7sus4");
    desc.longName  = PK_i18n(L"Seventh Sus4");

    desc.intervals.push_back(PK_INTERVAL_BASE);
    desc.intervals.push_back(PK_INTERVAL_4TH);
    desc.intervals.push_back(PK_INTERVAL_5TH);
    desc.intervals.push_back(PK_INTERVAL_7TH_MIN);

    return desc;
}

std::vector <PKChordDescription> PKChordGetAllChordsDescriptions()
{
    std::vector <PKChordDescription> descs;

    descs.push_back(PKChordGetMajorChordDescription());
    descs.push_back(PKChordGetMinorChordDescription());

    descs.push_back(PKChordGetSeventhChordDescription());
    descs.push_back(PKChordGetMajorSeventhChordDescription());
    descs.push_back(PKChordGetMinorSeventhChordDescription());
    descs.push_back(PKChordGetMinorMajorSeventhChordDescription());
    descs.push_back(PKChordGetSixthChordDescription());
    descs.push_back(PKChordGetMinorSixthChordDescription());
    descs.push_back(PKChordGetSus2ChordDescription());
    descs.push_back(PKChordGetSus4ChordDescription());
    descs.push_back(PKChordGetAugmentedChordDescription());
    descs.push_back(PKChordGetDiminishedChordDescription());
	descs.push_back(PKChordGetDiminishedSeventhChordDescription());
    descs.push_back(PKChordGetAddedNinthChordDescription());
    descs.push_back(PKChordGetMinorAddedNinthChordDescription());
    descs.push_back(PKChordGetNinthChordDescription());
    descs.push_back(PKChordGetMajorNinthChordDescription());
    descs.push_back(PKChordGetMinorNinthChordDescription());
    descs.push_back(PKChordGetMinorMajorNinthChordDescription());
	descs.push_back(PKChordGetEleventhChordDescription());
    descs.push_back(PKChordGetMinorEleventhChordDescription());
    descs.push_back(PKChordGetThirteenthChordDescription());
    descs.push_back(PKChordGetMinorThirteenthChordDescription());
	descs.push_back(PKChordGetSixNineChordDescription());
	descs.push_back(PKChordGetMinorSixNineChordDescription());
    descs.push_back(PKChordGetSeventhSus2ChordDescription());
    descs.push_back(PKChordGetSeventhSus4ChordDescription());

    return descs;
}

//
// Chord Helpers
//

void PKChordReplaceInterval(PKChordDescription *chord,
							int8_t intervalToRepace, 
							int8_t replacement)
{
	if(chord == NULL)
	{
		return;
	}

	for(uint32_t i=0; i<chord->intervals.size(); i++)
	{
		if(chord->intervals[i] == intervalToRepace)
		{
			chord->intervals[i] = replacement;
			return;
		}
	}
}


PKChordStruct PKChordGetChordForNoteAndDescription(PKNote base, 
                                                   PKChordDescription desc)
{
    PKChordStruct chord;

    for(uint32_t i=0; i < desc.intervals.size(); i++)
    {
        PKNote nextNote;
        PKNoteOffset(base, desc.intervals[i], &nextNote);
        chord.notes.push_back(nextNote);
    }

    return chord;
}

PKChordStruct PKChordGetChordForNoteAndIntervals(PKNote base,
												 uint8_t *intervals,
												 uint32_t numIntervals)
{
    PKChordStruct chord;

    for(uint32_t i=0; i < numIntervals; i++)
    {
		if(intervals[i] == PK_INTERVAL_STOP) // STOP
		{
			break;
		}

        PKNote nextNote;
        PKNoteOffset(base, intervals[i], &nextNote);
        chord.notes.push_back(nextNote);
    }

    return chord;
}

PKChordDescription PKChordGetChordDescriptionMatchingIntervals(uint8_t *intervals,
															   uint32_t numIntervals)
{
	PKChordDescription null;

	// Get all available descriptions
	std::vector <PKChordDescription> all_descs = PKChordGetAllChordsDescriptions();

	for(uint32_t i=0; i<all_descs.size(); i++)
	{
		bool all_match = true;

		// See if sizes match

		int real_num_intervals = 0;
		for(uint32_t n=0; n<numIntervals; n++)
		{
			if(intervals[n] == PK_INTERVAL_STOP) // STOP
			{
				break;
			}

			real_num_intervals++;
		}

		if(real_num_intervals != all_descs[i].intervals.size())
		{
			// Not the same number of notes
			continue;
		}

		// See if they match
		uint32_t size = pk_min(numIntervals, all_descs[i].intervals.size());

		for(uint32_t n=0; n<size; n++)
		{
			if(intervals[n] == PK_INTERVAL_STOP) // STOP
			{
				break;
			}

			if(intervals[n] != all_descs[i].intervals[n])
			{
				all_match = false;
				break;
			}
		}

		if(all_match)
		{
			// Found it
			return all_descs[i];
		}
	}

	return null;
}

bool PKChordMatchesDescription(PKChordStruct chord,
                               PKChordDescription desc)
{
    if(chord.notes.size() != desc.intervals.size())
    {
        return false;
    }

    for(uint32_t i=0; i < chord.notes.size(); i++)
    {
        if(PKNoteGetInterval(chord.notes[0], chord.notes[i]) != desc.intervals[i])
        {
            return false;
        }
    }

    return true;
}

std::wstring PKChordGetShortName(PKChordStruct chord,
                                 PKNotationName notation,
                                 bool preferSharp)
{
    std::wstring result;

    if(chord.notes.size() < 2)
    {
        return PK_i18n(L"Not a chord!");
    }

    PKNote base = chord.notes[0];
  
    result  = PKNoteGetName(base, notation, preferSharp);
    result += L" ";

    std::vector <PKChordDescription> descs = PKChordGetAllChordsDescriptions();

    for(uint32_t i=0; i < descs.size(); i++)
    {
        if(PKChordMatchesDescription(chord, descs[i]))
        {
            result += descs[i].shortName;
            return result;
        }
    }

    return PK_i18n(L"Unknown");
}

std::wstring PKChordGetLongName(PKChordStruct chord,
                                PKNotationName notation,
                                bool preferSharp)
{
    std::wstring result;

    if(chord.notes.size() < 2)
    {
        return PK_i18n(L"Not a chord!");
    }

    PKNote base = chord.notes[0];
  
    result  = PKNoteGetName(base, notation, preferSharp);
    result += L" ";

    std::vector <PKChordDescription> descs = PKChordGetAllChordsDescriptions();

    for(uint32_t i=0; i < descs.size(); i++)
    {
        if(PKChordMatchesDescription(chord, descs[i]))
        {
            result += descs[i].longName;
            return result;
        }
    }

    return PK_i18n(L"Unknown");
}

