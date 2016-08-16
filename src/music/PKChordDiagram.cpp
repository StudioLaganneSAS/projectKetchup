//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKChordDiagram.h"

//
// PKChordDiagram
//

PKChordDiagram::PKChordDiagram()
{
}

PKChordDiagram::PKChordDiagram(PKChordStruct chord)
{
    // Record our name
    this->chord = chord;
}

PKChordDiagram::PKChordDiagram(PKChordDiagram *copy)
{
	if(copy == NULL)
	{
		return;
	}

	this->chord = copy->chord;

	this->numberOfStrings = copy->numberOfStrings;
	this->numberOfFrets   = copy->numberOfFrets;

	this->startingFret = copy->startingFret;

	for(uint32_t i=0; i<copy->notes.size(); i++)
	{
		this->notes.push_back(copy->notes[i]);
	}

	for(uint32_t i=0; i<copy->strings.size(); i++)
	{
		this->strings.push_back(copy->strings[i]);
	}

	for(uint32_t i=0; i<copy->bars.size(); i++)
	{
		this->bars.push_back(copy->bars[i]);
	}
}

PKChordDiagram::~PKChordDiagram()
{
}

uint8_t PKChordDiagram::getNumberOfStrings()
{
    return this->numberOfStrings;
}

void PKChordDiagram::setNumberOfStrings(uint8_t num)
{
    this->numberOfStrings = num;
}

uint8_t PKChordDiagram::getNumberOfFrets()
{
    return this->numberOfFrets;
}

void PKChordDiagram::setNumberOfFrets(uint8_t num)
{
    this->numberOfFrets = num;
}

uint8_t PKChordDiagram::getStartingFret()
{
    return this->startingFret;
}

void PKChordDiagram::setStartingFret(uint8_t start)
{
    this->startingFret = start;
}

void PKChordDiagram::addNoteWithFretOnString(int8_t fret, uint8_t string)
{
    this->notes.push_back(fret);
    this->strings.push_back(string);
}

void PKChordDiagram::removeNoteWithFretOnString(int8_t fret, uint8_t string)
{
    std::vector<uint8_t> new_strings;
    std::vector<int8_t>  new_notes;

	for(uint32_t i=0; i<this->notes.size(); i++)
	{
		if(this->notes[i] != fret ||
		   this->strings[i] != string)
		{
			new_notes.push_back(this->notes[i]);
			new_strings.push_back(this->strings[i]);
		}
	}

	this->notes = new_notes;
	this->strings = new_strings;
}

uint32_t PKChordDiagram::getNumberOfNotes()
{
    return this->notes.size();
}

int8_t PKChordDiagram::getFretForNoteAt(uint32_t index)
{
    if(index >= this->notes.size())
    {
        return -2;
    }

    return this->notes[index];
}

int8_t PKChordDiagram::getFretForStringAt(uint32_t index)
{
	for(uint32_t i=0; i<this->notes.size(); i++)
	{
		if(this->strings[i] == index)
		{
			return this->notes[i];
		}
	}

	return -1;
}

uint8_t PKChordDiagram::getStringForNoteAt(uint32_t index)
{
    if(index >= this->strings.size())
    {
        return -1;
    }

    return this->strings[index];
}

void PKChordDiagram::addBar(PKChordBar bar)
{
    this->bars.push_back(bar);
}

uint32_t PKChordDiagram::getNumberOfBars()
{
    return this->bars.size();
}

PKChordBar PKChordDiagram::getBarAt(uint32_t index)
{
    if(index >= this->notes.size())
    {
        PKChordBar bar;
        memset(&bar, 0, sizeof(bar));
        return bar;
    }

    return this->bars[index];
}

