//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKMacSegmentedToolbarItem.h"
#include "PKMacSegmentedView.h"
#include "PKStr.h"

// Class ID

#define kSegmentedToolbarItemClassID CFSTR("com.projectKetchup.segmentedtoolbaritem")

// Globals

static bool sRegistered = false;

const EventTypeSpec segmentedEvents[] = {

	{ kEventClassHIObject,    kEventHIObjectConstruct }, 
	{ kEventClassHIObject,    kEventHIObjectInitialize }, 
	{ kEventClassHIObject,    kEventHIObjectDestruct },
	{ kEventClassToolbarItem, kEventToolbarItemCreateCustomView }
};

const EventTypeSpec segmentedControlEvents[] = {
	
	{ kEventClassControl, kEventControlGetSizeConstraints}
};

struct SegmentedToolbarItem
{
	HIToolbarItemRef        toolbarItem; 
	PKToolbarSegmentedItem *sitem;
};

typedef struct SegmentedToolbarItem SegmentedToolbarItem;

// Construct for the item structure

static OSStatus ConstructSegmentedToolbarItem(HIToolbarItemRef inItem, 
											  PKToolbarSegmentedItem *sitem,
											  SegmentedToolbarItem** outItem)
{
	SegmentedToolbarItem* item; 
	OSStatus err = noErr;
	
	item = (SegmentedToolbarItem*) malloc(sizeof(SegmentedToolbarItem));
	require_action( item != NULL, CantAllocItem, err = memFullErr );
	item->toolbarItem = inItem; 
	item->sitem = sitem;
	
	*outItem = item;

CantAllocItem: 
	return err;
}

// Handler for the control messages

static OSStatus SegmentedControlEventHandler(EventHandlerCallRef inCallRef, 
											 EventRef inEvent, void* inUserData)
{
	PKToolbarSegmentedItem *sitem = (PKToolbarSegmentedItem *) inUserData;
	
	if(sitem == NULL)
	{
		return eventNotHandledErr;
	}
	
	switch(GetEventKind(inEvent))
	{
		case kEventControlGetSizeConstraints: 
		{
			HISize minBounds; 
			HISize maxBounds;
			
			if(sitem->normalImage == NULL)
			{
				return eventNotHandledErr;
			}
			
			minBounds.width  = sitem->normalImage->getWidth();
			minBounds.height = sitem->normalImage->getHeight();
			
			maxBounds.width  = sitem->normalImage->getWidth();
			maxBounds.height = sitem->normalImage->getHeight();
			
			SetEventParameter(inEvent, kEventParamMinimumSize, typeHISize,
							  sizeof(HISize), &minBounds);
			
			SetEventParameter (inEvent, kEventParamMaximumSize, typeHISize, 
							   sizeof(HISize), &maxBounds);
			
			return noErr;
		}
		break;

		default:
			break;
	}
	
	return eventNotHandledErr; 
}

// Handler for the subclass

