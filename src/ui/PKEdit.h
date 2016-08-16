//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_EDIT_H
#define PK_EDIT_H

#include "PKControl.h"
#include "PKSafeEvents.h"
#include "PKUndoManager.h"

typedef enum 
{
	PK_EDIT_TYPE_TEXT,
    PK_EDIT_TYPE_PASSWORD,
	PK_EDIT_TYPE_NUMBER,

} PKEditType;

//
// PKEdit
//

class PKEdit : public PKControl, public PKUndoManagerCommitCallback
{
public:

    static std::string TEXT_WSTRING_PROPERTY;
    static std::string TYPE_INT32_PROPERTY;
    static std::string MULTILINE_BOOL_PROPERTY;
    static std::string VSCROLL_BOOL_PROPERTY;
    static std::string HSCROLL_BOOL_PROPERTY;
    static std::string BORDER_BOOL_PROPERTY;

public:
	PKEdit();
protected:
	~PKEdit();

public:
    void build();

	void setText(std::wstring text);
	std::wstring getText();
	
	void selectText();

	void commitChanges();
	void resetChanges();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

    // From PKControl

    virtual bool hasSelection();

    void focusAcquired(PKDirection direction);
    void focusLost();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

	// IPKSafeEventHandler
	
	void processSafeEvent(std::string  eventName,
						  void        *payload);

    // PKUndoManagerCallback

    void undoManagerCommitCallback();

#ifdef LINUX

    static void gtk_entry_text_changed_event(GtkEditable *editable,
                                             gpointer     user_data);
                                             
#endif
private:
	std::wstring _startText;
};

#endif // PK_EDIT_H
