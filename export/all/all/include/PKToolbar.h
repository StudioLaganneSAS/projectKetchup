//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_TOOLBAR_H
#define PK_TOOLBAR_H

#include "PKUI.h"
#include "PKObject.h"
#include "PKControl.h"
#include "PKImage.h"
#include "PKToolbarSegmentedItem.h"

#if(defined(WIN32))
#include "PKDoubleBufferedControl.h"
#endif

class PKWindow;
class PKFont;

//
// PKToolbar
//

#ifdef MACOSX
class PKToolbar : public PKObject
#endif
#ifdef LINUX
class PKToolbar : public PKControl
#endif
#if(defined(WIN32))
class PKToolbar : public PKDoubleBufferedControl
#endif
{
public:
	
	//
	// PROPERTIES
	//
	
	static std::string ID_STRING_PROPERTY;

    static std::string LIGHTCOLOR_COLOR_PROPERTY;
    static std::string DARKCOLOR_COLOR_PROPERTY;
	
public:	
	PKToolbar();

protected:
	virtual ~PKToolbar();
	
public:
    // Helper to load from XML
	
    static PKToolbar *loadToolbarFromXML(std::string xml);
		
	// Builds the controls
	
	virtual void build();
	virtual void initialize(PKDialog *dialog);

    virtual void createLayout();
    virtual void deleteLayout();
	
	// Returns the toolbal handle
	// for adding to a window
	
	virtual void *getToolbarHandle();

	// resize
	void resize(uint32_t w, 
			    uint32_t h);

	// i18n
	void updateUIStrings();

	// Util
	PKWindow *getWindow();
	void setWindow(PKWindow *window);
	
    void setSegmentedItemSelection(std::string id, int32_t sel);

    PKImage *loadBitmap(std::wstring imgURL);

    static int PK_TOOLBAR_HEIGHT;

#if(defined(WIN32))

    // From PKDoubleBufferedControl

    void draw32bpp(uint8_t *buffer);

    // From PKCustomControl

	void mouseDown(PKButtonType button,
				   int x,
				   int y,
				   unsigned int mod);

	void mouseUp(PKButtonType button,
				 int x,
				 int y,
				 unsigned int mod);

	void mouseDblClick(PKButtonType button,
					   int x,
					   int y,
					   unsigned int mod);

	void mouseMove(int x,
				   int y,
				   unsigned int mod);

	void mouseLeave();

    void invalidate(bool recurse = true);

	// Members

    PKFont *font;
    PKFont *shadowFont;

    std::vector <PKImage*> images;
    std::vector <PKImage*> over_images;
    std::vector <PKImage*> down_images;
    std::vector <PKPoint>  positions;
    std::vector <std::wstring> labels;
    std::vector <PKPoint>  lpositions;
    std::vector <int>      spositions;
    std::vector <PKRect>   itemAreas;
    std::vector<PKToolbarSegmentedItem*> segments;

    int overIndex;
    int downIndex;

    int overSubIndex;
    int downSubIndex;

    // From PKCustomControl
	
    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

#endif

#ifdef LINUX

    // From PKControl

    uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();
	
	static void gtk_toolbar_button_clicked(GtkToolButton *toolbutton,
                                           gpointer       user_data);
	
    void linuxButtonClicked(GtkToolButton *toolbutton);
    
    std::vector<std::string>     ids;
    std::vector<GtkToolButton *> buttons;
    
#endif


private:
	
#ifdef MACOSX

	HIToolbarRef toolbar;
	EventHandlerRef handler;
	EventHandlerRef commandHandler;

public:
	std::vector<PKImage*> bitmaps;
	
#endif
	
	bool built;
	PKWindow *window;
 
public:	
	static int lastId;
};

#endif // PK_TOOLBAR_H
