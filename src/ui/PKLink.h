//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_LINK_H
#define PK_LINK_H

#include "PKControl.h"

//
// PKLink
//

class PKLink : public PKControl
{
public:

    static std::string URL_STRING_PROPERTY;
    static std::string TEXT_WSTRING_PROPERTY;
    static std::string COLOR_COLOR_PROPERTY;
    static std::string FONTNAME_STRING_PROPERTY;
    static std::string FONTSIZE_UINT32_PROPERTY;
    static std::string RELATIVEFONTSIZE_STRING_PROPERTY;
	static std::string BOLD_BOOL_PROPERTY;
	static std::string ITALICS_BOOL_PROPERTY;

public:
	PKLink();
protected:
	~PKLink();

public:
    void build();

	void setText(std::wstring text);
    void setColor(PKColor color);
    
    // Fire the URL
    void clicked();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	int getTextWidth();
	int getTextHeight();

    // From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

#ifdef WIN32
public:

	WNDPROC lpOldProc;

	static LRESULT CALLBACK LinkWinProc(HWND hwnd,
							 		    UINT uMsg,
								 	    WPARAM wParam,
									    LPARAM lParam);
#ifdef WIN32
    HFONT font;
#endif

#endif

#ifdef MACOSX
	
	EventHandlerRef handler;
	
	static pascal OSStatus PKLinkEventHandler(EventHandlerCallRef  nextHandler,
											  EventRef             inEvent, 
											  void                *userData);
	
	
#endif
	
private:
};

#endif // PK_LINK_H
