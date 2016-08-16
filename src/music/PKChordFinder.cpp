//
//
//

#include "PKChordFinder.h"
#include "PKGuitarNeck.h"

//
//
//

PKChordFinder::PKChordFinder()
{
	this->neck = new PKGuitarNeck();

	if(this->neck)
	{
		PKTuning *std = PKTuning::getStandardGuitarTuning();
		this->neck->applyTuning(std);
		delete std;
	}

	this->allowOpenStrings = false;

    this->flags[PKCF_ALLOW_OPEN_BASS_STRING]   = false;
	this->flags[PKCF_TRY_TO_BAR_CHORD]         = false;
	this->flags[PKCF_ALLOW_OPEN_STRINGS]       = false;
}

PKChordFinder::~PKChordFinder()
{

}

void PKChordFinder::applyTuning(PKTuning *tuning)
{
	this->neck->applyTuning(tuning);
}

void PKChordFinder::setAllowOpenStrings(bool s)
{
	this->allowOpenStrings = s;
}

void PKChordFinder::findChords(PKNote rootNote,
							   PKNote bassNote,
							   PKChordDescription chordType,
							   int32_t startFret,
							   int32_t endFret,
							   std::vector<PKChordDiagram*> *chords)
{
	// Toggle flags and call our algo in a loop

	for(uint32_t i=0; i<PKCF_NUMBER_OF_FLAGS; i++)
	{
		this->flags[i] = false;
	}

	if(this->allowOpenStrings)
	{
		for(uint32_t i0=0; i0<2; i0++)
		{
			for(uint32_t i1=0; i1<2; i1++)
			{
				for(uint32_t i2=0; i2<2; i2++)
				{
					this->findChordsInternal(rootNote,
											 bassNote,
											 chordType,
											 startFret,
											 endFret,
											 chords);
			
					this->flags[2] = ((i2 % 2) == 0);
				}

				this->flags[1] = ((i1 % 2) == 0);
			}

			this->flags[0] = ((i0 % 2) == 0);
		}
	}
	else
	{
		for(uint32_t i0=0; i0<2; i0++)
		{
			for(uint32_t i1=0; i1<2; i1++)
			{
				this->findChordsInternal(rootNote,
										 bassNote,
										 chordType,
										 startFret,
										 endFret,
										 chords);
			
				this->flags[1] = ((i1 % 2) == 0);
			}

			this->flags[0] = ((i0 % 2) == 0);
		}
	}
}

//
// Internal
// 