void *PKChordDiagram::createBinaryRepresentationForDiagram(uint32_t *size)
{
	if(size == NULL)
	{
		return NULL;
	}

	*size = 0;

	// Compute the needed size

	uint32_t requiredSize = sizeof(uint32_t)                  + // version
							sizeof(uint8_t)                   + // this->chord.notes.size()
							sizeof(PKNote)*chord.notes.size() + // chord notes
							sizeof(uint8_t)                   + // numberOfStrings
							sizeof(uint8_t)                   + // numberOfFrets
							sizeof(uint8_t)                   + // startingFret
							sizeof(uint8_t)                   + // bars.size()
							bars.size()*sizeof(PKChordBar)    + // bars
							sizeof(uint8_t )                  + // strings.size()
							strings.size()*sizeof(uint8_t)    + // strings
							strings.size()*sizeof(uint8_t);     // notes

	uint8_t *buffer = new uint8_t[requiredSize];

	if(buffer == NULL)
	{
		return NULL;
	}

	memset(buffer, 0, requiredSize);

	// Now fill the buffer

	int32_t offset = 0;

	*((uint32_t *)&(buffer[offset])) = 1; // v1
	offset += sizeof(uint32_t);

	buffer[offset] = this->chord.notes.size();
	offset += 1;

	for(uint8_t i=0; i<this->chord.notes.size(); i++)
	{
		memcpy(&buffer[offset], &(this->chord.notes[i]), sizeof(PKNote));
		offset += sizeof(PKNote);
	}

	buffer[offset] = this->numberOfStrings;
	offset += 1;

	buffer[offset] = this->numberOfFrets;
	offset += 1;

	buffer[offset] = this->startingFret;
	offset += 1;

	uint8_t numBars = pk_min(255, this->bars.size());

	buffer[offset] = numBars;
	offset += 1;

	for(uint8_t i=0; i<numBars; i++)
	{
		memcpy(&(buffer[offset]), &(this->bars[i]), sizeof(PKChordBar));
		offset += sizeof(PKChordBar);
	}

	uint8_t numNotes = pk_min(255, this->strings.size());

	buffer[offset] = numNotes;
	offset += 1;

	for(uint8_t i=0; i<numNotes; i++)
	{
		memcpy(&(buffer[offset]), &(this->strings[i]), sizeof(uint8_t));
		offset += 1;
		memcpy(&(buffer[offset]), &(this->notes[i]), sizeof(uint8_t));
		offset += 1;
	}

	*size = requiredSize;
	return buffer;
}

uint32_t PKChordDiagram::getBinaryRepresentationVersion(void *data, uint32_t size)
{
	if(size < 4 || data == NULL)
	{
		return 0;
	}

	uint32_t v = ((uint32_t *) data)[0];
	return v;
}

bool PKChordDiagram::loadFromBinaryRepresentationV1(void *data, uint32_t size)
{
	if(data == NULL)
	{
		return false;
	}

	uint32_t requiredSize = sizeof(uint32_t)                  + // version
							sizeof(uint8_t)                   + // this->chord.notes.size()
							sizeof(uint8_t)                   + // numberOfStrings
							sizeof(uint8_t)                   + // numberOfFrets
							sizeof(uint8_t)                   + // startingFret
							sizeof(uint8_t)                   + // bars.size()
							sizeof(uint8_t);                    // strings.size()

	if(size < requiredSize) // sanity check
	{
		return false;
	}

	// Clear us up first

	this->bars.clear();
	this->notes.clear();
	this->strings.clear();

	// Now load

	uint8_t *buffer = (uint8_t *) data;
	int32_t offset  = 0;

	if(*((uint32_t *)&(buffer[offset])) != 1) // v1
	{
		return false;
	}

	offset += sizeof(uint32_t);

	uint8_t numChordNotes = buffer[offset];
	offset += 1;

	for(uint8_t i=0; i<numChordNotes; i++)
	{
		if(offset >= size)
		{
			return false;
		}

		PKNote note;
		memcpy(&note, &(buffer[offset]), sizeof(PKNote));
		this->chord.notes.push_back(note);
		offset += sizeof(PKNote);
	}

	if(offset >= size)
	{
		return false;
	}

	this->numberOfStrings = buffer[offset];
	offset += 1;

	if(offset >= size)
	{
		return false;
	}

	this->numberOfFrets = buffer[offset];
	offset += 1;

	if(offset >= size)
	{
		return false;
	}

	this->startingFret = buffer[offset];
	offset += 1;

	if(offset >= size)
	{
		return false;
	}

	uint8_t numBars = buffer[offset];
	offset += 1;

	for(uint8_t i=0; i<numBars; i++)
	{
		if(offset >= size)
		{
			return false;
		}

		PKChordBar bar;
		memcpy(&bar, &(buffer[offset]), sizeof(PKChordBar));
		offset += sizeof(PKChordBar);
		this->bars.push_back(bar);
	}

	if(offset >= size)
	{
		return false;
	}

	uint8_t numNotes = buffer[offset];
	offset += 1;

	for(uint8_t i=0; i<numNotes; i++)
	{
		if(offset >= (size -1))
		{
			return false;
		}

		uint8_t string;
		uint8_t fret;

		memcpy(&string, &(buffer[offset]), sizeof(uint8_t));
		offset += 1;
		this->strings.push_back(string);

		memcpy(&fret, &(buffer[offset]), sizeof(uint8_t));
		offset += 1;
		this->notes.push_back(fret);
	}

	return true;
}

