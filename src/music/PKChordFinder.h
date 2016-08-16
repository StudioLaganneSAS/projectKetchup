//
//
//

#ifndef PK_CHORD_FINDER_H
#define PK_CHORD_FINDER_H

#include "PKChordDiagram.h"
#include "PKChord.h"
#include "PKNeck.h"
#include "PKTuning.h"

//
// Flags for the algo
//

#define PKCF_NUMBER_OF_FLAGS 3

#define PKCF_ALLOW_OPEN_BASS_STRING   0
#define PKCF_TRY_TO_BAR_CHORD         1
#define PKCF_ALLOW_OPEN_STRINGS		  2

//
// PKChordFinder
//

class PKChordFinder {

public:

    PKChordFinder();
	~PKChordFinder();
	
	// Setup

	void applyTuning(PKTuning *tuning);

	void setAllowOpenStrings(bool s);

	// Main method

	void findChords(PKNote rootNote,
					PKNote bassNote,
					PKChordDescription chordType,
					int32_t startFret,
					int32_t endFret,
					std::vector<PKChordDiagram*> *chords);

	// Eliminate duplicates from array

	std::vector<PKChordDiagram*> trimChords(std::vector<PKChordDiagram*> chords);

private:

	void findChordsInternal(PKNote rootNote,
							PKNote bassNote,
							PKChordDescription chordType,
							int32_t startFret,
							int32_t endFret,
							std::vector<PKChordDiagram*> *chords);

	PKNeck *neck;
	bool    flags[PKCF_NUMBER_OF_FLAGS];
	bool    allowOpenStrings;
};

#endif // PK_CHORD_FINDER_H