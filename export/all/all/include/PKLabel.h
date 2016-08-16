//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_LABEL_H
#define PK_LABEL_H

#include "PKControl.h"

//
// PKLabel
//

class PKLabel : public PKControl
{
public:

    static std::string MULTILINE_BOOL_PROPERTY;
    static std::string TEXT_WSTRING_PROPERTY;
    static std::string FONTNAME_STRING_PROPERTY;
    static std::string FONTSIZE_UINT32_PROPERTY;
    static std::string RELATIVEFONTSIZE_STRING_PROPERTY;
    static std::string COLOR_COLOR_PROPERTY;
	static std::string BOLD_BOOL_PROPERTY;
	static std::string ITALICS_BOOL_PROPERTY;
    static std::string ALIGN_STRING_PROPERTY;
    static std::string MAXWIDTH_UINT32_PROPERTY;
    static std::string FIXED_SIZE_BOOL_PROPERTY;

public:
	PKLabel();
protected:
	~PKLabel();

public:
    void build();

	void setText(std::wstring text);

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	int getTextWidth();
	int getTextHeight();

    // From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

#ifdef WIN32
    HFONT font;
#endif

private:

    int numLines;
};

#endif // PK_LABEL_H
