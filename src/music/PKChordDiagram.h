//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CHORD_DIAGRAM_H
#define PK_CHORD_DIAGRAM_H

#include "PKChord.h"

//
// PKChordBar
//

typedef struct _PKChordBar
{
    uint8_t  startString;
    uint8_t  endString;
    uint8_t  fret;

} PKChordBar;

//
// PKChordDiagram
//

class PKChordDiagram
{
public:
    
    PKChordDiagram();
    PKChordDiagram(PKChordStruct chord);
    PKChordDiagram(PKChordDiagram *copy);
    ~PKChordDiagram();

    uint8_t getNumberOfStrings();
    void    setNumberOfStrings(uint8_t num);

    uint8_t getNumberOfFrets();
    void    setNumberOfFrets(uint8_t num);

    uint8_t getStartingFret();
    void    setStartingFret(uint8_t start);

    void addNoteWithFretOnString(int8_t fret, uint8_t string);
    void removeNoteWithFretOnString(int8_t fret, uint8_t string);

    uint32_t getNumberOfNotes();
    int8_t   getFretForNoteAt(uint32_t index);
    uint8_t  getStringForNoteAt(uint32_t index);

    int8_t   getFretForStringAt(uint32_t index);

	void addBar(PKChordBar bar);

    uint32_t   getNumberOfBars();
    PKChordBar getBarAt(uint32_t index);

	void *createBinaryRepresentationForDiagram(uint32_t *size);
	
	uint32_t getBinaryRepresentationVersion(void *data, uint32_t size);
	bool	 loadFromBinaryRepresentationV1(void *data, uint32_t size);

private:

    // As a indicator of 
    // what this chord is

    PKChordStruct chord;

    // Members used to define
    // the complete diagram

    uint8_t numberOfStrings;
    uint8_t numberOfFrets;

    uint8_t startingFret;

    std::vector<PKChordBar> bars;
    std::vector<uint8_t>    strings;
    std::vector<int8_t>     notes;
};

// Util

bool PKChordDiagramEquivalent(PKChordDiagram *d1, PKChordDiagram *d2);

#endif // PK_CHORD_DIAGRAM_H
