//
//
//

#include "PKChordLibraryDialog.h"
#include "PKChordLibraryDialog_XML.h"
#include "PKChordDiagramVectorUI.h"
#include "PKStr.h"
#include "PKStdDialog.h"

//
// PKChordLibraryDialog
//

PKChordLibraryDialog::PKChordLibraryDialog(PKWindowHandle parent,
					 		               std::wstring   icon)
: PKModalDialog(L"PK_CHORD_LIBRARY_DIALOG",
                PK_WINDOW_STYLE_DEFAULT | 
                PK_WINDOW_STYLE_NO_RESIZE |
                PK_WINDOW_STYLE_NO_MAX,
                parent, icon)
{
    this->parentW = parent;

	this->chordGrid  = NULL;
	this->sharp      = NULL;
	this->flat       = NULL;
	this->chordTypes = NULL;
	this->scrollView = NULL;
	this->vectorRoot = NULL;

    this->chordType = PKChordGetMajorChordDescription();

	this->notation = PKNOTATION_LETTERS;
	this->tunings  = PKTuning::getAllGuitarTunings();

	// Setup

	this->rootNote.note = -1;
	this->bassNote.note = PKNOTE_C;
	this->onlyBassChange = false;
	this->currentDiagram = NULL;
	this->diagramRoot = NULL;
	this->inited = false;
	
	// Build the dialog

	this->factory = new PKMusicUIFactory();
	this->addFactory(this->factory);

	PKVariantWStringI18N caption(L"PKI18N:Chord Library");
	this->set(PKWindow::CAPTION_WSTRING_PROPERTY, &caption);

    this->addFactory(factory);
	this->loadFromXML(xml);
    this->initUIElements();

	// Update the display

	this->updateChords();
    this->skipComboChanges = false;
}

PKChordLibraryDialog::~PKChordLibraryDialog()
{
	delete this->factory;

	for(uint32_t i=0; i<this->tunings.size(); i++)
	{
		delete this->tunings[i];
	}
}

