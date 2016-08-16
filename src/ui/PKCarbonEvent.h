//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKCarbonEvent_H_
#define PKCarbonEvent_H_

#include <Carbon/Carbon.h>

class PKCarbonEvent
{
public:
	// Construction/Destruction
	PKCarbonEvent(
					UInt32				inClass,
					UInt32				inKind );
	PKCarbonEvent(
					EventRef			inEvent );
	virtual ~PKCarbonEvent();
	
	UInt32		GetClass() const;
	UInt32		GetKind() const;
	
	// Time
	void		SetTime(
					EventTime inTime );
	EventTime	GetTime() const;
	
	// Retention
	void		Retain();
	void		Release();
	
	// Accessors
	operator	EventRef&()
					{ return fEvent; };
	EventRef	GetEventRef()
					{ return fEvent; }
	
	// Posting
	OSStatus 	PostToQueue(
					EventQueueRef		inQueue,
					EventPriority		inPriority = kEventPriorityStandard );

	// Parameters
	OSStatus	SetParameter(
					EventParamName		inName,
					EventParamType		inType,
					UInt32				inSize,
					const void*			inData );
	OSStatus	GetParameter(
					EventParamName		inName,
					EventParamType		inType,
					UInt32				inBufferSize,
					void*				outData );

	OSStatus	GetParameterType(
					EventParamName		inName,
					EventParamType*		outType );
	OSStatus	GetParameterSize(
					EventParamName		inName,
					UInt32*				outSize );

	// Simple parameters
	OSStatus	SetParameter(
					EventParamName		inName,
					Boolean				inValue );
	OSStatus	GetParameter(
					EventParamName		inName,
					Boolean*			outValue );

	OSStatus	SetParameter(
					EventParamName		inName,
					bool				inValue );
	OSStatus	GetParameter(
					EventParamName		inName,
					bool*				outValue );

	OSStatus	SetParameter(
					EventParamName		inName,
					Point				inPt );
	OSStatus	GetParameter(
					EventParamName		inName,
					Point*				outPt );

	OSStatus	SetParameter(
					EventParamName		inName,
					const HIPoint&		inPt );

	OSStatus	GetParameter(
					EventParamName		inName,
					HIPoint*			outPt );

	OSStatus	SetParameter(
					EventParamName		inName,
					const Rect&			inRect );
	OSStatus	GetParameter(
					EventParamName		inName,
					Rect*				outRect );

	OSStatus	SetParameter(
					EventParamName		inName,
					const HIRect&		inRect );
	OSStatus	GetParameter(
					EventParamName		inName,
					HIRect*				outRect );

	OSStatus	SetParameter(
					EventParamName		inName,
					const HISize&		inSize );
	OSStatus	GetParameter(
					EventParamName		inName,
					HISize*				outSize );

	OSStatus	SetParameter(
					EventParamName		inName,
					RgnHandle			inRegion );
	OSStatus	GetParameter(
					EventParamName		inName,
					RgnHandle*			outRegion );

	OSStatus	SetParameter(
					EventParamName		inName,
					WindowRef			inWindow );
	OSStatus	GetParameter(
					EventParamName		inName,
					WindowRef*			outWindow );

	OSStatus	SetParameter(
					EventParamName		inName,
					ControlRef			inControl );
	OSStatus	GetParameter(
					EventParamName		inName,
					ControlRef* outControl );

	OSStatus	SetParameter(
					EventParamName		inName,
					MenuRef				inMenu );
	OSStatus	GetParameter(
					EventParamName		inName,
					MenuRef*			outMenu );

	OSStatus	SetParameter(
					EventParamName		inName,
					DragRef				inDrag );
	OSStatus	GetParameter(
					EventParamName		inName,
					DragRef*			outDrag );

	OSStatus	SetParameter(
					EventParamName		inName,
					UInt32				inValue );
	OSStatus	GetParameter(
					EventParamName		inName,
					UInt32*				outValue );
	
	OSStatus	SetParameter(
					EventParamName		inName,
					const HICommand&	inValue );
	OSStatus	GetParameter(
					EventParamName		inName,
					HICommand*			outValue );

	OSStatus	SetParameter(
					EventParamName		inName,
					const ControlPartCode&	inValue );
	OSStatus	GetParameter(
					EventParamName		inName,
					ControlPartCode*			outValue );

	// Template parameters
	template <class T> OSStatus SetParameter(
		EventParamName	inName,
		EventParamType	inType,
		const T&		inValue )
	{
		return SetParameter( inName, inType, sizeof( T ), &inValue );
	}
			
	template <class T> OSStatus GetParameter(
		EventParamName	inName,
		EventParamType	inType,
		T*				outValue )
	{
		return GetParameter( inName, inType, sizeof( T ), outValue );
	}
	
private:
	EventRef	fEvent;
};

#endif // PKCarbonEvent_H_