//
// Util
//

bool PKChordDiagramEquivalent(PKChordDiagram *d1, PKChordDiagram *d2)
{
	if(d1 == NULL || d2 == NULL)
	{
		return false;
	}

	// Check basics

	if(d1->getNumberOfStrings() != d2->getNumberOfStrings())
	{
		return false;
	}

	if(d1->getNumberOfFrets() != d2->getNumberOfFrets())
	{
		return false;
	}

	if(d1->getNumberOfNotes() != d2->getNumberOfNotes())
	{
		return false;
	}

	if(d1->getNumberOfBars() != d2->getNumberOfBars())
	{
		return false;
	}

	// Check that all from d1 is found in d2

	for(uint32_t i=0; i<d1->getNumberOfNotes(); i++)
	{
		uint32_t s1 = d1->getStringForNoteAt(i);
		int32_t  f1 = d1->getFretForNoteAt(i);

		// Try to find it in d2
		bool found = false;

		for(uint32_t j=0; j<d2->getNumberOfNotes(); j++)
		{
			uint32_t s2 = d2->getStringForNoteAt(i);
			int32_t  f2 = d2->getFretForNoteAt(i);
		
			if(s1 == s2 && f1 == f2)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			return false;
		}
	}

	for(uint32_t i=0; i<d1->getNumberOfBars(); i++)
	{
		PKChordBar bar1 = d1->getBarAt(i);

		// Try to find it in d2
		bool found = false;

		for(uint32_t j=0; j<d2->getNumberOfNotes(); j++)
		{
			PKChordBar bar2 = d2->getBarAt(i);
		
			if(bar1.fret == bar2.fret && 
			   bar1.startString == bar2.startString &&
			   bar1.endString == bar2.endString)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			return false;
		}
	}

	// Check that all from d2 is found in d1

	for(uint32_t i=0; i<d2->getNumberOfNotes(); i++)
	{
		uint32_t s2 = d2->getStringForNoteAt(i);
		int32_t  f2 = d2->getFretForNoteAt(i);

		// Try to find it in d2
		bool found = false;

		for(uint32_t j=0; j<d1->getNumberOfNotes(); j++)
		{
			uint32_t s1 = d1->getStringForNoteAt(i);
			int32_t  f1 = d1->getFretForNoteAt(i);
		
			if(s1 == s2 && f1 == f2)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			return false;
		}
	}

	for(uint32_t i=0; i<d2->getNumberOfBars(); i++)
	{
		PKChordBar bar2 = d2->getBarAt(i);

		// Try to find it in d2
		bool found = false;

		for(uint32_t j=0; j<d1->getNumberOfNotes(); j++)
		{
			PKChordBar bar1 = d1->getBarAt(i);
		
			if(bar1.fret == bar2.fret && 
			   bar1.startString == bar2.startString &&
			   bar1.endString == bar2.endString)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			return false;
		}
	}

	// All passed, diagrams are equivalent

	return true;
}