void PKChordLibraryDialog::initUIElements()
{
	// Get the grid layout for chord diagrams
	chordGrid = dynamic_cast<PKGridLayout*>(this->findControl("ID_CHORD_GRID"));
	scrollView = dynamic_cast<PKScrollView*>(this->findControl("ID_SCROLL_VIEW"));
	vectorRoot = dynamic_cast<PKVectorControlRoot*>(this->findControl("ID_VECTOR_ROOT"));
	diagramRoot = dynamic_cast<PKVectorControlRoot*>(this->findControl("CHORD_DIAGRAM_ROOT"));

	// Get the note buttons
    buttons.push_back(dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_NOTE_C")));
    buttons.push_back(dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_NOTE_D")));
    buttons.push_back(dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_NOTE_E")));
    buttons.push_back(dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_NOTE_F")));
    buttons.push_back(dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_NOTE_G")));
    buttons.push_back(dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_NOTE_A")));
    buttons.push_back(dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_NOTE_B")));

    // Select the C note
    PKVariantBool toggled(true);
    buttons[0]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled);

	// Get the other buttons

    sharp = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_SHARP"));
    flat  = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_FLAT"));

    fivePlus  = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_5PLUS"));;
    fiveMinus = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_5MINUS"));;
    ninePlus  = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_9PLUS"));;
    nineMinus = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_9MINUS"));;
    thirteenPlus  = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_13PLUS"));;
    thirteenMinus = dynamic_cast<PKStretchToggleButton*>(this->findControl("ID_13MINUS"));;

    chordTypes = dynamic_cast<PKListBox*>(this->findControl("ID_CHORD_TYPES"));

    if(chordTypes)
    {
        std::vector <PKChordDescription> chords = PKChordGetAllChordsDescriptions();

        for(uint32_t i=0; i<chords.size(); i++)
        {
			std::wstring title = chords[i].longName + L" (" + chords[i].shortName + L")";
			
			if(chords[i].shortName == L"")
			{
				title = chords[i].longName;
			}
			
            chordTypes->addItem(title);
        }

        chordTypes->setCurrentSelection(0);
    }

	// Add tunings

	this->clearComboBoxItems("ID_TUNING");

	for(uint32_t i=0; i<this->tunings.size(); i++)
	{
		std::wstring tName = this->tunings[i]->getName();

		tName += L" (";

		for(uint32_t n=0; n<this->tunings[i]->getNumberOfStrings(); n++)
		{
			tName += PKNoteGetName(this->tunings[i]->getNoteForString(n),
								   this->notation, true);

            if((this->notation == PKNOTATION_NAMES) && 
               (n != (this->tunings[i]->getNumberOfStrings() - 1)))
            {
                tName += L" ";
            }
		}

		tName += L")";

		this->addComboBoxItem("ID_TUNING", tName);
	}

	this->selectComboBoxItem("ID_TUNING", 0);

	this->inited = true;
}

void PKChordLibraryDialog::selectTuningAtIndex(int index)
{
  	this->selectComboBoxItem("ID_TUNING", index);
}

void PKChordLibraryDialog::setNotation(PKNotationName n)
{
    this->notation = n;

    // Update the note buttons

    for(uint32_t i=0; i<this->buttons.size(); i++)
    {
        PKNote note;
        note.octave = 0;
        std::wstring text = L"";

        switch(i)
        {
        case 0:
            note.note = PKNOTE_DO;
            break;
        case 1:
            note.note = PKNOTE_RE;
            break;
        case 2:
            note.note = PKNOTE_MI;
            break;
        case 3:
            note.note = PKNOTE_FA;
            break;
        case 4:
            note.note = PKNOTE_SOL;
            break;
        case 5:
            note.note = PKNOTE_LA;
            break;
        case 6:
            note.note = PKNOTE_SI;
            break;
        }

        text = PKNoteGetName(note, this->notation);

        PKVariantWString newText(text);
        this->buttons[i]->set(PKStretchToggleButton::TEXT_WSTRING_PROPERTY, &newText);
    }

    // Update the tunings

    this->skipComboChanges = true;

    int sel = this->getComboBoxSelectedItem("ID_TUNING");
	this->clearComboBoxItems("ID_TUNING");

	for(uint32_t i=0; i<this->tunings.size(); i++)
	{
		std::wstring tName = this->tunings[i]->getName();

		tName += L" (";

		for(uint32_t n=0; n<this->tunings[i]->getNumberOfStrings(); n++)
		{
			tName += PKNoteGetName(this->tunings[i]->getNoteForString(n),
								   this->notation, true);


            if((this->notation == PKNOTATION_NAMES) && 
               (n != (this->tunings[i]->getNumberOfStrings() - 1)))
            {
                tName += L" ";
            }
		}

		tName += L")";

		this->addComboBoxItem("ID_TUNING", tName);
	}

    this->skipComboChanges = false;

    // This wil update the chords
    this->selectComboBoxItem("ID_TUNING", sel);
}

void PKChordLibraryDialog::reset()
{
    // Select the C note
    PKVariantBool toggled_true(true);
    PKVariantBool toggled_false(false);

	buttons[0]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_true);
	buttons[1]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	buttons[2]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	buttons[3]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	buttons[4]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	buttons[5]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	buttons[6]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);

	// Get the other buttons

	sharp->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	flat->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);

	fivePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	fiveMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	ninePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	nineMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	thirteenPlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
	thirteenMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);

	chordTypes->setCurrentSelection(-1);
	chordTypes->setCurrentSelection(0);
	
	this->setCheckBoxChecked("ID_ALLOW_OPEN", false);

	this->bassNote.note   = 0;
	this->bassNote.octave = 0;

	this->rootNote.note   = 0;
	this->rootNote.octave = 0;

	this->updateChords();
}

void PKChordLibraryDialog::matchChordStateAndDiagram(PKChordDiagram      *diagram,
											         PKChordLibraryState *state)
{
	if(diagram == NULL || state == NULL)
	{
		return;
	}

    PKVariantBool toggled_true(true);
    PKVariantBool toggled_false(false);

	// Select the root note

	for(uint32_t i=0; i<7; i++)
	{
		if(i == state->noteIndex)
		{
			buttons[i]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_true);
		}
		else
		{
			buttons[i]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled_false);
		}
	}

	// Setup the other fields

	sharp->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->sharp ? &toggled_true : &toggled_false);
	flat->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->flat ? &toggled_true : &toggled_false);

	fivePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->fivePlus ? &toggled_true : &toggled_false);
	fiveMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->fiveMinus ? &toggled_true : &toggled_false);
	ninePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->ninePlus ? &toggled_true : &toggled_false);
	nineMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->nineMinus ? &toggled_true : &toggled_false);
	thirteenPlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->thirteenPlus ? &toggled_true : &toggled_false);
	thirteenMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, state->thirteenMinus ? &toggled_true : &toggled_false);

	// Select the right chord type
		
    std::vector <PKChordDescription> chords = PKChordGetAllChordsDescriptions();

	PKNote note;

	note.note   = state->noteIndex;
	note.octave = 0;

	PKChordStruct chord = PKChordGetChordForNoteAndIntervals(note, state->chordType_intervals, PKCHORD_MAX_INTERVALS);

    chordTypes->setCurrentSelection(-1);

	for(uint32_t i=0; i<chords.size(); i++)
    {
		if(PKChordMatchesDescription(chord, chords[i]))
		{
			chordTypes->setCurrentSelection(i);
			break;
		}
	}

	this->rootNote = state->rootNote;
	this->bassNote = state->bassNote;

	this->setCheckBoxChecked("ID_ALLOW_OPEN", state->allowOpenStrings);

    // Update the dialog state
    this->updateChords();

	// Now find the diagram that matches
	// and select it so it's foremost

	for(uint32_t n=0; n<chordGrid->getChildrenCount(); n++)
	{
		PKChordDiagramVectorUI *child = dynamic_cast<PKChordDiagramVectorUI *>(chordGrid->getChildAt(n));

		if(child)
		{
			PKVariantBool sel(false);
			child->set(PKChordDiagramVectorUI::SELECTED_BOOL_PROPERTY, &sel);
		}
	}
	
	for(uint32_t n=0; n<chordGrid->getChildrenCount(); n++)
	{
		PKChordDiagramVectorUI *child = dynamic_cast<PKChordDiagramVectorUI *>(chordGrid->getChildAt(n));

		if(child)
		{
			PKChordDiagram *child_diagram = child->getChordDiagram();

			if(PKChordDiagramEquivalent(diagram, child_diagram))
			{
				PKVariantBool sel(true);
				child->set(PKChordDiagramVectorUI::SELECTED_BOOL_PROPERTY, &sel);

				this->currentDiagram = child_diagram;

				PKObject *object = this->findControl("CHORD_DIAGRAM");

				if(object)
				{
					PKChordDiagramVectorUI *main_ui = dynamic_cast<PKChordDiagramVectorUI *>(object);
			
					if(main_ui)
					{
						main_ui->setChordDiagram(this->currentDiagram);				
					}
				}
			
				break;
			}
		}
	}
	
}


