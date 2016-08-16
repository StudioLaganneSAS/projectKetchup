//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKMacView_H_
#define PKMacView_H_

#include <Carbon/Carbon.h>
#include "PKCarbonEvent.h"
#include "PKUI.h"

#define PURE_VIRTUAL 0

class PKMacView
{
public:
	// Bounds and Frame
	OSStatus				SetFrame(
								const HIRect&		inBounds );
	HIRect					Frame();
	HIRect					Bounds();
	
	// Visibility
	OSStatus				Show();
	OSStatus				Hide();

	EventTargetRef			GetEventTarget();

	OSStatus				AddSubView(
								PKMacView*				inSubView );
	OSStatus				RemoveFromSuperView();

	// Accessors
	HIViewRef				GePKMacViewRef()
								{ return fViewRef; }
	void					Hilite(
								ControlPartCode		inPart );
	ControlPartCode			GetHilite();
	WindowRef				GetOwner();
	SInt32					GetValue();
	void					SetValue(
								SInt32				inValue );
	SInt32					GetMinimum();
	void					SetMinimum(
								SInt32				inMinimum );
	SInt32					GetMaximum();
	void					SetMaximum(
								SInt32				inMaximum );

	// State
	Boolean					IsVisible();
	Boolean					IsEnabled();
	Boolean					IsActive();
	
	OSStatus				Invalidate();		// was SetNeedsDisplay()

protected:
	// Autoinvalidation
	enum					{ 
								kAutoInvalidateOnActivate 		= (1 << 0),
								kAutoInvalidateOnHilite			= (1 << 1),
								kAutoInvalidateOnEnable			= (1 << 2),
								kAutoInvalidateOnValueChange	= (1 << 3),
								kAutoInvalidateOnTitleChange	= (1 << 4)
							};
	void					ChangeAutoInvalidateFlags(
								OptionBits			inSetFlags,
								OptionBits			inClearFlags );
	OptionBits				GetAutoInvalidateFlags()
								{ return fAutoInvalidateFlags; }

	// Types
	typedef OSStatus		(*ConstructProc)(
								ControlRef			inBaseControl,
								PKMacView**				ouPKMacView );

	// Construction/Destruction
							PKMacView( HIViewRef inControl );
	virtual					~PKMacView();
	
	virtual ControlKind		GetKind() = PURE_VIRTUAL;
	virtual UInt32			GetBehaviors();

	// Handlers
	
	virtual int getScrollableWidth();
	virtual int getScrollableHeight();

	virtual int getScrollLineWidth();
	virtual int getScrollLineHeight();
	
	virtual void setScrollOrigin(int x, int y);
	
	// Focus
	
	virtual void focusAcquired(PKDirection dir);
	virtual void focusLost();
	
	virtual bool moveFocusToNextControlPart();
	virtual bool moveFocusToPreviousControlPart();
	
	// Signals
	
	virtual OSStatus		ActiveStateChanged();
	virtual OSStatus		BoundsChanged(
								UInt32				inOptions,
								const HIRect&		inOriginalBounds,
								const HIRect&		inCurrentBounds,
								RgnHandle			inInvalRgn );
	virtual OSStatus		ControlHit(
								ControlPartCode		inPart,
								UInt32				inModifiers );
	
	virtual OSStatus		ControlClick(EventMouseButton button,
										 int x, int y,
									     UInt32	inModifiers);
	
	virtual OSStatus		EnabledStateChanged();
	virtual void			Draw(
								RgnHandle			inLimitRgn,
								CGContextRef		inContext );
	virtual OSStatus		GetData(
								OSType				inTag,
								ControlPartCode		inPart,
								Size				inSize,
								Size*				outSize,
								void*				inPtr );
	virtual OSStatus		GetRegion(
								ControlPartCode		inPart,
								RgnHandle			outRgn );
	virtual OSStatus		HiliteChanged(
								ControlPartCode		inOriginalPart,
								ControlPartCode		inCurrentPart,
								RgnHandle			inInvalRgn );
	virtual ControlPartCode	HitTest(
								const HIPoint&		inWhere );
	virtual OSStatus		Initialize(
								PKCarbonEvent&		inEvent );
	virtual OSStatus		SetData(
								OSType				inTag,
								ControlPartCode		inPart,
								Size				inSize,
								const void*			inPtr );
	virtual OSStatus		SetFocusPart(
								ControlPartCode		inDesiredFocus,
								RgnHandle			inNnvalidRgn,
								Boolean				inFocusEverything,
								ControlPartCode*	outActualFocus );
	virtual OSStatus		TextInput(
								PKCarbonEvent&		inEvent );
	virtual OSStatus		KeyDown(
									  PKCarbonEvent&		inEvent );
	virtual OSStatus		KeyUp(
									  PKCarbonEvent&		inEvent );
	virtual OSStatus		TitleChanged();
	virtual OSStatus		Track(
								PKCarbonEvent&		inEvent,
								ControlPartCode*	outPartHit );
	virtual OSStatus		ValueChanged();
	
	// Sizing
	virtual OSStatus		GetSizeConstraints(
								HISize*				outMin,
								HISize*				outMax );
	virtual OSStatus		GetOptimalSize(
								HISize*				outSize,
								float*				outBaseLine );

	// Accessors
	WindowRef				GetWindowRef()
								{ return GetControlOwner( GePKMacViewRef() ); }
	
	
	// Drag and drop
	virtual bool			DragEnter(
								DragRef				inDrag );
	virtual bool			DragWithin(
								DragRef				inDrag );
	virtual bool			DragLeave(
								DragRef				inDrag );
	virtual OSStatus		DragReceive(
								DragRef				inDrag );

	// Command processing
	virtual OSStatus		ProcessCommand(
								const HICommand&	inCommand );
	virtual OSStatus		UpdateCommandStatus(
								const HICommand&	inCommand );

	// Utility
	static OSStatus			RegisterSubclass(
								CFStringRef			inID,
								ConstructProc		inProc );
	static EventRef			CreateInitializationEvent();
	enum Interface			{
								kDragAndDrop		= 1,
								kKeyboardFocus
							};
	virtual OSStatus		ActivateInterface(
								Interface			inInterface );
	
	// Debugging
	virtual void			PrintDebugInfo();

private:
	static pascal OSStatus	ObjectEventHandler(
								EventHandlerCallRef	inCallRef,
								EventRef			inEvent,
								void*				inUserData );
	static pascal OSStatus	ViewEventHandler(
								EventHandlerCallRef	inCallRef,
								EventRef			inEvent,
								void*				inUserData );
	OSStatus				HandleEvent(
								EventHandlerCallRef	inCallRef,
								PKCarbonEvent&		inEvent );

protected:
	HIViewRef				fViewRef;
	EventHandlerRef			fHandler;
	ControlPartCode			_focusedPart;
	bool haveFocus;
	
	OptionBits				fAutoInvalidateFlags;
};

typedef PKMacView*				PKMacViewPtr;

#endif // PKMacView_H_