void PKChordFinder::findChordsInternal(PKNote rootNote,
									   PKNote bassNote,
									   PKChordDescription chordType,
									   int32_t startFret,
									   int32_t endFret,
									   std::vector<PKChordDiagram*> *chords)
{
	bool xUnusedBassStrings = true;
	
	if(this->neck == NULL)
	{
		return;
	}

	if(chords == NULL)
	{
		return;
	}

	// Create the chord to get the actual notes

	PKChordStruct chord = PKChordGetChordForNoteAndDescription(rootNote,
															   chordType);

	uint32_t numNotes   = chord.notes.size();
	uint32_t numStrings = this->neck->getNumberOfStrings();

	uint32_t bassString = 0;
	int32_t bassFret    = -1;

	// Look for the bass in the first half of the strings

	for(uint32_t i=0; i < numStrings/2; i++)
	{
		PKString *string = this->neck->getStringAt(i);

		if(string)
		{
			if(this->flags[PKCF_ALLOW_OPEN_BASS_STRING])
			{
				if(string->getNoteAtFret(0).note == bassNote.note)
				{
					// Found it
					bassString = i;
					bassFret   = 0;
					break;
				}
			}

			if(string->findFirstNoteOnStringStartingAtFret(bassNote,
														   startFret,
														   &bassFret))
			{
				if(bassFret <= endFret && bassFret != -1)
				{
					bassString = i;
					break;
				}
				else
				{
					bassFret = -1;
				}
			}
		}
	}

	if(bassFret == -1)
	{
		return;
	}

	// Create an array to hold the 
	// possible frets for each string

	std::vector<std::vector<int32_t> *> possible_frets_for_string;

	for(uint32_t s=0; s<numStrings; s++)
	{
		std::vector<int32_t> *new_v = new std::vector<int32_t>;
		possible_frets_for_string.push_back(new_v);
	}

	// Add the bass note
	possible_frets_for_string[bassString]->push_back(bassFret);

	// Start looking for other notes
	if(xUnusedBassStrings)
	{
		// X the previous strings

		for(uint32_t i=0; i<bassString; i++)
		{
			possible_frets_for_string[i]->push_back(-1);
		}
	}
	else
	{
		// Find notes above bass string

		for(int32_t i=(int32_t)bassString-1; i>=0; i--)
		{
			for(uint32_t n=0; n<chord.notes.size(); n++)
			{
				int32_t outFret = 255;

				this->neck->getStringAt(i)->findFirstNoteOnStringStartingAtFret(chord.notes[n],
																				startFret, &outFret);

				if((outFret >= startFret) && (outFret <= endFret))
				{
					possible_frets_for_string[i]->push_back(outFret);
				}
			}

			if(possible_frets_for_string[i]->size() == 0)
			{
				possible_frets_for_string[i]->push_back(-1);
			}
		}
	}

	// Now find the remaining notes

	for(uint32_t i=bassString+1; i<numStrings; i++)
	{
		for(uint32_t n=0; n<chord.notes.size(); n++)
		{
			// See if an open string matches

			if(this->flags[PKCF_ALLOW_OPEN_STRINGS])
			{
				if(this->neck->getStringAt(i)->getNoteAtFret(0).note == chord.notes[n].note)
				{
					possible_frets_for_string[i]->push_back(0);
				}
			}

			// Only search for notes that are part of
			// the chord, and different from the last one
			
			int32_t outFret = 255;

			this->neck->getStringAt(i)->findFirstNoteOnStringStartingAtFret(chord.notes[n],
																			startFret, &outFret);

			if((outFret >= startFret) && (outFret <= endFret))
			{
				possible_frets_for_string[i]->push_back(outFret);
			}
		}

		if(possible_frets_for_string[i]->size() == 0)
		{
			possible_frets_for_string[i]->push_back(-1);
		}
	}

	// Now create diagrams for each fret combination

	std::vector<PKChordDiagram *> diagrams;

	PKChordDiagram *diag = new PKChordDiagram(chord);
	diag->setNumberOfStrings(numStrings);
	
	if(startFret != 0)
	{
		diag->setNumberOfFrets(endFret - startFret + 1);
	}
	else
	{
		diag->setNumberOfFrets(endFret - startFret);
	}
	diag->setStartingFret(startFret);

	// Add the first diagram to the list
	diagrams.push_back(diag);

	for(uint32_t s=0; s<numStrings; s++)
	{
		// Duplicate those diagrams if needed

		uint32_t dsize = diagrams.size();
		uint32_t nsize = possible_frets_for_string[s]->size();

		if(nsize > 1)
		{
			for(uint32_t k=0; k<(nsize-1); k++)
			{
				for(uint32_t d=0; d<dsize; d++)
				{
					PKChordDiagram *new_d = new PKChordDiagram(diagrams[d]);
					diagrams.push_back(new_d);
				}
			}
		}

		// Now assign the notes from our possible frets
		// to each diagram that we have in our array...

		for(uint32_t k=0; k<nsize; k++)
		{
			for(uint32_t d=0; d<dsize; d++)
			{
				diagrams[k*dsize + d]->addNoteWithFretOnString((*possible_frets_for_string[s])[k], s);
			}
		}
	}

	// Loop over diagrams and create bars
	// then eliminate those that are wrong

	uint32_t numDiagrams = diagrams.size();

	for(uint32_t nd=0; nd<numDiagrams; nd++)
	{
		// Finally, let's try to create a bar if it is possible
		// For that, find the minimum fret

		if(this->flags[PKCF_TRY_TO_BAR_CHORD])
		{
			int32_t minimumFret = 255;

			for(uint32_t n=0; n<diagrams[nd]->getNumberOfNotes(); n++)
			{
				int32_t fret = diagrams[nd]->getFretForNoteAt(n);

				if(fret != -1 && fret != 0)
				{
					if(fret < minimumFret)
					{
						minimumFret = fret;
					}
				}
			}

			// Now see how many notes are at this minimum

			std::vector<uint32_t> bar_strings;

			for(uint32_t n=0; n<diagrams[nd]->getNumberOfNotes(); n++)
			{
				int32_t fret = diagrams[nd]->getFretForNoteAt(n);

				if(fret == minimumFret)
				{
					bar_strings.push_back(diagrams[nd]->getStringForNoteAt(n));
				}
			}

			if(bar_strings.size() > 1 && minimumFret > 0)
			{
				// We can try to create a bar

				uint32_t minString = 255;
				uint32_t maxString = 0;

				for(uint32_t k=0; k<bar_strings.size(); k++)
				{
					if(bar_strings[k] < minString)
					{
						minString = bar_strings[k];
					}

					if(bar_strings[k] > maxString)
					{
						maxString = bar_strings[k];
					}
				}

				// Loop over the strings in between
				// the min and max bar strings and see
				// if the notes on those strings are
				// further down the neck 

				bool okToBar = true;

				for(uint32_t nn=minString+1; nn<maxString; nn++)
				{
					int32_t ff = diagrams[nd]->getFretForStringAt(nn);

					if(ff < minimumFret)
					{
						okToBar = false;
						break;
					}
				}

				if(okToBar)
				{
					// Remove notes

					for(uint32_t kk=0; kk<bar_strings.size(); kk++)
					{
						diagrams[nd]->removeNoteWithFretOnString(minimumFret, bar_strings[kk]);
					}

					// See if we can extend the bar

					uint32_t max = maxString;

					if(maxString < numStrings)
					{
						for(uint32_t m=maxString+1; m<=numStrings; m++)
						{
							int32_t ff = diagrams[nd]->getFretForStringAt(m);

							if(ff >= minimumFret)
							{
								max = m;
							}
							else
							{
								break;
							}
						}
					}

					// Add bar

					PKChordBar bar;

					bar.startString = minString;
					bar.endString   = max;
					bar.fret        = minimumFret;

					diagrams[nd]->addBar(bar);
				}
			}
		}

		// Now eliminate the chord if
		// it can't be fingered

		uint32_t numOfBars  = diagrams[nd]->getNumberOfBars();
		uint32_t maxNotes   = diagrams[nd]->getNumberOfNotes();
		uint32_t numOfNotes = 0;
		uint32_t numOfXs    = 0;

		for(uint32_t m=0; m<maxNotes; m++)
		{
			int32_t  f = diagrams[nd]->getFretForNoteAt(m);
			uint32_t s = diagrams[nd]->getStringForNoteAt(m);

			if(f > 0)
			{
				numOfNotes++;
			}

			if((f == -1) && (s > bassString))
			{
				numOfXs++;
			}
		}

		// Filter chords that have too many notes 
		// or too many Xs

		bool okToAdd = true;

		if((numOfNotes > 4) || ((numOfBars > 0) && (numOfNotes > 3)))
		{
			okToAdd = false;
		}

		if(numOfXs > 0)
		{
			okToAdd = false;
		}

		// Filter chords with too few notes

		if(numOfNotes == 0 && maxNotes < numStrings)
		{
			okToAdd = false;
		}

		// Check that the diagram has
		// all the notes from the chord

		for(uint32_t n=0; n<chord.notes.size(); n++)
		{
			if(PKNoteGetInterval(chord.notes[0], 
								 chord.notes[n]) == PK_INTERVAL_5TH)
			{
				// We make the 5th optional
				continue;
			}

			if((PKNoteGetInterval(chord.notes[0], 
								 chord.notes[n]) == PK_INTERVAL_11TH) &&
								 chord.notes.size() > 6)
			{
				// We make the 11th optional, for 13th chords
				continue;
			}


			bool found = false;

			for(uint32_t s=0; s<diagrams[nd]->getNumberOfStrings(); s++)
			{
				int32_t maxFret = -1;

				// Find the farthest note on each string
				for(uint32_t nn=0; nn<diagrams[nd]->getNumberOfNotes(); nn++)
				{
					if(diagrams[nd]->getStringForNoteAt(nn) == s)
					{
						int32_t fret = diagrams[nd]->getFretForNoteAt(nn);

						if(fret > maxFret)
						{
							maxFret = fret;
						}
					}
				}

				// Check if a bar is not further
				for(uint32_t bb=0; bb<diagrams[nd]->getNumberOfBars(); bb++)
				{
					PKChordBar bar = diagrams[nd]->getBarAt(bb);

					if((bar.startString <= s) && (s <= bar.endString))
					{
						int32_t fret = bar.fret;

						if(fret > maxFret)
						{
							maxFret = fret;
						}
					}
				}

				PKNote note = this->neck->getStringAt(s)->getNoteAtFret(maxFret);

				if(note.note == chord.notes[n].note)
				{
					found = true;
				}
			}

			if(!found)
			{
				okToAdd = false;
				break;
			}
		}

		// All done

		if(okToAdd)
		{
			chords->push_back(diagrams[nd]);
		}
		else
		{
			delete diagrams[nd];
		}
	}

	for(uint32_t s=0; s<numStrings; s++)
	{
		delete possible_frets_for_string[s];
	}
}

