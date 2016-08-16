//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_DIALOG_H
#define PK_DIALOG_H

#include "PKWindow.h"
#include "PKControl.h"

class PKToolbar;
class PKToolbarItem;
class PKToolbarSegmentedItem;
class PKBitmapToggleButton;
class PKStretchToggleButton;
class PKCustomControl;
class IPKObjectFactory;

//
// PKDialog
//

class PKDialog : public PKWindow
{
public:

	PKDialog(std::wstring  id, // unique
			 PKWindowStyle flags,
			 PKWindowHandle parent = NULL,
             std::wstring   icon   = L"");

protected:
	virtual ~PKDialog();

public:

    virtual void addFactory(IPKObjectFactory *factory);
    virtual bool loadFromXML(std::string xml);
	
	virtual void build();

	virtual void resizeToFit();

    virtual void updateUIStrings();
    
    virtual void resizeWindowAreaTo(int w, int h);
	virtual void resizeClientAreaTo(int w, int h);

	virtual void windowResized();

	virtual void windowGetMinMax(uint32_t *minWidth,
								 uint32_t *minHeight,
								 uint32_t *maxWidth,
								 uint32_t *maxHeight);

	// Toolbar
	
	virtual void setToolbar(PKToolbar *tb);

    // Menu

    virtual void doCut();
    virtual void doCopy();
    virtual void doPaste();
    virtual void doSelectAll();

    // Callbacks

	virtual void toolbarItemClicked(PKToolbar *toolbar,
									PKToolbarItem *item,
									std::string id);

    virtual void toolbarSegmentedItemSwitched(PKToolbar *toolbar,
									          PKToolbarSegmentedItem *item,
									          std::string id, 
                                              int32_t selection);

    virtual void buttonClicked(std::string id);
    virtual void linkClicked(std::string id);

	virtual void checkBoxAction(std::string id, bool previousValue);
	virtual void comboBoxChanged(std::string id, int32_t previousValue);
	virtual void sliderChanged(std::string id, int32_t previousValue);

	virtual void listBoxSelectionChanged(std::string id, int32_t oldValue);

	virtual void treeViewItemRightClicked(std::string id,
										  int32_t x, int32_t y);
    
	virtual void treeViewItemSelected(std::string id);

	virtual void editTextChanged(std::string id, 
                                 std::wstring text);

	virtual void editTextEditingStarted(std::string id);

	virtual void editTextEditingEnded(std::string id, 
									  std::wstring oldValue, 
									  std::wstring newValue);

    virtual void bitmapToggleButtonToggled(std::string id,
									       PKBitmapToggleButton *button,
									       bool previousValue);

	virtual void stretchToggleButtonToggled(std::string id,
                                            PKStretchToggleButton *button,
									        bool previousValue);
	
	virtual void editableVectorLabelTextChanged(std::string id);

    virtual void editableVectorLabelEditStarted(std::string id);
	virtual void editableVectorLabelEditEnded(std::string id, 
        									  std::wstring oldValue, 
		        							  std::wstring newValue);

    virtual void customControlClicked(PKButtonType button,
                                      std::string id,
                                      PKCustomControl *control,
                                      std::string partCode);

    virtual void customControlDoubleClicked(PKButtonType button,
											std::string id,
											PKCustomControl *control,
											std::string partCode);

    virtual void vectorControlRootMouseMove(std::string id,
                                            int32_t x,
                                            int32_t y);
    
    virtual void vectorControlRootMouseLeave(std::string id);

    virtual void selectorControlChanged(std::string id, int32_t oldValue);

    // Accessors

	PKControl *findControl(std::string id);
    PKControl *getFocusedControl();

	// Utilities

	void enableControl(std::string id);
	void disableControl(std::string id);

	void setControlEnabled(std::string id, bool enabled);
	
	void showControl(std::string id);
	void hideControl(std::string id);

	void setControlTreeHidden(std::string id, bool hidden);

    void setFocusToControl(std::string id);	
	void setControlHidden(std::string id, bool hidden);
    bool getControlHidden(std::string id);

