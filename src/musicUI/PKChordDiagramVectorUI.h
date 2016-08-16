//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CHORD_DIAGRAM_VECTOR_UI_H
#define PK_CHORD_DIAGRAM_VECTOR_UI_H

#include "PKVectorControl.h"
#include "PKChordDiagram.h"

class PKChordDiagramVectorUI;

//
// PKChordDiagramUICallback
//

class PKChordDiagramUICallback
{
public:

	virtual void chordDiagramClicked(PKChordDiagramVectorUI *ui) = 0;
};

//
// PKChordDiagramVectorUI
//

class PKChordDiagramVectorUI : public PKVectorControl
{
public:

    //
    // PROPERTIES
    //

    static std::string BACKCOLOR_COLOR_PROPERTY;
    static std::string FORECOLOR_COLOR_PROPERTY;
    static std::string MINHEIGTH_UINT32_PROPERTY;
	static std::string ALWAYSSHOWFRET_BOOL_PROPERTY;
    static std::string SELECTED_BACKCOLOR_COLOR_PROPERTY;
    static std::string SELECTED_FORECOLOR_COLOR_PROPERTY;
	static std::string SELECTED_BOOL_PROPERTY;

public:
	PKChordDiagramVectorUI();

protected:
	virtual ~PKChordDiagramVectorUI();

public:
    virtual void build();

    // Getters/Setters

    void setChordDiagram(PKChordDiagram *d);
	PKChordDiagram *getChordDiagram();

	void setUICallback(PKChordDiagramUICallback *call);

    // From PKCustomControl
	
    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKVectorControl

	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

	// From PKCustomControl

    virtual void mouseDown(PKButtonType button,
				            int x,
				            int y,
				            unsigned int mod);
        
    virtual void mouseUp(PKButtonType button,
						 int x,
						 int y,
						 unsigned int mod);

protected:

    virtual void drawVector(PKVectorEngine *engine,
		                    int32_t x, int32_t y);

private:

    PKChordDiagram *diagram;
	PKChordDiagramUICallback *callback;
    bool hit;
};

#endif // PK_CHORD_DIAGRAM_VECTOR_UI_H
