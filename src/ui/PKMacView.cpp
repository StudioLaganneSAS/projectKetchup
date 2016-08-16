//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//


#include "PKMacView.h"

//-----------------------------------------------------------------------------------
//	constants
//-----------------------------------------------------------------------------------
//
const EventTypeSpec kHIObjectEvents[] =
{	{ kEventClassHIObject, kEventHIObjectConstruct },
	{ kEventClassHIObject, kEventHIObjectInitialize },
	{ kEventClassHIObject, kEventHIObjectDestruct }
};
	
const EventTypeSpec kHIViewEvents[] =
{	{ kEventClassCommand, kEventCommandProcess },
	{ kEventClassCommand, kEventCommandUpdateStatus },
	
	{ kEventClassControl, kEventControlInitialize },
	{ kEventClassControl, kEventControlDraw },
	{ kEventClassControl, kEventControlHitTest },
	{ kEventClassControl, kEventControlGetPartRegion },
	{ kEventClassControl, kEventControlGetData },
	{ kEventClassControl, kEventControlSetData },
	{ kEventClassControl, kEventControlGetOptimalBounds },
	{ kEventClassControl, kEventControlBoundsChanged },
	{ kEventClassControl, kEventControlTrack },
	{ kEventClassControl, kEventControlGetSizeConstraints },
	{ kEventClassControl, kEventControlHit },
	{ kEventClassControl, kEventControlClick },
	
	{ kEventClassControl, kEventControlHiliteChanged },
	{ kEventClassControl, kEventControlActivate },
	{ kEventClassControl, kEventControlDeactivate },
	{ kEventClassControl, kEventControlValueFieldChanged },
	{ kEventClassControl, kEventControlTitleChanged },
	{ kEventClassControl, kEventControlEnabledStateChanged },

	{ kEventClassScrollable, kEventScrollableGetInfo },
	{ kEventClassScrollable, kEventScrollableScrollTo }
};

// This param name was accidentally left unexported for
// the release of Jaguar.
const EventParamName kEventParamControlLikesDrag = 'cldg';

//-----------------------------------------------------------------------------------
//	PKMacView constructor
//-----------------------------------------------------------------------------------
//
PKMacView::PKMacView(
	HIViewRef			inControl )
	:	fViewRef( inControl )
{
	_focusedPart = 0;
	haveFocus = false;
	verify_noerr( InstallEventHandler( GetControlEventTarget( fViewRef ), ViewEventHandler,
			GetEventTypeCount( kHIViewEvents ), kHIViewEvents, this, &fHandler ) );
}

//-----------------------------------------------------------------------------------
//	PKMacView destructor
//-----------------------------------------------------------------------------------
//
PKMacView::~PKMacView()
{
}

//-----------------------------------------------------------------------------------
//	Initialize
//-----------------------------------------------------------------------------------
//	Called during HIObject construction, this is your subclasses' chance to extract
//	any parameters it might have added to the initialization event passed into the
//	HIObjectCreate call.
//
OSStatus PKMacView::Initialize(
	PKCarbonEvent&		inEvent )
{
#pragma unused( inEvent )

	return noErr;
}

//-----------------------------------------------------------------------------------
//	GetBehaviors
//-----------------------------------------------------------------------------------
//	Returns our behaviors. Any subclass that overrides this should OR in its behaviors
//	into the inherited behaviors.
//
UInt32 PKMacView::GetBehaviors()
{
	return kControlSupportsDataAccess | kControlSupportsGetRegion;
}

//-----------------------------------------------------------------------------------
//	Draw
//-----------------------------------------------------------------------------------
//	Draw your view. You should draw based on VIEW coordinates, not frame coordinates.
//
void PKMacView::Draw(
	RgnHandle			inLimitRgn,
	CGContextRef		inContext )
{
#pragma unused( inLimitRgn, inContext )
}

//-----------------------------------------------------------------------------------
//	HitTest
//-----------------------------------------------------------------------------------
//	Asks your view to return what part of itself (if any) is hit by the point given
//	to it. The point is in VIEW coordinates, so you should get the view rect to do
//	bounds checking.
//
ControlPartCode PKMacView::HitTest(
	const HIPoint&		inWhere )
{
#pragma unused( inWhere )

	return kControlNoPart;
}

