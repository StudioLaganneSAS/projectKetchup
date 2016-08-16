//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKMacSegmentedView.h"
#include "PKStr.h"
#include "PKVectorEngine.h"
#include "PKDialog.h"

// -----------------------------------------------------------------------------
//	constants
// -----------------------------------------------------------------------------
//

#define kTViewPKMacSegmentedViewClassID	CFSTR( "com.projectKetchup.SegmentedView" )

// -----------------------------------------------------------------------------
//	HIPKMacCustomViewCreate
// -----------------------------------------------------------------------------
//	High-level API for creating a color swatch control.
//

OSStatus HIPKMacSegmentedViewCreate(WindowRef		inWindow,
								    const HIRect*	inBounds,
								    PKToolbarSegmentedItem *sitem,
								    ControlRef*		outControl)
{
	return PKMacSegmentedView::Create(inWindow, inBounds, sitem, outControl);
}


// -----------------------------------------------------------------------------
//	PKMacCustomView constructor
// -----------------------------------------------------------------------------
//
PKMacSegmentedView::PKMacSegmentedView(
	HIViewRef			inControl )
	:	PKMacView( inControl )
{
	this->sitem     = NULL;
	this->overIndex = -1;
	this->downIndex = -1;

	this->fHelpContentProviderUPP = NewHMControlContentUPP(PKMacSegmentedView::HelpContentProvider);
	HMInstallControlContentCallback(inControl, this->fHelpContentProviderUPP);
}

// -----------------------------------------------------------------------------
//	PKMacCustomView destructor
// -----------------------------------------------------------------------------
//
PKMacSegmentedView::~PKMacSegmentedView()
{
}

// -----------------------------------------------------------------------------
//	Create
// -----------------------------------------------------------------------------
//	Public class method for creating a PKMacCustomView
//
OSStatus PKMacSegmentedView::Create(
	WindowRef		 inWindow,
	const HIRect*	 inBounds,
	PKToolbarSegmentedItem *sitem,
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
		PKMacView::RegisterSubclass( kTViewPKMacSegmentedViewClassID, PKMacSegmentedView::Construct );
		sRegistered = true;
	}
	
	// instantiate the object
	err = HIObjectCreate( kTViewPKMacSegmentedViewClassID, event, (HIObjectRef*) outControl );
	
	// add view to parent view if needed
	if( err == noErr )
	{
		if( inWindow != NULL )
		{
			GetRootControl( inWindow, &root );
			HIViewAddSubview( root, *outControl );
		}
		
		HIViewSetFrame( *outControl, inBounds );
		
		SetControlReference(*outControl, (SInt32) sitem);
	}

	ReleaseEvent( event );
	
CantCreateEvent:
	
	return err;
}

// -----------------------------------------------------------------------------
//	GetKind
// -----------------------------------------------------------------------------
//
ControlKind PKMacSegmentedView::GetKind()
{
	const ControlKind kPKMacSegmentedViewKind = { 'pksv', 'pksv' };
	
	return kPKMacSegmentedViewKind;
}

// -----------------------------------------------------------------------------
//	Construct
// -----------------------------------------------------------------------------
//
OSStatus PKMacSegmentedView::Construct(
	ControlRef		inControl,
	PKMacView**			outView )
{
	PKMacSegmentedView *macView = new PKMacSegmentedView(inControl);
	*outView = macView;
	
	return noErr;
}


// -----------------------------------------------------------------------------
//	Draw
// -----------------------------------------------------------------------------
//	Draw your view. You should draw based on VIEW coordinates, not frame
//	coordinates.
//
void PKMacSegmentedView::Draw(
	RgnHandle			inLimitRgn,
	CGContextRef		inContext )
{
	if(this->sitem == NULL)
	{
		SInt32 value  = GetControlReference(this->fViewRef);
		this->sitem = (PKToolbarSegmentedItem *) value;
	}
	
	if(this->sitem != NULL && sitem->normalImage != NULL)
	{
		PKVectorEngine engine;
		engine.startWithCGContext(inContext);
		engine.setDimensions(sitem->normalImage->getWidth(), 
							 sitem->normalImage->getHeight());
		
		int32_t selection = PKVALUE_INT32(this->sitem->get(PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY));
		std::vector<std::string> offsets = PKVALUE_STRINGLIST(this->sitem->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY));
		
		int x = 0;
		int y = 0;
		
		for(uint32_t n=0; n<offsets.size()-1; n++)
		{
			int32_t offset1 = PKStr::stringToInt32(offsets[n]);
			int32_t offset2 = PKStr::stringToInt32(offsets[n+1]);
			
			if(n == selection)
			{
				// Draw down
				engine.drawImageSubImageWithOffset(this->sitem->downImage, x+offset1, y, offset1, 0, offset2 - offset1);
			}
			else
			{
				// Draw down, normal or over
				
				if(this->downIndex == n) // down
				{
					engine.drawImageSubImageWithOffset(this->sitem->downImage, x+offset1, y, offset1, 0, offset2 - offset1);
				}
				else
				{
					if(this->overIndex == n) // hover
					{
						engine.drawImageSubImageWithOffset(this->sitem->hoverImage, x+offset1, y, offset1, 0, offset2 - offset1);
					}
					else // normal
					{
						engine.drawImageSubImageWithOffset(this->sitem->normalImage, x+offset1, y, offset1, 0, offset2 - offset1);
					}
				}
			}
		}

		engine.end();
	}
}