//
// Eliminate duplicates
//

std::vector<PKChordDiagram*> PKChordFinder::trimChords(std::vector<PKChordDiagram*> chords)
{
	std::vector<PKChordDiagram*> result = chords;

	uint32_t i=0;

	while(i<result.size())
	{
		PKChordDiagram *d = result[i];
		std::vector<uint32_t> indexes;
		indexes.push_back(i);

		for(uint32_t j=i+1; j<result.size(); j++)
		{
			PKChordDiagram *dd = result[j];

			if(PKChordDiagramEquivalent(d, dd))
			{
				indexes.push_back(j);
			}
		}

		bool trimmed = false;

		if(indexes.size() > 1)
		{
			trimmed = true;

			// We have doublons for chord @ index i
			// so we need to remove all but one

			uint32_t indexToKeep = i;
			if(d->getNumberOfBars() > 0) // If it's a bar chord, keep the last one
			{                            // as it will be the one with the bar on fret #1
				indexToKeep = indexes[indexes.size()-1];
			}

			std::vector<PKChordDiagram*> new_result;

			for(uint32_t m=0; m<result.size(); m++)
			{
				bool removeIt = false;
	
				for(uint32_t n=0; n<indexes.size(); n++)
				{
					if((m == indexes[n]) && (indexes[n] != indexToKeep))
				    {
						removeIt = true;
						break;
				    }
				}

				if(removeIt)
				{
					delete result[m];
				}
				else
				{
					new_result.push_back(result[m]);
				}
			}

			result = new_result;
		}

		if(!trimmed)
		{
			i++;
		}
	}

	return result;
}
