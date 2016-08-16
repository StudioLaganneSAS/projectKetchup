//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKCarbonEvent.h"

//-----------------------------------------------------------------------------------
//	PKCarbonEvent constructor
//-----------------------------------------------------------------------------------
//
PKCarbonEvent::PKCarbonEvent(
	UInt32				inClass,
	UInt32				inKind )
{
	CreateEvent( NULL, inClass, inKind, GetCurrentEventTime(), 0, &fEvent );
}

//-----------------------------------------------------------------------------------
//	PKCarbonEvent constructor
//-----------------------------------------------------------------------------------
//
PKCarbonEvent::PKCarbonEvent(
	EventRef			inEvent )
{
	fEvent = inEvent;
	RetainEvent( fEvent );
}

//-----------------------------------------------------------------------------------
//	PKCarbonEvent destructor
//-----------------------------------------------------------------------------------
//
PKCarbonEvent::~PKCarbonEvent()
{
	ReleaseEvent( fEvent );
}

//-----------------------------------------------------------------------------------
//	GetClass
//-----------------------------------------------------------------------------------
//
UInt32 PKCarbonEvent::GetClass() const
{
	return GetEventClass( fEvent );
}

//-----------------------------------------------------------------------------------
//	GetKind
//-----------------------------------------------------------------------------------
//
UInt32 PKCarbonEvent::GetKind() const
{
	return GetEventKind( fEvent );
}

//-----------------------------------------------------------------------------------
//	SetTime
//-----------------------------------------------------------------------------------
//
void PKCarbonEvent::SetTime(
	EventTime			inTime )
{
	SetEventTime( fEvent, inTime );
}

//-----------------------------------------------------------------------------------
//	GetTime
//-----------------------------------------------------------------------------------
//
EventTime PKCarbonEvent::GetTime() const
{
	return GetEventTime( fEvent );
}

//-----------------------------------------------------------------------------------
//	Retain
//-----------------------------------------------------------------------------------
//
void PKCarbonEvent::Retain()
{
	RetainEvent( fEvent );
}

//-----------------------------------------------------------------------------------
//	Release
//-----------------------------------------------------------------------------------
//
void PKCarbonEvent::Release()
{
	ReleaseEvent( fEvent );
}

//-----------------------------------------------------------------------------------
//	PostToQueue
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::PostToQueue(
	EventQueueRef		inQueue,
	EventPriority		inPriority )
{
	return PostEventToQueue( inQueue, fEvent, inPriority );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	EventParamType		inType,
	UInt32				inSize,
	const void*			inData )
{
	return SetEventParameter( fEvent, inName, inType, inSize, inData );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	EventParamType		inType,
	UInt32				inBufferSize,
	void*				outData )
{
	return GetEventParameter( fEvent, inName, inType, NULL, inBufferSize, NULL, outData );
}

//-----------------------------------------------------------------------------------
//	GetParameterType
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameterType(
	EventParamName		inName,
	EventParamType*		outType )
{
	return GetEventParameter( fEvent, inName, typeWildCard, outType, 0, NULL, NULL );
}

//-----------------------------------------------------------------------------------
//	GetParameterSize
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameterSize(
	EventParamName		inName,
	UInt32*				outSize )
{
	return GetEventParameter( fEvent, inName, typeWildCard, NULL, 0, outSize, NULL );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	Boolean				inValue )
{
	return SetParameter<Boolean>( inName, typeBoolean, inValue );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	Boolean*			outValue )
{
	return GetParameter<Boolean>( inName, typeBoolean, outValue );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	bool				inValue )
{
	return SetParameter<Boolean>( inName, typeBoolean, (Boolean) inValue );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	bool*				outValue )
{
	return GetParameter<Boolean>( inName, sizeof( Boolean ), (Boolean*) outValue );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	Point				inPt )
{
	return SetParameter<Point>( inName, typeQDPoint, inPt );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	Point*				outPt )
{
	return GetParameter<Point>( inName, typeQDPoint, outPt );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	const HIPoint&		inPt )
{
	return SetParameter<HIPoint>( inName, typeHIPoint, inPt );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	HIPoint*			outPt )
{
	return GetParameter<HIPoint>( inName, typeHIPoint, outPt );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	const Rect&			inRect )
{
	return SetParameter<Rect>( inName, typeQDRectangle, inRect );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	Rect*				outRect )
{
	return GetParameter<Rect>( inName, typeQDRectangle, outRect );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	const HIRect&		inRect )
{
	return SetParameter<HIRect>( inName, typeHIRect, inRect );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	HIRect*				outRect )
{
	return GetParameter<HIRect>( inName, typeHIRect, outRect );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	const HISize&		inSize )
{
	return SetParameter<HISize>( inName, typeHISize, inSize );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	HISize*				outSize )
{
	return GetParameter<HISize>( inName, typeHISize, outSize );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	RgnHandle			inRegion )
{
	return SetParameter<RgnHandle>( inName, typeQDRgnHandle, inRegion );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	RgnHandle*			outRegion )
{
	return GetParameter<RgnHandle>( inName, typeQDRgnHandle, outRegion );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	WindowRef			inWindow )
{
	return SetParameter<WindowRef>( inName, typeWindowRef, inWindow );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	WindowRef*			outWindow )
{
	return GetParameter<WindowRef>( inName, typeWindowRef, outWindow );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	ControlRef			inControl )
{
	return SetParameter<ControlRef>( inName, typeControlRef, inControl );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	ControlRef* outControl )
{
	return GetParameter<ControlRef>( inName, typeControlRef, outControl );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	MenuRef				inMenu )
{
	return SetParameter<MenuRef>( inName, typeMenuRef, inMenu );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	MenuRef*			outMenu )
{
	return GetParameter<MenuRef>( inName, typeMenuRef, outMenu );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	DragRef				inDrag )
{
	return SetParameter<DragRef>( inName, typeDragRef, inDrag );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	DragRef*			outDrag )
{
	return GetParameter<DragRef>( inName, typeDragRef, outDrag );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	UInt32				inValue )
{
	return SetParameter<UInt32>( inName, typeUInt32, inValue );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	UInt32*				outValue )
{
	return GetParameter<UInt32>( inName, typeUInt32, outValue );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName		inName,
	const HICommand&	inValue )
{
	return SetParameter<HICommand>( inName, typeHICommand, inValue );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	HICommand*			outValue )
{
	return GetParameter<HICommand>( inName, typeHICommand, outValue );
}

//-----------------------------------------------------------------------------------
//	SetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::SetParameter(
	EventParamName			inName,
	const ControlPartCode&	inValue )
{
	return SetParameter<ControlPartCode>( inName, typeControlPartCode, inValue );
}

//-----------------------------------------------------------------------------------
//	GetParameter
//-----------------------------------------------------------------------------------
//
OSStatus PKCarbonEvent::GetParameter(
	EventParamName		inName,
	ControlPartCode*	outValue )
{
	return GetParameter<ControlPartCode>( inName, typeControlPartCode, outValue );
}