OSStatus PKMacSegmentedView::ControlClick(EventMouseButton button,
										  int x, int y,
										  UInt32 inModifiers)
{
	if(this->sitem != NULL && button == kEventMouseButtonPrimary)
	{
		// See where it hit
		
		std::vector<std::string> offsets = PKVALUE_STRINGLIST(this->sitem->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY));
		int32_t selection = PKVALUE_INT32(this->sitem->get(PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY));
		
		for(int32_t k=0; k<offsets.size()-1; k++)
		{
			int off1 = PKStr::stringToInt32(offsets[k]);
			int off2 = PKStr::stringToInt32(offsets[k+1]);
			
			if((x >= off1) && 
			   (x <= off2))
			{
				PKVariant *i   = this->sitem->get(PKToolbarSegmentedItem::ID_STRING_PROPERTY);
				std::string id = PKVALUE_STRING(i);
				
				if(k != selection)
				{
					PKVariantInt32 newSel(k);
					this->sitem->set(PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY, &newSel);
					
					this->sitem->dialog->toolbarSegmentedItemSwitched(NULL, this->sitem, id, k);

					// Invalidate
					Invalidate();
				}
				
				break;
			}
		}
		
	}	
	
	return noErr;
}

// For the tooltips

pascal OSStatus PKMacSegmentedView::HelpContentProvider(ControlRef inControl, 
											            Point inGlobalMouse, 
											            HMContentRequest inRequest, 
											            HMContentProvidedType *outContentProvided, 
											            HMHelpContentPtr ioHelpContent)
{
    Point   localMouse;
    GrafPtr currPort;	
	
    if(inRequest != kHMSupplyContent)
	{
        return noErr;
	}
	
    // Convert the global mouse to dialog relative
	
    GetPort(&currPort);
    SetPortWindowPort(GetControlOwner(inControl));
    
    localMouse = inGlobalMouse;
    GlobalToLocal(&localMouse);
    
	// Convert to control coordinates
	
	HIPoint point;
	point.x = 0;
	point.y = 0;
	
	WindowRef window = GetControlOwner(inControl);
	HIViewRef mContent;
	
	HIViewFindByID(HIViewGetRoot(window),
				   kHIViewWindowContentID, &mContent);		
	HIViewConvertPoint(&point, inControl, mContent);
	
	localMouse.h -= point.x;
	localMouse.v -= point.y;
	
    // Assume we have no content
    *outContentProvided = kHMContentNotProvided;	
	
	// Get hold of the item
	
	SInt32 value  = GetControlReference(inControl);
	PKToolbarSegmentedItem *sitem = (PKToolbarSegmentedItem *) value;
	
	if(sitem == NULL || sitem->normalImage == NULL)
	{
		return noErr;
	}
	
	// See if we match inside any of the 
	// control's own zones
	
	PKVariant *tt = sitem->get(PKToolbarSegmentedItem::HELP_WSTRINGLIST_PROPERTY);
	std::vector<std::wstring> tooltips = PKVALUE_WSTRINGLIST(tt);
	
	PKVariant *of = sitem->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY);
	std::vector<std::string> offsets = PKVALUE_STRINGLIST(of);
	
	if(tooltips.size() != offsets.size() - 1)
	{
		return noErr;
	}	
	
	for(uint32_t i=0; i < offsets.size()-1; i++)
	{
		int off1 = PKStr::stringToInt32(offsets[i]);
		int off2 = PKStr::stringToInt32(offsets[i+1]);
		
		if((localMouse.h >= off1) && (localMouse.h <= off2))
		{
			std::wstring wtip = tooltips[i];
			CFStringRef cfTip = PKStr::wStringToCFString(wtip);
			
			ioHelpContent->version = kMacHelpVersion; 
			ioHelpContent->tagSide = kHMDefaultSide; 
			
			PKRect rect;
			rect.x = off1;
			rect.y = 0;
			rect.w = off2 - off1;
			rect.h = sitem->normalImage->getWidth();
			
			Point pt;
			pt.h = rect.x+point.x;
			pt.v = rect.y+point.y-18;
			LocalToGlobal(&pt);
			
			SetRect(&(ioHelpContent->absHotRect), pt.h, pt.v, pt.h+rect.w, pt.v+rect.h);
			
			ioHelpContent->content[kHMMinimumContentIndex].contentType   = kHMCFStringLocalizedContent; 
			ioHelpContent->content[kHMMinimumContentIndex].u.tagCFString = cfTip;
			ioHelpContent->content[kHMMaximumContentIndex].contentType   = kHMCFStringLocalizedContent; 
			ioHelpContent->content[kHMMaximumContentIndex].u.tagCFString = cfTip;
			
			*outContentProvided = kHMContentProvided;
			
			break;
		}
	}
	
	SetPort(currPort);
	
	return noErr;
}


