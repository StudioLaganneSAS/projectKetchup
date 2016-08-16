//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_EDITABLE_VECTOR_LABEL_H
#define PK_EDITABLE_VECTOR_LABEL_H

#include "PKVectorControl.h"
#include "PKTimer.h"
#include "PKPopupMenu.h"
#include "PKUndoManager.h"

//
// PKEditableVectorLabel
//

class PKEditableVectorLabel : public PKVectorControl, public PKTimer, public PKPopupMenuCallback, public PKUndoManagerCommitCallback
{
public:

    //
    // PROPERTIES
    //

    static std::string TEXT_WSTRING_PROPERTY;
    static std::string BACKGROUND_COLOR_COLOR_PROPERTY;
    static std::string BORDER_COLOR_COLOR_PROPERTY;
    static std::string HOVER_BACKGROUND_COLOR_COLOR_PROPERTY;
    static std::string HOVER_BORDER_COLOR_COLOR_PROPERTY;
    static std::string TEXT_COLOR_COLOR_PROPERTY;
    static std::string SELECTED_TEXT_COLOR_COLOR_PROPERTY;
    static std::string SELECTION_COLOR_COLOR_PROPERTY;
    static std::string BORDER_WIDTH_UINT32_PROPERTY;
    static std::string FONT_FAMILY_STRING_PROPERTY;
    static std::string BOLD_BOOL_PROPERTY;
    static std::string DEFAULT_TEXT_WSTRING_PROPERTY;
    static std::string DEFAULT_TEXT_COLOR_COLOR_PROPERTY;
    static std::string OFFSET_UINT32_PROPERTY;
	static std::string EDITABLE_BOOL_PROPERTY;
    static std::string CENTER_BOOL_PROPERTY;
	
public:
	PKEditableVectorLabel();

protected:
	virtual ~PKEditableVectorLabel();

public:
    virtual void build();

	void commitChanges();
	void resetChanges();

	// From PKVectorControl

	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

	// From PKCustomControl
	
	virtual void mouseDown(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod);

	virtual void mouseUp(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod);

    virtual void mouseMove(int x,
						   int y,
						   unsigned int mod);
	
	virtual void mouseDblClick(PKButtonType button,
							   int x,
							   int y,
							   unsigned int mod);

	virtual void mouseLeave();	
	
    virtual void focusAcquired(PKDirection dir);
    virtual void focusLost();

	virtual bool keyDown(unsigned int modifier,
						 PKKey        keyCode,
						 unsigned int virtualCode);

    virtual void characterInput(std::wstring wstr);

	virtual bool wantsKeyboardFocus();

    // From PKControl

    void updateUIStrings();

    virtual void doCut();
    virtual void doCopy();
    virtual void doPaste();
    virtual void doSelectAll();

    virtual bool hasSelection();

    // From PKTimer

    void timerCallbackFunction();

    // From PKUndoManagerCallback

    void undoManagerCommitCallback();

protected:

    // From PKPopupMenuCallback

    virtual void menuCancelled();
	virtual void menuCallback(unsigned int id);

    // From PKVectorControl

    virtual void drawVector(PKVectorEngine *engine,
		                    int32_t x, int32_t y);
	
	// Selection
	
	void selectAll();

	// Misc
	
	uint32_t getBarPosForMouseX(int x);
	
public:
	
	bool built;
    bool needsFontSizeUpdate;
    uint32_t fontSize;
	bool editing;
	bool hover;
    PKPopupMenu *menu;
    uint32_t barPos;
    bool barShown;
    int32_t selectionStart;
    uint32_t selectionSize;
    uint32_t selectionStartedFrom;
	bool needsOffsetUpdate;
	std::vector<int> letter_offsets;
	bool mouseIsDown;
	bool popupShowing;
    std::wstring _startText;
	
#ifdef LINUX

public:
    GtkIMContext *im_context;
    gulong signal_id_1;
    gulong signal_id_2;
    
protected:

    static gboolean gtk_vector_key_press_event_callback(GtkWidget   *widget,
                                                        GdkEventKey *event,
                                                        gpointer     user_data);  

    static gboolean gtk_vector_key_release_event_callback(GtkWidget   *widget,
                                                          GdkEventKey *event,
                                                          gpointer     user_data);  

    static void gtk_im_commit(GtkIMContext *context,
                              gchar        *str,
                              gpointer      user_data);
                              
    static gboolean gtk_im_retrieve_surroundings(GtkIMContext *context,
                                                 gpointer      user_data);
    
#endif
	
};

#endif // PK_EDITABLE_VECTOR_LABEL_H
