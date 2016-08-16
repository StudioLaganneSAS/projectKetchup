//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CHORD_DIAGRAM_UI_H
#define PK_CHORD_DIAGRAM_UI_H

#include "PKDoubleBufferedControl.h"
#include "PKChordDiagram.h"

class PKFont;

//
// PKChordDiagramUI
//

class PKChordDiagramUI : public PKDoubleBufferedControl
{
public:

    //
    // PROPERTIES
    //

    static std::string BACKCOLOR_COLOR_PROPERTY;
    static std::string FORECOLOR_COLOR_PROPERTY;
    static std::string MINHEIGTH_UINT32_PROPERTY;

public:
	PKChordDiagramUI();

protected:
	virtual ~PKChordDiagramUI();

public:
    virtual void build();

    // Getters/Setters

    void setChordDiagram(PKChordDiagram *d);

    // From PKCustomControl
	
    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	void resize(uint32_t w, 
				uint32_t h);

protected:

    virtual void draw32bpp(uint8_t *buffer);

private:

    PKChordDiagram *diagram;
    PKImage        *cache;
};

#endif // PK_CHORD_DIAGRAM_UI_H
