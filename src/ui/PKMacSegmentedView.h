//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKMACSEGMENTEDVIEW_H
#define PKMACSEGMENTEDVIEW_H

#include <Carbon/Carbon.h>
#include "PKUI.h"
#include "PKMacView.h"
#include "PKToolbarSegmentedItem.h"

OSStatus	HIPKMacSegmentedViewCreate(WindowRef			   window,
									   const HIRect*		   boundsRect,
									   PKToolbarSegmentedItem *sitem,
									   ControlRef*			   outControl);

// PKMacSegmentedView

class PKMacSegmentedView : public PKMacView
{
public:
	
	static OSStatus			Create(WindowRef			       window,
								   const HIRect*	           boundsRect,
								   PKToolbarSegmentedItem    *sitem,
								   ControlRef*			       outControl);
protected:
	
	// Construction/Destruction
	PKMacSegmentedView( HIViewRef inControl );
	~PKMacSegmentedView();
	
	ControlKind	GetKind();
	
	virtual void Draw(RgnHandle			inLimitRgn,
				 	  CGContextRef		inContext);
	
	virtual OSStatus ControlClick(EventMouseButton button,
								  int x, int y,
								  UInt32 inModifiers);
private:
	
	PKToolbarSegmentedItem *sitem;
	
	static OSStatus	Construct(ControlRef		inControl,
							  PKMacView**		outView);
	
	int overIndex;
	int downIndex;
							
public:

	HMControlContentUPP fHelpContentProviderUPP;
	
	static pascal OSStatus HelpContentProvider(ControlRef inControl, 
											   Point inGlobalMouse, 
											   HMContentRequest inRequest, 
											   HMContentProvidedType *outContentProvided, 
											   HMHelpContentPtr ioHelpContent);
};

typedef PKMacSegmentedView* PKMacSegmentedViewPtr;

#endif // PKMACSEGMENTEDVIEW_H
