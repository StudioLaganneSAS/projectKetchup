//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKMACCUSTOMVIEW_H
#define PKMACCUSTOMVIEW_H

#include <Carbon/Carbon.h>
#include "PKUI.h"
#include "PKMacView.h"

class PKCustomControl;

OSStatus	HIPKMacCustomViewCreate(
								WindowRef			window,
								const HIRect*		boundsRect,
								PKCustomControl    *control,
								ControlRef*			outControl );

// PKMacCustomView

class PKMacCustomView : public PKMacView
{
public:
	
	static OSStatus			Create(
								WindowRef			window,
								const HIRect*		boundsRect,
								PKCustomControl    *control,
								ControlRef*			outControl );
protected:
	
	// Construction/Destruction
	PKMacCustomView( HIViewRef inControl );
	~PKMacCustomView();
	
	ControlKind				GetKind();
	
	virtual void		    Draw(
								RgnHandle			inLimitRgn,
								CGContextRef		inContext );
	
	virtual OSStatus		TextInput(PKCarbonEvent &inEvent);

	virtual OSStatus		KeyDown(PKCarbonEvent& inEvent);
	virtual OSStatus		KeyUp(PKCarbonEvent& inEvent);

	virtual OSStatus		SetFocusPart(ControlPartCode	inDesiredFocus,
										 RgnHandle			inNnvalidRgn,
										 Boolean			inFocusEverything,
										 ControlPartCode*	outActualFocus);

	virtual void focusAcquired(PKDirection dir);
	virtual void focusLost();

	virtual bool moveFocusToNextControlPart();
	virtual bool moveFocusToPreviousControlPart();

	// Scrollview handling
	
	virtual int getScrollableWidth();
	virtual int getScrollableHeight();
	
	virtual int getScrollLineWidth();
	virtual int getScrollLineHeight();
	
	virtual void setScrollOrigin(int x, int y);

private:
	
	bool activated;
	PKCustomControl *control;
	
	static OSStatus			Construct(
								ControlRef		inControl,
								PKMacView**			outView );
							
};

typedef PKMacCustomView* PKMacCustomViewPtr;

#endif // PKMACCUSTOMVIEW_H
