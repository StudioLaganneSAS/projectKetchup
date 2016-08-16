//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKMacCustomView.h"
#include "PKCustomControl.h"
#include "PKStr.h"

// -----------------------------------------------------------------------------
//	constants
// -----------------------------------------------------------------------------
//
#define kTViewPKMacCustomViewClassID	CFSTR( "com.ProjectKetchup.CustomControl" )

// -----------------------------------------------------------------------------
//	HIPKMacCustomViewCreate
// -----------------------------------------------------------------------------
//	High-level API for creating a color swatch control.
//
OSStatus HIPKMacCustomViewCreate(WindowRef		inWindow,
								 const HIRect*	inBounds,
								 PKCustomControl    *control,
								 ControlRef*		outControl)
{
	return PKMacCustomView::Create( inWindow, inBounds, control, outControl );
}


// -----------------------------------------------------------------------------
//	PKMacCustomView constructor
// -----------------------------------------------------------------------------
//
PKMacCustomView::PKMacCustomView(
	HIViewRef			inControl )
	:	PKMacView( inControl )
{
	this->activated = false;
	this->control   = NULL;
}

// -----------------------------------------------------------------------------
//	PKMacCustomView destructor
// -----------------------------------------------------------------------------
//
PKMacCustomView::~PKMacCustomView()
{
}

// -----------------------------------------------------------------------------
//	Create
// -----------------------------------------------------------------------------
//	Public class method for creating a PKMacCustomView
//
OSStatus PKMacCustomView::Create(
	WindowRef		 inWindow,
	const HIRect*	 inBounds,
	PKCustomControl *control,
	ControlRef*		 outControl )
{
	OSStatus	err = noErr;
	ControlRef	root;
	EventRef	event = PKMacView::CreateInitializationEvent(); // create initialization event
	
	require_action( event != NULL, CantCreateEvent, err = eventInternalErr );
	
	// register the subclass
	static bool sRegistered = false;
	if( !sRegistered )
	{
		PKMacView::RegisterSubclass( kTViewPKMacCustomViewClassID, PKMacCustomView::Construct );
		sRegistered = true;
	}
	
	// instantiate the object
	err = HIObjectCreate( kTViewPKMacCustomViewClassID, event, (HIObjectRef*) outControl );
	
	// add view to parent view
	if( err == noErr )
	{
		if( inWindow != NULL )
		{
			GetRootControl( inWindow, &root );
			HIViewAddSubview( root, *outControl );
		}
		
		HIViewSetFrame( *outControl, inBounds );
		
		SetControlReference(*outControl, (SInt32) control);
	}

	ReleaseEvent( event );
	
CantCreateEvent:
	
	return err;
}

// -----------------------------------------------------------------------------
//	GetKind
// -----------------------------------------------------------------------------
//
ControlKind PKMacCustomView::GetKind()
{
	const ControlKind kPKMacCustomViewKind = { 'pkcc', 'pkcc' };
	
	return kPKMacCustomViewKind;
}

// -----------------------------------------------------------------------------
//	Construct
// -----------------------------------------------------------------------------
//
OSStatus PKMacCustomView::Construct(
	ControlRef		inControl,
	PKMacView**			outView )
{
	PKMacCustomView *macView = new PKMacCustomView( inControl );
	*outView = macView;
	
	return noErr;
}


// -----------------------------------------------------------------------------
//	Draw
// -----------------------------------------------------------------------------
//	Draw your view. You should draw based on VIEW coordinates, not frame
//	coordinates.
//
void PKMacCustomView::Draw(
	RgnHandle			inLimitRgn,
	CGContextRef		inContext )
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		if(!this->activated)
		{
			this->activated = true;
			
			PKVariant *key = this->control->get(PKCustomControl::WANTFOCUS_BOOL_PROPERTY);
			bool wantFocus = PKVALUE_BOOL(key);
				
			if(wantFocus)
			{
				this->ActivateInterface(kKeyboardFocus);
			}
		}
		
		this->control->drawMacOSXInternal(inLimitRgn, inContext);
	}
}

OSStatus PKMacCustomView::TextInput(PKCarbonEvent &inEvent)
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		OSStatus err = noErr;
		EventParamType actualType = typeUnicodeText;
		UInt32 actualSize = 0;
		size_t actualCount = 0;
		UniChar *buffer = NULL;
		
		// Get the size of the unicode data
		err = GetEventParameter (inEvent, kEventParamTextInputSendText, typeUnicodeText, &actualType, 0, &actualSize, NULL);

		if(err == noErr)
		{
			// Allocate a buffer and get the actual data.
			
			actualCount = actualSize / sizeof(UniChar);
			buffer = new UniChar[actualCount];
			
			err = GetEventParameter (inEvent, kEventParamTextInputSendText, typeUnicodeText, &actualType, actualSize, &actualSize, buffer);
			
			if(err == noErr)
			{
				std::wstring wc = PKStr::uniCharToWString(buffer, actualCount);				
				this->control->characterInput(wc);
			}
		}		
	}
	
	return noErr;
}

