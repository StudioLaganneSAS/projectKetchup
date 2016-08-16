//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CHORD_H
#define PK_CHORD_H

#include "PKNote.h"

//
// PKChordDescription
//

typedef struct _PKChordDescription
{
    std::wstring shortName;
    std::wstring longName;

    std::vector <int8_t> intervals;

} PKChordDescription;

//
// PKChordStruct
//

typedef struct _PKChordStruct
{
    std::vector<PKNote> notes;

} PKChordStruct;

//
// Helper Functions
//

// Descriptions Getters

PKChordDescription PKChordGetMajorChordDescription();
PKChordDescription PKChordGetMinorChordDescription();
PKChordDescription PKChordGetSus2ChordDescription();
PKChordDescription PKChordGetSus4ChordDescription();
PKChordDescription PKChordGetSeventhChordDescription();
PKChordDescription PKChordGetMajorSeventhChordDescription();
PKChordDescription PKChordGetMinorSeventhChordDescription();
PKChordDescription PKChordGetMinorMajorSeventhChordDescription();
PKChordDescription PKChordGetSixthChordDescription();
PKChordDescription PKChordGetMinorSixthChordDescription();
PKChordDescription PKChordGetAugmentedChordDescription();
PKChordDescription PKChordGetDiminishedChordDescription();
PKChordDescription PKChordGetDiminishedSeventhChordDescription();
PKChordDescription PKChordGetAddedNinthChordDescription();
PKChordDescription PKChordGetMinorAddedNinthChordDescription();
PKChordDescription PKChordGetNinthChordDescription();
PKChordDescription PKChordGetMajorNinthChordDescription();
PKChordDescription PKChordGetMinorNinthChordDescription();
PKChordDescription PKChordGetMinorMajorNinthChordDescription();
PKChordDescription PKChordGetEleventhChordDescription();
PKChordDescription PKChordGetMinorEleventhChordDescription();
PKChordDescription PKChordGetSixNineChordDescription();
PKChordDescription PKChordGetMinorSixNineChordDescription();
PKChordDescription PKChordGetThirteenthChordDescription();
PKChordDescription PKChordGetMinorThirteenthChordDescription();
PKChordDescription PKChordGetSeventhSus2ChordDescription();
PKChordDescription PKChordGetSeventhSus4ChordDescription();

std::vector <PKChordDescription> PKChordGetAllChordsDescriptions();

// Chords Helpers

PKChordStruct PKChordGetChordForNoteAndDescription(PKNote base, 
                                                   PKChordDescription desc);

PKChordStruct PKChordGetChordForNoteAndIntervals(PKNote base,
												 uint8_t *intervals,
												 uint32_t numIntervals);

PKChordDescription PKChordGetChordDescriptionMatchingIntervals(uint8_t *intervals,
															   uint32_t numIntervals);

bool PKChordMatchesDescription(PKChordStruct chord,
                               PKChordDescription desc);

void PKChordReplaceInterval(PKChordDescription *chord,
							int8_t intervalToRepace, 
							int8_t replacement);

std::wstring PKChordGetShortName(PKChordStruct  chord,
                                 PKNotationName notation,
                                 bool preferSharp = true);

std::wstring PKChordGetLongName(PKChordStruct  chord,
                                PKNotationName notation,
                                bool preferSharp = true);

#endif // PK_CHORD_H