void PKChordLibraryDialog::updateChords()
{
    if(!this->inited)
        return;

    this->skipComboChanges = true;

	// Find selected root note

	uint32_t i=0;

	for(i=0; i<buttons.size(); i++)
    {
		if(PKVALUE_BOOL(buttons[i]->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY)))
		{
			break;
		}
	}

	PKNote currentRoot = this->rootNote;

	// Update diagram

	PKObject *object = this->findControl("CHORD_DIAGRAM");

	if(object)
	{
		PKChordDiagramVectorUI *ui = dynamic_cast<PKChordDiagramVectorUI *>(object);

		if(ui)
		{
			PKVariantBool v(true);
			ui->set(PKChordDiagramVectorUI::ALWAYSSHOWFRET_BOOL_PROPERTY, &v);

			// Cleanup chord grid UI

			if(chordGrid)
			{
				chordGrid->hide();

				for(uint32_t n=0; n<chordGrid->getChildrenCount(); n++)
				{
					PKChordDiagramVectorUI *child = dynamic_cast<PKChordDiagramVectorUI *>(chordGrid->getChildAt(n));

					if(child)
					{
						child->release();
					}
				}

				chordGrid->clearChildren();
			}

			// Cleanup chord vector

			for(uint32_t n=0; n < chords.size(); n++)
			{
				if(chords[n])
				{
					delete chords[n];
				}
			}

			chords.clear();

			bool s = PKVALUE_BOOL(sharp->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
			bool f = PKVALUE_BOOL(flat->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));

			switch(i)
			{
			case 0: // C
				{
					rootNote.note = PKNOTE_C;
					if(s) rootNote.note = PKNOTE_C_SHARP;
					if(f) rootNote.note = PKNOTE_B;
				}
				break;
			case 1: // D
				{
					rootNote.note = PKNOTE_D;
					if(s) rootNote.note = PKNOTE_D_SHARP;
					if(f) rootNote.note = PKNOTE_D_FLAT;
				}
				break;
			case 2: // E
				{
					rootNote.note = PKNOTE_E;
					if(s) rootNote.note = PKNOTE_F;
					if(f) rootNote.note = PKNOTE_E_FLAT;
				}
				break;
			case 3: // F
				{
					rootNote.note = PKNOTE_F;
					if(s) rootNote.note = PKNOTE_F_SHARP;
					if(f) rootNote.note = PKNOTE_E;
				}
				break;
			case 4: // G
				{
					rootNote.note = PKNOTE_G;
					if(s) rootNote.note = PKNOTE_G_SHARP;
					if(f) rootNote.note = PKNOTE_G_FLAT;
				}
				break;
			case 5: // A
				{
					rootNote.note = PKNOTE_A;
					if(s) rootNote.note = PKNOTE_A_SHARP;
					if(f) rootNote.note = PKNOTE_A_FLAT;
				}
				break;
			case 6: // B
				{
					rootNote.note = PKNOTE_B;
					if(s) rootNote.note = PKNOTE_C;
					if(f) rootNote.note = PKNOTE_B_FLAT;
				}
				break;
			}

            std::vector <PKChordDescription> the_chords = PKChordGetAllChordsDescriptions();
			int32_t chordTypeIndex = chordTypes->getCurrentSelection();

			if(chordTypeIndex < (int32_t) the_chords.size() && chordTypeIndex >= 0)
			{
				this->chordType = the_chords[chordTypeIndex];
			}

			PKChordDescription alteredChordType = this->chordType;

			// Apply alterations to the new chord type if any

			uint32_t numNotes  = chordType.intervals.size();

			bool has5th  = false;
			bool has9th  = false;
			bool has13th = false;

			bool apply5Plus   = PKVALUE_BOOL(fivePlus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
			bool apply5Minus  = PKVALUE_BOOL(fiveMinus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
			bool apply9Plus   = PKVALUE_BOOL(ninePlus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
			bool apply9Minus  = PKVALUE_BOOL(nineMinus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
			bool apply13Plus  = PKVALUE_BOOL(thirteenPlus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
			bool apply13Minus = PKVALUE_BOOL(thirteenMinus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));

			for(uint32_t i=1; i<=numNotes; i++)
			{
				if(chordType.intervals[i-1] == PK_INTERVAL_5TH)
				{
					has5th = true;
				}

				if(chordType.intervals[i-1] == PK_INTERVAL_9TH)
				{
					has9th = true;
				}

				if(chordType.intervals[i-1] == PK_INTERVAL_13TH)
				{
					has13th = true;
				}
			}

			if(has5th)
			{
				if(apply5Plus)
				{
					PKChordReplaceInterval(&alteredChordType,
										   PK_INTERVAL_5TH,
										   PK_INTERVAL_5TH_AUG);
				}

				if(apply5Minus)
				{
					PKChordReplaceInterval(&alteredChordType,
										   PK_INTERVAL_5TH,
										   PK_INTERVAL_5TH_DIM);
				}
			}

			if(has9th)
			{
				if(apply9Plus)
				{
					PKChordReplaceInterval(&alteredChordType,
										   PK_INTERVAL_9TH,
										   PK_INTERVAL_9TH_AUG);
				}

				if(apply9Minus)
				{
					PKChordReplaceInterval(&alteredChordType,
										   PK_INTERVAL_9TH,
										   PK_INTERVAL_9TH_DIM);
				}
			}

			if(has13th)
			{
				if(apply13Plus)
				{
					PKChordReplaceInterval(&alteredChordType,
										   PK_INTERVAL_13TH,
										   PK_INTERVAL_13TH_AUG);
				}

				if(apply13Minus)
				{
					PKChordReplaceInterval(&alteredChordType,
										   PK_INTERVAL_13TH,
										   PK_INTERVAL_13TH_DIM);
				}
			}

			// Update the bass note combobox

			if(!this->onlyBassChange)
			{
				this->clearComboBoxItems("ID_BASS");
			}

			// Update the note breakdown
				
			uint32_t matchBass = -1;
			bool preferSharpBass = true;

			for(uint32_t i=1; i<=numNotes; i++)
			{
				std::string lidh = "ID_LABEL_HEADER_" + PKStr::uint32ToString(i);
				std::string lidn = "ID_LABEL_NOTE_" + PKStr::uint32ToString(i);
			
				this->showControl(lidh);
				this->showControl(lidn);

				this->setLabelText(lidh, PKNoteGetShortIntervalName(alteredChordType.intervals[i-1]) + L":");

				PKNote note = rootNote;
				PKNoteOffset(rootNote, alteredChordType.intervals[i-1], &note);
				bool preferSharp = true;

				if(chordType.intervals[i-1] == PK_INTERVAL_2ND_MIN ||
                   chordType.intervals[i-1] == PK_INTERVAL_3RD_MIN ||
				   chordType.intervals[i-1] == PK_INTERVAL_5TH_DIM ||
				   chordType.intervals[i-1] == PK_INTERVAL_7TH_MIN ||
				   chordType.intervals[i-1] == PK_INTERVAL_9TH_DIM)
				{
					preferSharp = false;
				}

				if(note.note == this->bassNote.note)
				{
					matchBass = i;
					preferSharpBass = preferSharp;
				}

				this->setLabelText(lidn, PKNoteGetName(note, this->notation, preferSharp));

				if(!this->onlyBassChange)
				{
					this->addComboBoxItem("ID_BASS", PKNoteGetName(note, this->notation, preferSharp));
				}
			}

			// Update alterations toggles

			if(has5th)
			{
				this->enableControl("ID_5PLUS");
				this->enableControl("ID_5MINUS");
			}
			else
			{
				PKVariantBool toggled(false);
				fivePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled);
				fiveMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled);

				this->disableControl("ID_5PLUS");
				this->disableControl("ID_5MINUS");
			}

			if(has9th)
			{
				this->enableControl("ID_9PLUS");
				this->enableControl("ID_9MINUS");
			}
			else
			{
				PKVariantBool toggled(false);
				ninePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled);
				nineMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled);

				this->disableControl("ID_9PLUS");
				this->disableControl("ID_9MINUS");
			}

			if(has13th)
			{
				this->enableControl("ID_13PLUS");
				this->enableControl("ID_13MINUS");
			}
			else
			{
				PKVariantBool toggled(false);
				thirteenPlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled);
				thirteenMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &toggled);

				this->disableControl("ID_13PLUS");
				this->disableControl("ID_13MINUS");
			}

			for(uint32_t i=numNotes+1; i<=7; i++)
			{
				std::string lidh = "ID_LABEL_HEADER_" + PKStr::uint32ToString(i);
				std::string lidn = "ID_LABEL_NOTE_" + PKStr::uint32ToString(i);
			
				this->hideControl(lidh);
				this->hideControl(lidn);
			}

			// Select the right bass

			if(!this->onlyBassChange)
			{
				if((this->bassNote.note == 1) ||
				   (this->rootNote.note != currentRoot.note))
				{
					this->selectComboBoxItem("ID_BASS", 0);
					this->bassNote = rootNote;
				}
				else
				{
					if(matchBass != -1)
					{
						this->selectComboBoxItem("ID_BASS", matchBass-1);
					}
					else
					{
						// Fall back to root note
						this->selectComboBoxItem("ID_BASS", 0);
						this->bassNote = rootNote;
					}
				}
			}
			else
			{
				this->onlyBassChange = false;
			}

			// Now generate the chords

			uint32_t startFret = 0;
			uint32_t endFret   = 4;

			finder.setAllowOpenStrings(this->getCheckBoxChecked("ID_ALLOW_OPEN"));

			while(startFret <= 20)
			{
				finder.findChords(rootNote,
					              bassNote,
								  alteredChordType,
								  startFret, endFret, &chords);

				if(startFret == 0)
				{
					startFret++;
				}
				else
				{
					startFret++;
					endFret++;
				}
			}

			for(uint32_t n=0; n<chords.size(); n++)
			{
				chords[n]->setNumberOfFrets(4);
			}

			// Trim chords arrray (erase duplicates)

			chords = finder.trimChords(chords);

			// Select first chord in main chord diagram view

			this->currentDiagram = NULL;

			if(chords.size())
			{
				this->currentDiagram = chords[0];
				ui->setChordDiagram(chords[0]);

				diagramRoot->invalidate();
			}

			// Display chord name

			std::wstring chordName = PKNoteGetName(rootNote, notation) + chordType.shortName;

			if(apply5Plus)
			{
				chordName += L"/5+";
			}

			if(apply5Minus)
			{
				chordName += L"/5-";
			}

			if(apply9Plus)
			{
				if(PKStr::findFirstOf(chordName, L"9") != std::wstring::npos)
				{
					chordName = PKStr::replaceString(L"9", L"9+", chordName);
				}
				else
				{
					chordName += L"/9+";
				}
			}

			if(apply9Minus)
			{
				if(PKStr::findFirstOf(chordName, L"9") != std::wstring::npos)
				{
					chordName = PKStr::replaceString(L"9", L"9-", chordName);
				}
				else
				{
					chordName += L"/9-";
				}
			}

			if(apply13Plus)
			{
				if(PKStr::findFirstOf(chordName, L"13") != std::wstring::npos)
				{
					chordName = PKStr::replaceString(L"13", L"13+", chordName);
				}
				else
				{
					chordName += L"/13+";
				}
			}

			if(apply13Minus)
			{
				if(PKStr::findFirstOf(chordName, L"13") != std::wstring::npos)
				{
					chordName = PKStr::replaceString(L"13", L"13-", chordName);
				}
				else
				{
					chordName += L"/13-";
				}
			}
			
			// Add the bass if needed
			
			if(rootNote.note != bassNote.note)
			{
				chordName += (L"/" + PKNoteGetName(bassNote, notation, preferSharpBass));
			}

			this->setLabelText("CHORD_NAME", chordName);
			this->setLabelText("ID_SMALL_NAME", chordName);

			this->currentChordName = chordName;

			// Add other chord diagrams to the grid layout

			if(chordGrid)
			{
				for(uint32_t n=0; n<chords.size(); n++)
				{
					PKChordDiagramVectorUI *diagramUI = new PKChordDiagramVectorUI();

					PKVariantBool shared(true);
					diagramUI->set(PKVectorControl::SHARED_VECTOR_ENGINE_BOOL_PROPERTY, &shared);

					PKVariantUInt32 mh(54);
					diagramUI->set(PKChordDiagramVectorUI::MINHEIGTH_UINT32_PROPERTY, &mh);

					diagramUI->setChordDiagram(chords[n]);
					chordGrid->addChild(diagramUI);

					diagramUI->build();
					diagramUI->initialize(this);

					if(n == 0)
					{
						PKVariantBool sel(true);
						diagramUI->set(PKChordDiagramVectorUI::SELECTED_BOOL_PROPERTY, &sel);
					}
				}

				if(vectorRoot)
				{
					vectorRoot->resize(vectorRoot->getMinimumWidth(), 
									   vectorRoot->getMinimumHeight());
					vectorRoot->invalidate();
				}

				if(scrollView)
				{
					// Update layouting
					scrollView->updateScrollbars();
					scrollView->scrollToTop();
				}

				chordGrid->show();
			}
		}
	}

    this->skipComboChanges = false;
}