OSStatus PKMacCustomView::KeyDown(PKCarbonEvent& inEvent)
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		UInt32 keyCode;
		unsigned char charCode;
		UInt32 modifiers;
		
		//  Extract the key code parameter (kEventParamKeyCode).
		
		GetEventParameter(inEvent, kEventParamKeyCode, 
						  typeUInt32, nil, sizeof (keyCode),
						  nil, &keyCode );
		
		//  Extract the character code parameter (kEventParamKeyMacCharCodes).
		
		GetEventParameter(inEvent, kEventParamKeyMacCharCodes, typeChar, nil,
						  sizeof( charCode ), nil, &charCode );
		
		//  Extract the modifiers parameter (kEventParamKeyModifiers).
		
		GetEventParameter(inEvent, kEventParamKeyModifiers, typeUInt32, nil,
						  sizeof( modifiers ), nil, &modifiers );     
		
		unsigned int modifier  = PK_MOD_NONE;
		PKKey key = PK_KEY_UNKNOWN;
		unsigned int vcode = 0;
		
		EventRecord rec;
		
		rec.message   = charCode | (keyCode << 8);
		rec.modifiers = modifiers;
		
		PKUIComputeKeyState(&rec, &modifier, &key, &vcode);
		
		// TODO: Handle the TAB key
		
		
		
		// And forward
		
		this->control->keyDown(modifier, key, vcode);
	}		
	
	return noErr;
}

OSStatus PKMacCustomView::KeyUp(PKCarbonEvent& inEvent)
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		UInt32 keyCode;
		unsigned char charCode;
		UInt32 modifiers;
		
		//  Extract the key code parameter (kEventParamKeyCode).
		
		GetEventParameter(inEvent, kEventParamKeyCode, 
						  typeUInt32, nil, sizeof (keyCode),
						  nil, &keyCode );
		
		//  Extract the character code parameter (kEventParamKeyMacCharCodes).
		
		GetEventParameter(inEvent, kEventParamKeyMacCharCodes, typeChar, nil,
						  sizeof( charCode ), nil, &charCode );
		
		//  Extract the modifiers parameter (kEventParamKeyModifiers).
		
		GetEventParameter(inEvent, kEventParamKeyModifiers, typeUInt32, nil,
						  sizeof( modifiers ), nil, &modifiers );     
		
		unsigned int modifier  = PK_MOD_NONE;
		PKKey key = PK_KEY_UNKNOWN;
		unsigned int vcode = 0;
		
		EventRecord rec;
		
		rec.message   = charCode | (keyCode << 8);
		rec.modifiers = modifiers;
		
		PKUIComputeKeyState(&rec, &modifier, &key, &vcode);
		
		this->control->keyUp(modifier, key, vcode);
	}		
	
	return noErr;
}

OSStatus PKMacCustomView::SetFocusPart(ControlPartCode	inDesiredFocus,
									   RgnHandle			inNnvalidRgn,
									   Boolean			inFocusEverything,
									   ControlPartCode*	outActualFocus)
{
	return noErr;
}

void PKMacCustomView::focusAcquired(PKDirection dir)
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		this->control->focusAcquired(dir);
	}		
}

void PKMacCustomView::focusLost()
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		this->control->focusLost();
	}			
}

bool PKMacCustomView::moveFocusToNextControlPart()
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		return this->control->moveFocusToNextControlPart();
	}				
}

bool PKMacCustomView::moveFocusToPreviousControlPart()
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		return this->control->moveFocusToPreviousControlPart();
	}				
}

int PKMacCustomView::getScrollableWidth()
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		return this->control->getScrollableWidth();
	}	
	
	return 0;
}

int PKMacCustomView::getScrollableHeight()
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		return this->control->getScrollableHeight();
	}
	
	return 0;
}

int PKMacCustomView::getScrollLineWidth()
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		return this->control->getScrollLineWidth();
	}	
	
	return 0;
}

int PKMacCustomView::getScrollLineHeight()
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		return this->control->getScrollLineHeight();
	}
	
	return 0;
}

void PKMacCustomView::setScrollOrigin(int x, int y)
{
	if(this->control == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->control = (PKCustomControl *) value;
	}
	
	if(this->control != NULL)
	{
		this->control->setScrollOrigin(x, y);
	}	
}