    void invalidateControl(std::string id);
    void relayoutControl(std::string id);
	
	void setControlWidth(std::string id, uint32_t w);
	void setControlHeight(std::string id, uint32_t h);

	void setLayoutMargin(std::string id, uint32_t margin);
	void setLayoutSpacing(std::string id, uint32_t spacing);

	// Controls Helpers

	void setButtonText(std::string id, std::wstring text);

	void setCheckBoxText(std::string id, std::wstring text);
	void setCheckBoxChecked(std::string id, bool checked);
	bool getCheckBoxChecked(std::string id);

	void setEditText(std::string id, std::wstring text);
	std::wstring getEditText(std::string id);
	void resetEditChanges(std::string id);

	void setEditableLabelText(std::string id, std::wstring text);
	std::wstring getEditableLabelText(std::string id);
	void resetEditableLabelChanges(std::string id);

	void setLabelText(std::string id, std::wstring text);

	void loadImageFromResource(std::string id, std::wstring image);
	void loadImageFromFilename(std::string id, std::wstring image);

	void clearComboBoxItems(std::string id);
	void addComboBoxItem(std::string id, std::wstring text);
	void selectComboBoxItem(std::string id, int index);
	int  getComboBoxSelectedItem(std::string id);
	std::wstring getComboBoxSelectedText(std::string id);

	void    setSliderValue(std::string id, int32_t value);
	int32_t getSliderValue(std::string id);

	void    setProgressValue(std::string id, int32_t value);
	int32_t getProgressValue(std::string id);

	bool getBitmapToggleButtonToggled(std::string id);
	void setBitmapToggleButtonToggled(std::string id, bool toggled);

    void startAnimation(std::string id);
    void stopAnimation(std::string id);

    void    setSelectorControlSelection(std::string id, int32_t selection);
    int32_t getSelectorControlSelection(std::string id);

    void    setIconTabControlTab(std::string id, int32_t tab);
    int32_t getIconTabControlTab(std::string id);

    void    setArrowTabControlTab(std::string id, int32_t tab);
    int32_t getArrowTabControlTab(std::string id);

    void setToolbarSegmentedItemSelection(std::string id, int32_t sel);

	void commitEditControlChanges();

    // From PKWindow

	//
	// Mouse events
	//
	// Sent by the window when the mouse
	// hovers, or is down/up as well as for
	// double clicks and mouse wheel
	//
	
	virtual void mouseDown(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod);
	
	virtual void mouseUp(PKButtonType button,
						 int x,
						 int y,
						 unsigned int mod);
	
	virtual void mouseDblClick(PKButtonType button,
							   int x,
							   int y,
							   unsigned int mod);
	
	virtual void mouseMove(int x,
						   int y,
						   unsigned int mod);
	
	virtual void mouseLeave();
	
	virtual void mouseWheel(PKButtonType button,
							int x,
							int y,
							unsigned int mod);
	
	//
	// keyDown() / keyUp()
	//
	// Sent when the window has
	// the focus and a key is 
	// pressed/released.
	//
	
	virtual void keyDown(unsigned int modifier,
						 PKKey        keyCode,
						 unsigned int virtualCode);
	
	virtual void keyUp(unsigned int modifier,
					   PKKey        keyCode,
					   unsigned int virtualCode);

protected:

    PKToolbar *toolbar;

public:

#ifdef WIN32

	HBRUSH getBackgroundBrush();
	
	WNDPROC lpOldProc;

	static LRESULT CALLBACK DialogWinProc(HWND hwnd,
								 		  UINT uMsg,
										  WPARAM wParam,
										  LPARAM lParam);
#endif

#ifdef MACOSX

	EventHandlerRef handler;

	static pascal OSStatus PKDialogEventHandler(EventHandlerCallRef  nextHandler,
												EventRef             inEvent, 
												void                *userData);


#endif

#ifdef LINUX
    PKControl *current_focus;
#endif

    PKView *currentView;
    PKView *downView;
    bool built;

    std::vector <IPKObjectFactory*> factories;
};

#endif // PK_DIALOG_H