PKChordDiagram *PKChordLibraryDialog::getSelectedChordDiagram()
{
	PKChordDiagram *d = new PKChordDiagram(this->currentDiagram);
	return d;
}

std::wstring PKChordLibraryDialog::getSelectedChordName()
{
	return this->currentChordName;
}

std::wstring PKChordLibraryDialog::getChordNameForLibraryState(PKChordLibraryState *state, PKNotationName n)
{
	if(state == NULL)
	{
		return L"";
	}

	// Find selected root note

	uint32_t i = state->noteIndex;
	PKNote currentRoot = state->rootNote;

	bool s = state->sharp;
	bool f = state->flat;

	PKNote rootNote;

	switch(i)
	{
	case 0: // C
		{
			rootNote.note = PKNOTE_C;
			if(s) rootNote.note = PKNOTE_C_SHARP;
			if(f) rootNote.note = PKNOTE_B;
		}
		break;
	case 1: // D
		{
			rootNote.note = PKNOTE_D;
			if(s) rootNote.note = PKNOTE_D_SHARP;
			if(f) rootNote.note = PKNOTE_D_FLAT;
		}
		break;
	case 2: // E
		{
			rootNote.note = PKNOTE_E;
			if(s) rootNote.note = PKNOTE_F;
			if(f) rootNote.note = PKNOTE_E_FLAT;
		}
		break;
	case 3: // F
		{
			rootNote.note = PKNOTE_F;
			if(s) rootNote.note = PKNOTE_F_SHARP;
			if(f) rootNote.note = PKNOTE_E;
		}
		break;
	case 4: // G
		{
			rootNote.note = PKNOTE_G;
			if(s) rootNote.note = PKNOTE_G_SHARP;
			if(f) rootNote.note = PKNOTE_G_FLAT;
		}
		break;
	case 5: // A
		{
			rootNote.note = PKNOTE_A;
			if(s) rootNote.note = PKNOTE_A_SHARP;
			if(f) rootNote.note = PKNOTE_A_FLAT;
		}
		break;
	case 6: // B
		{
			rootNote.note = PKNOTE_B;
			if(s) rootNote.note = PKNOTE_C;
			if(f) rootNote.note = PKNOTE_B_FLAT;
		}
		break;
	}

	PKChordDescription chordType = PKChordGetChordDescriptionMatchingIntervals(state->chordType_intervals, PKCHORD_MAX_INTERVALS);
	PKChordDescription alteredChordType = PKChordGetChordDescriptionMatchingIntervals(state->chordType_intervals, PKCHORD_MAX_INTERVALS);

	// Apply alterations to the new chord type if any

	uint32_t numNotes  = chordType.intervals.size();

	bool has5th  = false;
	bool has9th  = false;
	bool has13th = false;

	bool apply5Plus   = state->fivePlus;
	bool apply5Minus  = state->fiveMinus;
	bool apply9Plus   = state->ninePlus;
	bool apply9Minus  = state->nineMinus;
	bool apply13Plus  = state->thirteenPlus;
	bool apply13Minus = state->thirteenMinus;

	for(uint32_t i=1; i<=numNotes; i++)
	{
		if(chordType.intervals[i-1] == PK_INTERVAL_5TH)
		{
			has5th = true;
		}

		if(chordType.intervals[i-1] == PK_INTERVAL_9TH)
		{
			has9th = true;
		}

		if(chordType.intervals[i-1] == PK_INTERVAL_13TH)
		{
			has13th = true;
		}
	}

	if(has5th)
	{
		if(apply5Plus)
		{
			PKChordReplaceInterval(&alteredChordType,
								   PK_INTERVAL_5TH,
								   PK_INTERVAL_5TH_AUG);
		}

		if(apply5Minus)
		{
			PKChordReplaceInterval(&alteredChordType,
								   PK_INTERVAL_5TH,
								   PK_INTERVAL_5TH_DIM);
		}
	}

	if(has9th)
	{
		if(apply9Plus)
		{
			PKChordReplaceInterval(&alteredChordType,
								   PK_INTERVAL_9TH,
								   PK_INTERVAL_9TH_AUG);
		}

		if(apply9Minus)
		{
			PKChordReplaceInterval(&alteredChordType,
								   PK_INTERVAL_9TH,
								   PK_INTERVAL_9TH_DIM);
		}
	}

	if(has13th)
	{
		if(apply13Plus)
		{
			PKChordReplaceInterval(&alteredChordType,
								   PK_INTERVAL_13TH,
								   PK_INTERVAL_13TH_AUG);
		}

		if(apply13Minus)
		{
			PKChordReplaceInterval(&alteredChordType,
								   PK_INTERVAL_13TH,
								   PK_INTERVAL_13TH_DIM);
		}
	}

	// Compute sharp preference

	bool preferSharpBass = true;

	for(uint32_t i=1; i<=numNotes; i++)
	{
		PKNote note = rootNote;
		PKNoteOffset(rootNote, alteredChordType.intervals[i-1], &note);
		bool preferSharp = true;

		if(chordType.intervals[i-1] == PK_INTERVAL_2ND_MIN ||
           chordType.intervals[i-1] == PK_INTERVAL_3RD_MIN ||
		   chordType.intervals[i-1] == PK_INTERVAL_5TH_DIM ||
		   chordType.intervals[i-1] == PK_INTERVAL_7TH_MIN ||
		   chordType.intervals[i-1] == PK_INTERVAL_9TH_DIM)
		{
			preferSharp = false;
		}

		if(note.note == state->bassNote.note)
		{
			preferSharpBass = preferSharp;
		}
	}

	// Compute chord name

	std::wstring chordName = PKNoteGetName(rootNote, n) + chordType.shortName;

	if(apply5Plus)
	{
		chordName += L"/5+";
	}

	if(apply5Minus)
	{
		chordName += L"/5-";
	}

	if(apply9Plus)
	{
		if(PKStr::findFirstOf(chordName, L"9") != std::wstring::npos)
		{
			chordName = PKStr::replaceString(L"9", L"9+", chordName);
		}
		else
		{
			chordName += L"/9+";
		}
	}

	if(apply9Minus)
	{
		if(PKStr::findFirstOf(chordName, L"9") != std::wstring::npos)
		{
			chordName = PKStr::replaceString(L"9", L"9-", chordName);
		}
		else
		{
			chordName += L"/9-";
		}
	}

	if(apply13Plus)
	{
		if(PKStr::findFirstOf(chordName, L"13") != std::wstring::npos)
		{
			chordName = PKStr::replaceString(L"13", L"13+", chordName);
		}
		else
		{
			chordName += L"/13+";
		}
	}

	if(apply13Minus)
	{
		if(PKStr::findFirstOf(chordName, L"13") != std::wstring::npos)
		{
			chordName = PKStr::replaceString(L"13", L"13-", chordName);
		}
		else
		{
			chordName += L"/13-";
		}
	}
		
	// Add the bass if needed
	
	if(rootNote.note != state->bassNote.note)
	{
		chordName += (L"/" + PKNoteGetName(state->bassNote, n, preferSharpBass));
	}

	return chordName;
}