//-----------------------------------------------------------------------------------
//	GetRegion
//-----------------------------------------------------------------------------------
//	This is called when someone wants to know certain metrics regarding this view.
//	The base class does nothing. Subclasses should handle their own parts, such as
//	the content region by overriding this method. The structure region is, by default,
//	the view's bounds. If a subclass does not have a region for a given part, it 
//	should always call the inherited method.
//
OSStatus PKMacView::GetRegion(
	ControlPartCode		inPart,
	RgnHandle			outRgn )
{
#pragma unused( inPart, outRgn )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	PrintDebugInfo
//-----------------------------------------------------------------------------------
//	This is called when asked to print debugging information.
//
void PKMacView::PrintDebugInfo()
{
}

//-----------------------------------------------------------------------------------
//	GetData
//-----------------------------------------------------------------------------------
//	Gets some data from our view. Subclasses should override to handle their own
//	defined data tags. If a tag is not understood by the subclass, it should call the
//	inherited method. As a convienience, we map the request for ControlKind into our
//	GetKind method.
//
OSStatus PKMacView::GetData(
	OSType				inTag,
	ControlPartCode		inPart,
	Size				inSize,
	Size*				outSize,
	void*				inPtr )
{
#pragma unused( inPart )

	OSStatus			err = noErr;
	
	switch( inTag )
	{
		case kControlKindTag:
			if ( inPtr )
			{
				if ( inSize != sizeof( ControlKind ) )
					err = errDataSizeMismatch;
				else
					( *(ControlKind *) inPtr ) = GetKind();
			}
			*outSize = sizeof( ControlKind );
			break;
		
		default:
			err = eventNotHandledErr;
			break;
	}
	
	return err;
}

//-----------------------------------------------------------------------------------
//	SetData
//-----------------------------------------------------------------------------------
//	Sets some data on our control. Subclasses should override to handle their own
//	defined data tags. If a tag is not understood by the subclass, it should call the
//	inherited method.
//
OSStatus PKMacView::SetData(
	OSType				inTag,
	ControlPartCode		inPart,
	Size				inSize,
	const void*			inPtr )
{
#pragma unused( inTag, inPart, inSize, inPtr )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	GetOptimalSize
//-----------------------------------------------------------------------------------
//	Someone wants to know this view's optimal size and text baseline, probably to help
//	do some type of layout. The base class does nothing, but subclasses should
//	override and do something meaningful here.
//
OSStatus PKMacView::GetOptimalSize(
	HISize*				outSize,
	float*				outBaseLine )
{
#pragma unused( outSize, outBaseLine )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	GetSizeConstraints
//-----------------------------------------------------------------------------------
//	Someone wants to know this view's minimum and maximum sizes, probably to help
//	do some type of layout. The base class does nothing, but subclasses should
//	override and do something meaningful here.
//
OSStatus PKMacView::GetSizeConstraints(
	HISize*				outMin,
	HISize*				outMax )
{
#pragma unused( outMin, outMax )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	BoundsChanged
//-----------------------------------------------------------------------------------
//	The bounds of our view have changed. Subclasses can override here to make note
//	of it and flush caches, etc. The base class does nothing.
//
OSStatus PKMacView::BoundsChanged(
	UInt32 				inOptions,
	const HIRect& 		inOriginalBounds,
	const HIRect& 		inCurrentBounds,
	RgnHandle 			inInvalRgn )
{
#pragma unused( inOptions, inOriginalBounds, inCurrentBounds, inInvalRgn )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	ControlHit
//-----------------------------------------------------------------------------------
//	The was hit.  Subclasses can overide to care about what part was hit.
//
OSStatus PKMacView::ControlHit(
	ControlPartCode		inPart,
	UInt32				inModifiers )
{
#pragma unused( inPart, inModifiers )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	ControlClick
//-----------------------------------------------------------------------------------
//	The was clicked.  Subclasses can overide to care about what part was hit.
//
OSStatus PKMacView::ControlClick(EventMouseButton button,
								 int x, int y,
								 UInt32	inModifiers)
{
#pragma unused( inPart, inModifiers )
	
	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	HiliteChanged
//-----------------------------------------------------------------------------------
//	The hilite of our view has changed. Subclasses can override here to make note
//	of it and flush caches, etc. The base class does nothing.
//
OSStatus PKMacView::HiliteChanged(
	ControlPartCode		inOriginalPart,
	ControlPartCode		inCurrentPart,
	RgnHandle 			inInvalRgn )
{
#pragma unused( inOriginalPart, inCurrentPart, inInvalRgn )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	DragEnter
//-----------------------------------------------------------------------------------
//	A drag has entered our bounds. The Drag and Drop interface also should have been
//	activated or else this method will NOT be called. If true is returned, this view
//	likes the drag and will receive drag within/leave/receive messages as appropriate.
//	If false is returned, it is assumed the drag is not valid for this view, and no
//	further drag activity will flow into this view unless the drag leaves and is
//	re-entered.
//
bool PKMacView::DragEnter(
	DragRef				inDrag )
{
#pragma unused( inDrag )

	return false;
}

//-----------------------------------------------------------------------------------
//	DragWithin
//-----------------------------------------------------------------------------------
//	A drag has moved within our bounds. In order for this method to be called, the
//	view must have signaled the drag as being desirable in the DragEnter method. The
//	Drag and Drop interface also should have been activated.
//
bool PKMacView::DragWithin(
	DragRef				inDrag )
{
#pragma unused( inDrag )

	return false;
}

//-----------------------------------------------------------------------------------
//	DragLeave
//-----------------------------------------------------------------------------------
//	A drag has left. Deal with it. Subclasses should override as necessary. The
//	Drag and Drop interface should be activated in order for this method to be valid.
//	The drag must have also been accepted in the DragEnter method, else this method
//	will NOT be called.
//
bool PKMacView::DragLeave(
	DragRef				inDrag )
{
#pragma unused( inDrag )

	return false;
}

//-----------------------------------------------------------------------------------
//	DragReceive
//-----------------------------------------------------------------------------------
//	Deal with receiving a drag. By default we return dragNotAcceptedErr. I'm not sure
//	if this is correct, or eventNotHandledErr. Time will tell...
//
OSStatus PKMacView::DragReceive(
	DragRef				inDrag )
{
#pragma unused( inDrag )

	return dragNotAcceptedErr;
}

//-----------------------------------------------------------------------------------
//	Track
//-----------------------------------------------------------------------------------
//	Default tracking method. Subclasses should override as necessary. We do nothing
//	here in the base class, so we return eventNotHandledErr.
//
OSStatus PKMacView::Track(
	PKCarbonEvent&		inEvent,
	ControlPartCode*	outPart )
{
#pragma unused( inEvent, outPart )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	SetFocusPart
//-----------------------------------------------------------------------------------
//	Handle focusing. Our base behavior is to OK.
//
OSStatus PKMacView::SetFocusPart(
	ControlPartCode		inDesiredFocus,
	RgnHandle			inInvalidRgn,
	Boolean				inFocusEverything,
	ControlPartCode*	outActualFocus )
{
#pragma unused( inDesiredFocus, inInvalidRgn, inFocusEverything, outActualFocus )

	return noErr;
//	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	ProcessCommand
//-----------------------------------------------------------------------------------
//	Process a command. Subclasses should override as necessary.
//
OSStatus PKMacView::ProcessCommand(
	const HICommand&	inCommand )
{
#pragma unused( inCommand )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	UpdateCommandStatus
//-----------------------------------------------------------------------------------
//	Update the status for a command. Subclasses should override as necessary.
//
OSStatus
PKMacView::UpdateCommandStatus(
	const HICommand&	inCommand )
{
#pragma unused( inCommand )

	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	ActivateInterface
//-----------------------------------------------------------------------------------
//	This routine is used to allow a subclass to turn on a specific event or suite of
//	events, like Drag and Drop. This allows us to keep event traffic down if we are
//	not interested, but register for the events if we are.
//
OSStatus PKMacView::ActivateInterface(
	PKMacView::Interface	inInterface )
{
	OSStatus		result = noErr;
	
	switch( inInterface )
	{
		case kDragAndDrop:
			{
				static const EventTypeSpec kDragEvents[] =
				{
					{ kEventClassControl, kEventControlDragEnter },			
					{ kEventClassControl, kEventControlDragLeave },			
					{ kEventClassControl, kEventControlDragWithin },			
					{ kEventClassControl, kEventControlDragReceive }
				};
				
				result = AddEventTypesToHandler( fHandler, GetEventTypeCount( kDragEvents ),
						kDragEvents );
			}
			break;
			
		case kKeyboardFocus:
			{
				static const EventTypeSpec kKeyboardFocusEvents[] =
				{
					{ kEventClassControl,   kEventControlSetFocusPart },
					{ kEventClassKeyboard,  kEventRawKeyDown },
					{ kEventClassKeyboard,  kEventRawKeyRepeat },
					{ kEventClassKeyboard,  kEventRawKeyUp },					
					{ kEventClassTextInput, kEventTextInputUnicodeForKeyEvent }
				};
				
				result = AddEventTypesToHandler( fHandler, GetEventTypeCount( kKeyboardFocusEvents ),
						kKeyboardFocusEvents );
			}
			break;
	}
	
	return result;
}

//-----------------------------------------------------------------------------------
//	RegisterSubclass
//-----------------------------------------------------------------------------------
//	This routine should be called by subclasses so they can be created as HIObjects.
//
OSStatus PKMacView::RegisterSubclass(
	CFStringRef			inID,
	ConstructProc		inProc )
{
	return HIObjectRegisterSubclass( inID, kHIViewClassID, 0, ObjectEventHandler,
			GetEventTypeCount( kHIObjectEvents ), kHIObjectEvents, (void*) inProc, NULL );
}

// Scoll view handling

int PKMacView::getScrollableWidth()
{
	// Reimplement
	return 0;
}

int PKMacView::getScrollableHeight()
{
	// Reimplement
	return 0;
}

int PKMacView::getScrollLineWidth()
{
	// Reimplement
	return 0;
}

int PKMacView::getScrollLineHeight()
{
	// Reimplement
	return 0;
}

void PKMacView::setScrollOrigin(int x, int y)
{
	// Reimplement
}

void PKMacView::focusAcquired(PKDirection dir)
{
	// Reimplement
}

void PKMacView::focusLost()
{
	// Reimplement
}

bool PKMacView::moveFocusToNextControlPart()
{
	// Reimplement
	return false;
}

bool PKMacView::moveFocusToPreviousControlPart()
{
	// Reimplement
	return false;
}

//-----------------------------------------------------------------------------------
//	ObjectEventHandler
//-----------------------------------------------------------------------------------
//	Our static event handler proc. We handle any HIObject based events directly in
// 	here at present.
//
pascal OSStatus PKMacView::ObjectEventHandler(
	EventHandlerCallRef	inCallRef,
	EventRef			inEvent,
	void*				inUserData )
{
	OSStatus			result = eventNotHandledErr;
	PKMacView*				view = (PKMacView*) inUserData;
	PKCarbonEvent		event( inEvent );
	
	switch ( event.GetClass() )
	{
		case kEventClassHIObject:
			switch ( event.GetKind() )
			{
				case kEventHIObjectConstruct:
					{
						ControlRef		control; // ControlRefs are HIObjectRefs
						PKMacView*			view;

						result = event.GetParameter<HIObjectRef>( kEventParamHIObjectInstance,
								typeHIObjectRef, (HIObjectRef*)&control );
						require_noerr( result, ParameterMissing );
						
						// on entry for our construct event, we're passed the
						// creation proc we registered with for this class.
						// we use it now to create the instance, and then we
						// replace the instance parameter data with said instance
						// as type void.

						result = (*(ConstructProc)inUserData)( control, &view );
						if ( result == noErr )
							event.SetParameter<PKMacViewPtr>( kEventParamHIObjectInstance,
									typeVoidPtr, view ); 
					}
					break;
				
				case kEventHIObjectInitialize:
					result = CallNextEventHandler( inCallRef, inEvent );
					if ( result == noErr )
						result = view->Initialize( event );
					break;
				
				case kEventHIObjectDestruct:
					delete view;
					break;
			}
			break;
	}

ParameterMissing:

	return result;
}

//-----------------------------------------------------------------------------------
//	ViewEventHandler
//-----------------------------------------------------------------------------------
//	Our static event handler proc. We handle all non-HIObject events here.
//
pascal OSStatus PKMacView::ViewEventHandler(
	EventHandlerCallRef	inCallRef,
	EventRef			inEvent,
	void*				inUserData )
{
	OSStatus			result;
	PKMacView*				view = (PKMacView*) inUserData;
	PKCarbonEvent		event( inEvent );
	
	result = view->HandleEvent( inCallRef, event );

	return result;
}

//-----------------------------------------------------------------------------------
//	HandleEvent
//-----------------------------------------------------------------------------------
//	Our object's virtual event handler method. I'm not sure if we need this these days.
//	We used to do various things with it, but those days are long gone...
//
OSStatus PKMacView::HandleEvent(
	EventHandlerCallRef	inCallRef,
	PKCarbonEvent&		inEvent )
{
#pragma unused( inCallRef )

	OSStatus			result = eventNotHandledErr;
	HIPoint				where;
	OSType				tag;
	void *				ptr;
	Size				size, outSize;
	UInt32				features;
	RgnHandle			region = NULL;
	ControlPartCode		part;
	RgnHandle			invalRgn;
	
	switch ( inEvent.GetClass() )
	{
		case kEventClassCommand:
			{
				HICommand		command;
				
				result = inEvent.GetParameter( kEventParamDirectObject, &command );
				require_noerr( result, MissingParameter );
				
				switch ( inEvent.GetKind() )
				{
					case kEventCommandProcess:
						result = ProcessCommand( command );
						break;
					
					case kEventCommandUpdateStatus:
						result = UpdateCommandStatus( command );
						break;
				}
			}
			break;

		case kEventClassScrollable:
			switch ( inEvent.GetKind() )
			{
				case kEventScrollableGetInfo:
				{
					HIViewRef userPane = (HIViewRef)this->fViewRef;
					HIRect bounds;
					HIViewGetBounds(userPane, &bounds);
					
					int w = getScrollableWidth();
					int h = getScrollableHeight();

					int lw = getScrollLineWidth();
					int lh = getScrollLineHeight();
					
					HISize lineSize = {lw, lh};
					HISize imageSize = {w, h};
					
					SetEventParameter(inEvent, kEventParamViewSize, typeHISize, sizeof(bounds.size), &bounds.size);
					SetEventParameter(inEvent, kEventParamImageSize, typeHISize, sizeof(imageSize), &imageSize);
					SetEventParameter(inEvent, kEventParamLineSize, typeHISize, sizeof(lineSize), &lineSize);
					SetEventParameter(inEvent, kEventParamOrigin, typeHIPoint, sizeof(bounds.origin), &bounds.origin);
					
					result = noErr;
				}
				break;
					
				case kEventScrollableScrollTo:
					{
						// We need to offset the drawing of our content view and 
						// trigger its redraw based on the offset we are given here
						
						HIPoint where;
						GetEventParameter(inEvent, kEventParamOrigin, typeHIPoint, NULL, sizeof(where), NULL, &where);

						setScrollOrigin(where.x, where.y);
						HIViewSetNeedsDisplay(this->fViewRef, true);
					}	
					break;
			}
			break;
					
		case kEventClassControl:
			switch ( inEvent.GetKind() )
			{
				case kEventControlInitialize:
					features = GetBehaviors();
					inEvent.SetParameter( kEventParamControlFeatures, features );
					result = noErr;
					break;
					
				case kEventControlDraw:
					{
						CGContextRef		context = NULL;
						
						inEvent.GetParameter( kEventParamRgnHandle, &region );
						inEvent.GetParameter<CGContextRef>( kEventParamCGContextRef, typeCGContextRef, &context );

						Draw( region, context );
						result = noErr;
					}
					break;
				
				case kEventControlHitTest:
					inEvent.GetParameter<HIPoint>( kEventParamMouseLocation, typeHIPoint, &where );
					part = HitTest( where );
					inEvent.SetParameter<ControlPartCode>( kEventParamControlPart, typeControlPartCode, part );
					result = noErr;
					break;
					
				case kEventControlGetPartRegion:
					inEvent.GetParameter<ControlPartCode>( kEventParamControlPart, typeControlPartCode, &part );
					inEvent.GetParameter( kEventParamControlRegion, &region );
					result = GetRegion( part, region );
					break;
				
				case kEventControlGetData:
					inEvent.GetParameter<ControlPartCode>( kEventParamControlPart, typeControlPartCode, &part );
					inEvent.GetParameter<OSType>( kEventParamControlDataTag, typeEnumeration, &tag );
					inEvent.GetParameter<Ptr>( kEventParamControlDataBuffer, typePtr, (Ptr*)&ptr );
					inEvent.GetParameter<Size>( kEventParamControlDataBufferSize, typeLongInteger, &size );

					result = GetData( tag, part, size, &outSize, ptr );

					if ( result == noErr )
						verify_noerr( inEvent.SetParameter<Size>( kEventParamControlDataBufferSize, typeLongInteger, outSize ) );
					break;
				
				case kEventControlSetData:
					inEvent.GetParameter<ControlPartCode>( kEventParamControlPart, typeControlPartCode, &part );
					inEvent.GetParameter<OSType>( kEventParamControlDataTag, typeEnumeration, &tag );
					inEvent.GetParameter<Ptr>( kEventParamControlDataBuffer, typePtr, (Ptr*)&ptr );
					inEvent.GetParameter<Size>( kEventParamControlDataBufferSize, typeLongInteger, &size );

					result = SetData( tag, part, size, ptr );
					break;
				
				case kEventControlGetOptimalBounds:
					{
						HISize		size;
						float		floatBaseLine;
						
						result = GetOptimalSize( &size, &floatBaseLine );
						if ( result == noErr )
						{
							Rect		bounds;
							SInt16		baseLine;

							GetControlBounds( GePKMacViewRef(), &bounds );

							bounds.bottom = bounds.top + (SInt16)size.height;
							bounds.right = bounds.left + (SInt16)size.width;
							baseLine = (SInt16)floatBaseLine;
							
							inEvent.SetParameter( kEventParamControlOptimalBounds, bounds );
							inEvent.SetParameter<SInt16>( kEventParamControlOptimalBaselineOffset, typeShortInteger, baseLine );
						}
					}
					break;
				
				case kEventControlBoundsChanged:
					{
						HIRect		prevRect, currRect;
						UInt32		attrs;
						
						inEvent.GetParameter( kEventParamAttributes, &attrs );
						inEvent.GetParameter( kEventParamOriginalBounds, &prevRect );
						inEvent.GetParameter( kEventParamCurrentBounds, &currRect );
						inEvent.GetParameter( kEventParamControlInvalRgn, &invalRgn );

						result = BoundsChanged( attrs, prevRect, currRect, invalRgn );
						
						// Signal to parent to update scroll bars if needed
						
						HIViewRef parentView = HIViewGetSuperview(this->fViewRef);
						
						if (parentView)
						{
							EventRef theEvent;
							CreateEvent(NULL, kEventClassScrollable,
										kEventScrollableInfoChanged, 
										GetCurrentEventTime(),
										kEventAttributeUserEvent, 
										&theEvent);
							SendEventToEventTarget(theEvent, GetControlEventTarget(parentView));
							ReleaseEvent(theEvent);
						}
						
					}
					break;

				case kEventControlHit:
					{
						UInt32		modifiers;
						
						inEvent.GetParameter<ControlPartCode>( kEventParamControlPart, typeControlPartCode, &part );
						inEvent.GetParameter( kEventParamKeyModifiers, &modifiers );
	
						result = ControlHit( part, modifiers );
					}
					break;
				
				case kEventControlClick:
				{
					ControlRef control;
					EventMouseButton button;
					HIPoint     mousePt;
					UInt32		modifiers;
					
					inEvent.GetParameter<ControlRef>(kEventParamDirectObject, typeControlRef, &control);
					inEvent.GetParameter<ControlPartCode>( kEventParamControlPart, typeControlPartCode, &part );
					inEvent.GetParameter( kEventParamKeyModifiers, &modifiers );
					
					inEvent.GetParameter<HIPoint>(kEventParamMouseLocation, typeHIPoint, &mousePt);
					inEvent.GetParameter<EventMouseButton>(kEventParamMouseButton, typeMouseButton, &button);
					
					// Compute local coordinates
					
					Point   localMouse;
					GrafPtr currPort;	
					
					// Convert the global mouse to dialog relative
					
					GetPort(&currPort);
					SetPortWindowPort(GetControlOwner(control));
					
					localMouse.h = mousePt.x;
					localMouse.v = mousePt.y;

					GlobalToLocal(&localMouse);

					SetPort(currPort);
					
					HIPoint point;
					point.x = 0;
					point.y = 0;
					
					WindowRef window = GetControlOwner(control);
					HIViewRef mContent;
					
					HIViewFindByID(HIViewGetRoot(window),
								   kHIViewWindowContentID, &mContent);
					
					HIViewConvertPoint(&point, control, mContent);
					
					localMouse.h -= point.x;
					localMouse.v -= point.y;
						
					result = ControlClick(button, localMouse.h, localMouse.v, modifiers);
				}
					break;

				case kEventControlHiliteChanged:
					{
						ControlPartCode	prevPart, currPart;
						
						inEvent.GetParameter<ControlPartCode>( kEventParamControlPreviousPart, typeControlPartCode, &prevPart );
						inEvent.GetParameter<ControlPartCode>( kEventParamControlCurrentPart, typeControlPartCode, &currPart );
						inEvent.GetParameter( kEventParamControlInvalRgn, &invalRgn );

						result = HiliteChanged( prevPart, currPart, invalRgn );
						
						if ( GetAutoInvalidateFlags() & kAutoInvalidateOnHilite )
							Invalidate();
					}
					break;
					
				case kEventControlActivate:
					result = ActiveStateChanged();

					if ( GetAutoInvalidateFlags() & kAutoInvalidateOnActivate )
						Invalidate();
					break;
					
				case kEventControlDeactivate:
					result = ActiveStateChanged();

					if ( GetAutoInvalidateFlags() & kAutoInvalidateOnActivate )
						Invalidate();
					break;
					
				case kEventControlValueFieldChanged:
					result = ValueChanged();

					if ( GetAutoInvalidateFlags() & kAutoInvalidateOnValueChange )
						Invalidate();
					break;
					
				case kEventControlTitleChanged:
					result = TitleChanged();

					if ( GetAutoInvalidateFlags() & kAutoInvalidateOnTitleChange )
						Invalidate();
					break;
					
				case kEventControlEnabledStateChanged:
					result = EnabledStateChanged();

					if ( GetAutoInvalidateFlags() & kAutoInvalidateOnEnable )
						Invalidate();
					break;
					
				case kEventControlDragEnter:
				case kEventControlDragLeave:
				case kEventControlDragWithin:
					{
						DragRef		drag;
						bool		likesDrag;
						
						inEvent.GetParameter( kEventParamDragRef, &drag );

						switch ( inEvent.GetKind() )
						{
							case kEventControlDragEnter:
								likesDrag = DragEnter( drag );
								
								// Why only if likesDrag?  What if it doesn't?  No parameter?
								if ( likesDrag )
									result = inEvent.SetParameter( kEventParamControlLikesDrag, likesDrag );
								break;
							
							case kEventControlDragLeave:
								DragLeave( drag );
								result = noErr;
								break;
							
							case kEventControlDragWithin:
								DragWithin( drag );
								result = noErr;
								break;
						}
					}
					break;
				
				case kEventControlDragReceive:
					{
						DragRef		drag;
						
						inEvent.GetParameter( kEventParamDragRef, &drag );

						result = DragReceive( drag );
					}
					break;
				
				case kEventControlTrack:
					{
						ControlPartCode		part;
						
						result = Track( inEvent, &part );
						if ( result == noErr )
							verify_noerr( inEvent.SetParameter<ControlPartCode>( kEventParamControlPart, typeControlPartCode, part ) );
					}
					break;

				case kEventControlGetSizeConstraints:
					{
						HISize		minSize, maxSize;
						
						result = GetSizeConstraints( &minSize, &maxSize );

						if ( result == noErr )
						{
							verify_noerr( inEvent.SetParameter( kEventParamMinimumSize, minSize ) );
							verify_noerr( inEvent.SetParameter( kEventParamMaximumSize, maxSize ) );
						}
					}
					break;

				case kEventControlSetFocusPart:
					{
						OSStatus            err = noErr;
						ControlPartCode		desiredFocus;
						RgnHandle			invalidRgn;
						Boolean				focusEverything;
						ControlPartCode		actualFocus;
						
						result = inEvent.GetParameter<ControlPartCode>(kEventParamControlPart, typeControlPartCode, &desiredFocus); 

						focusEverything = false; // a good default in case the parameter doesn't exist
						inEvent.GetParameter( kEventParamControlFocusEverything, &focusEverything );
						
						if((desiredFocus == kControlFocusNextPart)) 
						{
							if(!haveFocus)
							{
								haveFocus    = true;
								focusAcquired(PK_DIR_FORWARD);
								_focusedPart = desiredFocus;
							}
							else
							{
								// We already have focus, let's ask
								// our control if it has more parts
								// to focus on
								
								if(moveFocusToNextControlPart())
								{
									haveFocus    = true;
									_focusedPart = desiredFocus;
								}
								else
								{
									haveFocus = false;
									_focusedPart = kControlFocusNoPart;
									focusLost();
								}
							}
						}
						else if((desiredFocus == kControlFocusPrevPart))
						{
							if(!haveFocus)
							{
								haveFocus    = true;
								focusAcquired(PK_DIR_BACKWARD);
								_focusedPart = desiredFocus;
							}
							else
							{
								// We already have focus, let's ask
								// our control if it has more parts
								// to focus on
								
								if(moveFocusToPreviousControlPart())
								{
									haveFocus    = true;
									_focusedPart = desiredFocus;
								}
								else
								{
									haveFocus = false;
									_focusedPart = kControlFocusNoPart;
									focusLost();
								}
							}
						}
						else if(desiredFocus == kControlFocusNoPart)
						{
							haveFocus = false;
							focusLost();
							_focusedPart = desiredFocus;
						}
						
						if(!err) 
						{
							SetEventParameter(inEvent, kEventParamControlPart,
											  typeControlPartCode, sizeof(_focusedPart),
											  &_focusedPart);
						}
							
						inEvent.GetParameter( kEventParamControlInvalRgn, &invalidRgn );
							
						SetFocusPart( desiredFocus, invalidRgn, focusEverything, &actualFocus );

						result = err;
					}
					break;
				
				// some other kind of Control event
				default:
					break;
			}
			break;
			
		case kEventClassTextInput:
			TextInput( inEvent );
			result = eventNotHandledErr;
			break;
			
		case kEventClassKeyboard:
			{
				if(inEvent.GetKind() == kEventRawKeyDown)
				{
					KeyDown(inEvent);
				}
				
				if(inEvent.GetKind() == kEventRawKeyRepeat)
				{
					KeyDown(inEvent);
				}
				
				if(inEvent.GetKind() == kEventRawKeyUp)
				{
					KeyUp(inEvent);
				}

				// Always return this so that raw keys are processed
				// by the default handler, which will interpret them
				return eventNotHandledErr;
			}
			break;
			
		// some other event class
		default:
			break;
	}

MissingParameter:
	return result;
}

//-----------------------------------------------------------------------------------
//	CreateInitializationEvent
//-----------------------------------------------------------------------------------
// 	Create a basic intialization event containing the parent control and bounds. At
//	present we set the bounds to empty and the parent to NULL. In theory, after creating
//	this event, any subclass could add its own parameter to receive in its
//	Initialize method.
//
EventRef PKMacView::CreateInitializationEvent()
{
	OSStatus		result = noErr;
	EventRef		event;

	result = CreateEvent( NULL, kEventClassHIObject, kEventHIObjectInitialize,
					GetCurrentEventTime(), 0, &event );
	require_noerr_action( result, CantCreateEvent, event = NULL );
		
CantCreateEvent:
	return event;
}

//-----------------------------------------------------------------------------------
//	Frame
//-----------------------------------------------------------------------------------
//
HIRect PKMacView::Frame()
{
	HIRect		frame;

	HIViewGetFrame( GePKMacViewRef(), &frame );
	
	return frame;
}

//-----------------------------------------------------------------------------------
//	SetFrame
//-----------------------------------------------------------------------------------
//
OSStatus PKMacView::SetFrame(
	const HIRect&			inFrame )
{
	OSStatus				err;
	
	err = HIViewSetFrame( GePKMacViewRef(), &inFrame );
	
	return err;
}

//-----------------------------------------------------------------------------------
//	Bounds
//-----------------------------------------------------------------------------------
//
HIRect PKMacView::Bounds()
{
	HIRect		bounds;
	
	HIViewGetBounds( GePKMacViewRef(), &bounds );
	
	return bounds;
}

//-----------------------------------------------------------------------------------
//	Show
//-----------------------------------------------------------------------------------
//
OSStatus PKMacView::Show()
{
	return HIViewSetVisible( GePKMacViewRef(), true );
}

//-----------------------------------------------------------------------------------
//	Hide
//-----------------------------------------------------------------------------------
//
OSStatus PKMacView::Hide()
{
	return HIViewSetVisible( GePKMacViewRef(), false );
}

//-----------------------------------------------------------------------------------
//	GetEventTarget
//-----------------------------------------------------------------------------------
//
EventTargetRef PKMacView::GetEventTarget()
{
	return HIObjectGetEventTarget( (HIObjectRef) GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	AddSubView
//-----------------------------------------------------------------------------------
//
OSStatus PKMacView::AddSubView(
	PKMacView*				inSubView )
{
	return HIViewAddSubview( GePKMacViewRef(), inSubView->GePKMacViewRef() );;
}

//-----------------------------------------------------------------------------------
//	RemoveFromSuperView
//-----------------------------------------------------------------------------------
//
OSStatus PKMacView::RemoveFromSuperView()
{
	return HIViewRemoveFromSuperview( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	GetHilite
//-----------------------------------------------------------------------------------
//
ControlPartCode PKMacView::GetHilite()
{
	return GetControlHilite( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	GetValue
//-----------------------------------------------------------------------------------
//
SInt32 PKMacView::GetValue()
{
	return GetControl32BitValue( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	SetValue
//-----------------------------------------------------------------------------------
//
void PKMacView::SetValue(
	SInt32					inValue )
{
	SetControl32BitValue( GePKMacViewRef(), inValue );
}

//-----------------------------------------------------------------------------------
//	GetMinimum
//-----------------------------------------------------------------------------------
//
SInt32 PKMacView::GetMinimum()
{
	return GetControl32BitMinimum( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	SetMinimum
//-----------------------------------------------------------------------------------
//
void PKMacView::SetMinimum(
	SInt32					inMinimum )
{
	SetControl32BitMinimum( GePKMacViewRef(), inMinimum );
}

//-----------------------------------------------------------------------------------
//	GetMaximum
//-----------------------------------------------------------------------------------
//
SInt32 PKMacView::GetMaximum()
{
	return GetControl32BitMaximum( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	SetMaximum
//-----------------------------------------------------------------------------------
//
void PKMacView::SetMaximum(
	SInt32					inMaximum )
{
	SetControl32BitMaximum( GePKMacViewRef(), inMaximum );
}

//-----------------------------------------------------------------------------------
//	GetOwner
//-----------------------------------------------------------------------------------
//
WindowRef PKMacView::GetOwner()
{
	return GetControlOwner( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	Hilite
//-----------------------------------------------------------------------------------
//
void PKMacView::Hilite(
	ControlPartCode			inPart)
{
	return HiliteControl( GePKMacViewRef(), inPart );
}

//-----------------------------------------------------------------------------------
//	Invalidate
//-----------------------------------------------------------------------------------
//
OSStatus PKMacView::Invalidate()
{
	return HIViewSetNeedsDisplay( GePKMacViewRef(), true );
}

//-----------------------------------------------------------------------------------
//	IsVisible
//-----------------------------------------------------------------------------------
//
Boolean PKMacView::IsVisible()
{
	return IsControlVisible( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	IsEnabled
//-----------------------------------------------------------------------------------
//
Boolean PKMacView::IsEnabled()
{
	return IsControlEnabled( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	IsActive
//-----------------------------------------------------------------------------------
//
Boolean PKMacView::IsActive()
{
	return IsControlActive( GePKMacViewRef() );
}

//-----------------------------------------------------------------------------------
//	ActiveStateChanged
//-----------------------------------------------------------------------------------
//	Default activation method. Subclasses should override as necessary. We do nothing
//	here in the base class, so we return eventNotHandledErr.
//
OSStatus PKMacView::ActiveStateChanged()
{
	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	ValueChanged
//-----------------------------------------------------------------------------------
//	Default value changed method. Subclasses should override as necessary. We do
//	nothing here in the base class, so we return eventNotHandledErr.
//
OSStatus PKMacView::ValueChanged()
{
	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	TitleChanged
//-----------------------------------------------------------------------------------
//	Default title changed method. Subclasses should override as necessary. We
//	do nothing here in the base class, so we return eventNotHandledErr.
//
OSStatus PKMacView::TitleChanged()
{
	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	EnabledStateChanged
//-----------------------------------------------------------------------------------
//	Default enable method. Subclasses should override as necessary. We
//	do nothing here in the base class, so we return eventNotHandledErr.
//
OSStatus PKMacView::EnabledStateChanged()
{
	return eventNotHandledErr;
}

//-----------------------------------------------------------------------------------
//	TextInput
//-----------------------------------------------------------------------------------
//	Default text (Unicode) input method. Subclasses should override as necessary. We
//	do nothing here in the base class, so we return eventNotHandledErr.
//
OSStatus PKMacView::TextInput(
	PKCarbonEvent&		inEvent )
{
#pragma unused( inEvent )

	return eventNotHandledErr;
}

OSStatus PKMacView::KeyDown(PKCarbonEvent& inEvent)
{
#pragma unused( inEvent )
	return eventNotHandledErr;
}

OSStatus PKMacView::KeyUp(PKCarbonEvent& inEvent)
{
#pragma unused( inEvent )
	return eventNotHandledErr;	
}

//-----------------------------------------------------------------------------------
//	ChangeAutoInvalidateFlags
//-----------------------------------------------------------------------------------
//	Change behavior for auto-invalidating views on certain actions.
//
void PKMacView::ChangeAutoInvalidateFlags(
	OptionBits			inSetThese,
	OptionBits			inClearThese )
{
    fAutoInvalidateFlags = ( ( fAutoInvalidateFlags | inSetThese ) & ( ~inClearThese ) );
}