static OSStatus SegmentedToolbarItemHandler(EventHandlerCallRef inCallRef,
											EventRef inEvent, void* inUserData)
{
	OSStatus result = eventNotHandledErr;
	
	SegmentedToolbarItem *object = (SegmentedToolbarItem*) inUserData;

	switch(GetEventClass(inEvent)) 
	{
		case kEventClassHIObject: 
			switch (GetEventKind(inEvent))
			{
				case kEventHIObjectConstruct: 
				{
					HIObjectRef	toolbarItem; 
					SegmentedToolbarItem* item;
					
					GetEventParameter(inEvent, kEventParamHIObjectInstance, 
									  typeHIObjectRef, NULL, 
									  sizeof( HIObjectRef ), NULL, &toolbarItem);
					
					result = ConstructSegmentedToolbarItem(toolbarItem, NULL, &item);
					
					if(result == noErr)
					{
						SetEventParameter(inEvent, kEventParamHIObjectInstance, 
										  typeVoidPtr, sizeof( void * ), &item);
					}
				}
				break;
					
			case kEventHIObjectInitialize:
				{
					void *sitem = NULL;

					GetEventParameter(inEvent, kEventParamUserData, 
									  typeVoidPtr, NULL, 
									  sizeof(void *), NULL, &sitem);

					object->sitem = (PKToolbarSegmentedItem *) sitem;
					
					if(CallNextEventHandler(inCallRef, inEvent) == noErr)
					{
						if(object->sitem != NULL)
						{
							std::wstring label   = PKVALUE_WSTRING(object->sitem->get(PKToolbarSegmentedItem::TEXT_WSTRING_PROPERTY));
							CFStringRef labelStr = PKStr::wStringToCFString(label);
							
							HIToolbarItemSetLabel(object->toolbarItem, labelStr);
							CFRelease(labelStr);
							
							result = noErr;
						}
					}
				}
				break;
					
			case kEventHIObjectDestruct: 
				free(object);
				result = noErr;
				break;
		} 
		break;
		
		case kEventClassToolbarItem:
		{
			switch(GetEventKind(inEvent))
			{
				case kEventToolbarItemCreateCustomView: 
				{
					EventTargetRef segmentedEventTarget; 
					HIViewRef segmentedView; 
					HIRect rect;
				
					if(object->sitem->normalImage == NULL)
					{
						return result;
					}
					
					rect.origin.x = 0;
					rect.origin.y = 0;
					rect.size.width  = object->sitem->normalImage->getWidth();
					rect.size.height = object->sitem->normalImage->getHeight();
					
					HIPKMacSegmentedViewCreate(NULL, &rect, object->sitem, (ControlRef*) &segmentedView);
					
					SetEventParameter(inEvent, kEventParamControlRef, typeControlRef, sizeof(segmentedView), &segmentedView);
				
					segmentedEventTarget = GetControlEventTarget(segmentedView);
				
					InstallEventHandler(segmentedEventTarget, SegmentedControlEventHandler,
										GetEventTypeCount(segmentedControlEvents),
										segmentedControlEvents, (void *) object->sitem, NULL); 
					
					result = noErr;
				}
				break;
			}
		}
		break;
	} 
	
	return result;				
}

// Register subclass function

void RegisterSegmentedToolbarItemClass() 
{
	
	if(!sRegistered) 
	{
		HIObjectRegisterSubclass(kSegmentedToolbarItemClassID, 
							     kHIToolbarItemClassID, 0, 
								 SegmentedToolbarItemHandler, 
								 GetEventTypeCount(segmentedEvents),
								 segmentedEvents, 0, NULL);
		sRegistered = true;
	}
}

//
// Create Function for the Toolbar item
//

HIToolbarItemRef CreateSegmentedToolbarItem(PKToolbarSegmentedItem *item)
{
	if(item == NULL)
	{
		return NULL;
	}
	
	OSStatus err;
	EventRef event;
	UInt32 options = kHIToolbarItemAllowDuplicates;
	HIToolbarItemRef result = NULL;
	
	PKVariant   *iid  = item->get(PKToolbarSegmentedItem::ID_STRING_PROPERTY);
	std::string  itid = PKVALUE_STRING(iid);
	
	std::wstring witid  = PKStr::stringToWString(itid);
	CFStringRef inIdentifier = PKStr::wStringToCFString(witid);

	RegisterSegmentedToolbarItemClass();
	
	err = CreateEvent(NULL, kEventClassHIObject, 
					  kEventHIObjectInitialize, 
					  GetCurrentEventTime(), 0,
					  &event); 
	
	require_noerr(err, CantCreateEvent);

	SetEventParameter(event, kEventParamToolbarItemIdentifier,	
					  typeCFStringRef, sizeof( CFStringRef ), &inIdentifier);
	
    SetEventParameter(event, kEventParamAttributes, 
					  typeUInt32,	sizeof( UInt32 ), &options);
	
	SetEventParameter(event, kEventParamUserData,
					  typeVoidPtr, sizeof(void *), (void *) &item);
										
	err = HIObjectCreate(kSegmentedToolbarItemClassID, event, (HIObjectRef*) &result);
	
	CFRelease(inIdentifier);
	
	check_noerr( err );
	ReleaseEvent( event );

CantCreateEvent:
	return result;

}