PKChordLibraryState *PKChordLibraryDialog::getCurrentLibraryState()
{
	PKChordLibraryState *state = new PKChordLibraryState;

	uint32_t i=0;

	for(i=0; i<buttons.size(); i++)
    {
		if(PKVALUE_BOOL(buttons[i]->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY)))
		{
			break;
		}
	}

	state->version          = 1;
	state->noteIndex        = i;
	state->flat             = PKVALUE_BOOL(flat->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->sharp            = PKVALUE_BOOL(sharp->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->bassNote         = this->bassNote;
	state->rootNote         = this->rootNote;

	uint32_t maxSize = pk_min(PKCHORD_MAX_INTERVALS-1, this->chordType.intervals.size());
	for(i=0; i<maxSize; i++)
	{
		state->chordType_intervals[i] = this->chordType.intervals[i];
	}
	state->chordType_intervals[i] = PK_INTERVAL_STOP; // STOP

	state->fivePlus         = PKVALUE_BOOL(fivePlus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->fiveMinus        = PKVALUE_BOOL(fiveMinus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->ninePlus         = PKVALUE_BOOL(ninePlus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->nineMinus        = PKVALUE_BOOL(nineMinus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->thirteenPlus     = PKVALUE_BOOL(thirteenPlus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->thirteenMinus    = PKVALUE_BOOL(thirteenMinus->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));
	state->allowOpenStrings = this->getCheckBoxChecked("ID_ALLOW_OPEN");

	return state;
}

void PKChordLibraryDialog::listBoxSelectionChanged(std::string id, int32_t previousValue)
{
	if(id == "ID_CHORD_TYPES")
	{
		this->updateChords();
	}
}

void PKChordLibraryDialog::comboBoxChanged(std::string id, int32_t previousValue)
{
	if(id == "ID_BASS")
	{
        if(this->skipComboChanges)
        {
            return;
        }

		int32_t selection = this->getComboBoxSelectedItem("ID_BASS");

        if(selection >= 0)
    	{
		    PKNote note;
		    PKNoteOffset(rootNote, chordType.intervals[selection], &note);
		    this->bassNote = note;

		    this->onlyBassChange = true;
		    this->updateChords();
    	}
	}

	if(id == "ID_TUNING")
	{
        if(this->skipComboChanges)
        {
            return;
        }

        int32_t sel = this->getComboBoxSelectedItem("ID_TUNING");

		if(sel >= 0 && sel < (int32_t) this->tunings.size())
		{
			PKTuning *tuning = this->tunings[sel];
			this->finder.applyTuning(tuning);
			this->updateChords();
		}
	}
}


void PKChordLibraryDialog::checkBoxAction(std::string id, bool previousValue)
{
	this->updateChords();
}

void PKChordLibraryDialog::stretchToggleButtonToggled(std::string id,
												      PKStretchToggleButton *button,
												      bool previousValue)
{
	bool on = PKVALUE_BOOL(button->get(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY));

	// Handle the toggling of the note buttons manually

    if(PKStr::findFirstOf(id, "ID_NOTE_") != std::string::npos)
    {
		if(on)
		{
			for(uint32_t i=0; i<buttons.size(); i++)
			{
				if(button != buttons[i]) // turn the others off
				{
					PKVariantBool f(false);
					buttons[i]->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
				}
			}

			// Update the chord diagrams
			this->updateChords();
		}
    }
    else
    {
        // Handle the # and b toggles

		if(on)
		{
			if(button == sharp)
			{
				PKVariantBool f(false);
				flat->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}
	        
			if (button == flat)
			{
				PKVariantBool f(false);
				sharp->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}

			if(button == fivePlus)
			{
				PKVariantBool f(false);
				fiveMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}

			if(button == fiveMinus)
			{
				PKVariantBool f(false);
				fivePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}

			if(button == ninePlus)
			{
				PKVariantBool f(false);
				nineMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}

			if(button == nineMinus)
			{
				PKVariantBool f(false);
				ninePlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}

			if(button == thirteenPlus)
			{
				PKVariantBool f(false);
				thirteenMinus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}

			if(button == thirteenMinus)
			{
				PKVariantBool f(false);
				thirteenPlus->set(PKStretchToggleButton::TOGGLED_BOOL_PROPERTY, &f);
			}
		}

		// Update the chord diagrams
		this->updateChords();
	}
}

void PKChordLibraryDialog::buttonClicked(std::string id)
{
	this->hide();
	
    if(id == "ID_CHOOSE")
    {
        this->terminateDialog(PK_STD_BUTTON_OK);
    }

    if(id == "ID_CANCEL")
    {
        this->terminateDialog(PK_STD_BUTTON_CANCEL);
    }
}

void PKChordLibraryDialog::windowClosed()
{
	this->hide();
    this->terminateDialog(PK_STD_BUTTON_CANCEL);
}

void PKChordLibraryDialog::customControlClicked(PKButtonType button,
                                                std::string id,
                                                PKCustomControl *control,
                                                std::string partCode)
{
	PKChordDiagramVectorUI *ui = dynamic_cast<PKChordDiagramVectorUI *>(control);

	if(ui == NULL)
	{
		return;
	}

	PKObject *object = this->findControl("CHORD_DIAGRAM");

	if(object)
	{
		PKChordDiagramVectorUI *main_ui = dynamic_cast<PKChordDiagramVectorUI *>(object);

		if(main_ui)
		{
			// Is this the main diagram display ?

			if(main_ui == ui)
			{
				return;
			}

			// Else look which one it was
		
			if(chordGrid && (this->currentDiagram != ui->getChordDiagram()))
			{
				for(uint32_t n=0; n<chordGrid->getChildrenCount(); n++)
				{
					PKChordDiagramVectorUI *child = dynamic_cast<PKChordDiagramVectorUI *>(chordGrid->getChildAt(n));

					if(child)
					{
						if(child == ui)
						{
							PKVariantBool sel(true);
							child->set(PKChordDiagramVectorUI::SELECTED_BOOL_PROPERTY, &sel);
						}

						if(child->getChordDiagram() == this->currentDiagram)
						{
							PKVariantBool sel(false);
							child->set(PKChordDiagramVectorUI::SELECTED_BOOL_PROPERTY, &sel);
						}
					}
				}

				this->currentDiagram = ui->getChordDiagram();
				main_ui->setChordDiagram(this->currentDiagram);
				diagramRoot->invalidate();

				if(vectorRoot)
				{
					vectorRoot->invalidate();
				}
			}
		}
	}
}


