//
//
//

#ifndef PK_CHORD_LIBRARY_DIALOG_H
#define PK_CHORD_LIBRARY_DIALOG_H

//
// Includes
//

#include "PKModalDialog.h"
#include "PKChordFinder.h"
#include "PKStretchToggleButton.h"
#include "PKListBox.h"
#include "PKGridLayout.h"
#include "PKSpacer.h"
#include "PKScrollView.h"
#include "PKMusicUIFactory.h"
#include "PKTuning.h"
#include "PKChordDiagramVectorUI.h"
#include "PKVectorControlRoot.h"

//
// State structure
//

#define PKCHORD_MAX_INTERVALS  10
#define PKCHORD_INTERVAL_STOP 255

typedef struct _PKChordLibraryState
{
	int32_t version;

	int32_t noteIndex;
	bool    sharp;
	bool    flat;

	bool    fivePlus;
	bool    fiveMinus;

	bool    ninePlus;
	bool    nineMinus;

	bool    thirteenPlus;
	bool    thirteenMinus;

	PKNote  rootNote;
	PKNote  bassNote;

	uint8_t chordType_intervals[PKCHORD_MAX_INTERVALS];

	bool allowOpenStrings;

} PKChordLibraryState;

//
// PKChordLibraryDialog
//

class PKChordLibraryDialog : public PKModalDialog
{
public:

	PKChordLibraryDialog(PKWindowHandle parent = NULL,
			             std::wstring   icon   = L"");
	
	~PKChordLibraryDialog();

	// Main

	void    reset();
	void    matchChordStateAndDiagram(PKChordDiagram      *diagram,
							          PKChordLibraryState *state);

    void selectTuningAtIndex(int index);
    void setNotation(PKNotationName n);

    PKChordDiagram      *getSelectedChordDiagram();
    std::wstring         getSelectedChordName();
    PKChordLibraryState *getCurrentLibraryState();

	std::wstring getChordNameForLibraryState(PKChordLibraryState *state, PKNotationName n);

	// Setup

    void initUIElements();
	void updateChords();

	// Internal

	void listBoxSelectionChanged(std::string id, int32_t previousValue);
	void comboBoxChanged(std::string id, int32_t previousValue);
	void checkBoxAction(std::string id, bool previousValue);

	void stretchToggleButtonToggled(std::string id,
                                    PKStretchToggleButton *button,
									bool previousValue);

	void buttonClicked(std::string id);
	void windowClosed();

    virtual void customControlClicked(PKButtonType button,
                                      std::string id,
                                      PKCustomControl *control,
                                      std::string partCode);

private:

	std::vector<PKChordDiagram *> chords;
	PKChordFinder  finder;
	PKNotationName notation;
    PKListBox *chordTypes;
    PKStretchToggleButton *sharp;
    PKStretchToggleButton *flat;
    PKStretchToggleButton *fivePlus;
    PKStretchToggleButton *fiveMinus;
    PKStretchToggleButton *ninePlus;
    PKStretchToggleButton *nineMinus;
    PKStretchToggleButton *thirteenPlus;
    PKStretchToggleButton *thirteenMinus;
    std::vector<PKStretchToggleButton*> buttons;
	PKScrollView *scrollView;
	PKGridLayout *chordGrid;
	PKMusicUIFactory *factory;
    PKWindowHandle parentW;
	PKNote rootNote;
	PKNote bassNote;
	PKChordDescription chordType;
	bool onlyBassChange;
	std::vector<PKTuning*> tunings;
	PKChordDiagram *currentDiagram;
	PKVectorControlRoot *vectorRoot;
	PKVectorControlRoot *diagramRoot;
	bool inited;
    bool skipComboChanges;
	std::wstring currentChordName;
};

#endif // PK_CHORD_LIBRARY_DIALOG_H
