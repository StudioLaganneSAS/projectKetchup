//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKDialog.h"
#include "PKButton.h"
#include "PKCheckBox.h"
#include "PKGroupBox.h"
#include "PKImageView.h"
#include "PKCheckBox.h"
#include "PKLabel.h"
#include "PKComboBox.h"
#include "PKSlider.h"
#include "PKEdit.h"
#include "PKProgressBar.h"
#include "PKTabControl.h"
#include "PKLink.h"
#include "PKAnimationControl.h"
#include "PKPanelControl.h"
#include "PKGradientPanel.h"
#include "PKCustomControl.h"
#include "PKBitmapButton.h"
#include "PKStretchButton.h"
#include "PKBitmapToggleButton.h"
#include "PKStretchToggleButton.h"
#include "PKToolbar.h"
#include "PKToolbarItem.h"
#include "PKVLayout.h"
#include "PKStr.h"
#include "PKScrollView.h"
#include "PKTreeView.h"
#include "PKListBox.h"
#include "PKMenu.h"
#include "PKUIFactory.h"
#include "PKObjectLoader.h"
#include "PKCursorTools.h"
#include "PKClipboard.h"
#include "PKEditableVectorLabel.h"
#include "PKGridLayout.h"
#include "PKSelectorControl.h"
#include "PKIconTabControl.h"
#include "PKArrowTabControl.h"
#include "PKToolbarSegmentedItem.h"
#include "PKBitmapProgressBar.h"
#include "PKLittleArrows.h"
#include "PKUndoManager.h"
#include "PKVectorControlRoot.h"

#ifdef WIN32

void PKDialogScrollViewfunc(PKObject *child, void *context)
{
	PKScrollView *scroll = dynamic_cast<PKScrollView*>(child);

	if(scroll)
	{
		SendMessage(scroll->getWindowHandle(), WM_MOUSEWHEEL, (WPARAM)context, 0);
	}
}

void PKChildrenWalkdefaultButton(PKObject *child, void *context)
{
    PKDialog *dialog = (PKDialog *) context;
    PKButton *button = dynamic_cast<PKButton*>(child);

    if(button)
    {
        PKVariant *d = button->get(PKButton::DEFAULT_BOOL_PROPERTY);
        bool def = PKVALUE_BOOL(d);

        if(def)
        {
            PKVariant *id = button->get(PKControl::ID_STRING_PROPERTY);
            std::string bid = PKVALUE_STRING(id);
            dialog->buttonClicked(bid);
        }
    }
}

//
// This is the dialog window proc
// we override the original window
// proc from PKWindow to be able to
// catch WM_COMMAND messages and then
// we call the original proc
//

extern HWND pkActiveWindow;

LRESULT CALLBACK PKDialog::DialogWinProc(HWND hWnd, 
										 UINT msg, 
										 WPARAM wParam, 
										 LPARAM lParam)
{
	PKDialog *dialog = (PKDialog *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(dialog == NULL)
	{
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}

	switch(msg)
	{
	case WM_ACTIVATE:
		{
		  if (0 == wParam) // becoming inactive
               pkActiveWindow = NULL;
          else // becoming active
               pkActiveWindow = hWnd;
		}
		break;

	case WM_COMMAND:
		{
			// Find which control is sending this
			// and fire the appropriate callback

			uint32_t msg = HIWORD(wParam);
			uint32_t id  = LOWORD(wParam);

            if(msg == 0 && id == 1) // Happens when enter key is hit (why??)
            {
                // Find the default button if
                // any and push it
                dialog->walkChildren(PKChildrenWalkdefaultButton, dialog);
            }

			PKControl *obj = PKControl::getControlWithId(id);

			if(obj != NULL)
			{
				std::string cid = obj->getControlId();

                if(msg == STN_CLICKED)
                {
                    PKLink *link = dynamic_cast<PKLink*>(obj);

                    if(link)
                    {
                        // Handle the click
                        link->clicked();
                    }
                }

                if(msg == CBN_SELCHANGE)
				{
					PKComboBox *cbox = dynamic_cast<PKComboBox *>(obj);

					if(cbox != NULL)
					{
						int32_t newValue = (int32_t) SendMessage(obj->getWindowHandle(), CB_GETCURSEL, 0, 0);
						int32_t oldValue = PKVALUE_INT32(obj->get(PKComboBox::CURRENTITEM_INT32_PROPERTY));

						PKVariantInt32 newV(newValue);
						cbox->set(PKComboBox::CURRENTITEM_INT32_PROPERTY, &newV);

						dialog->comboBoxChanged(cid, oldValue);
					}
				}

				if(msg == LBN_SELCHANGE)
				{
					PKListBox *lbox = dynamic_cast<PKListBox *>(obj);

					if(lbox != NULL)
					{
						int32_t newValue = (int32_t) SendMessage(obj->getWindowHandle(), LB_GETCURSEL, 0, 0);
						int32_t oldValue = PKVALUE_INT32(obj->get(PKListBox::SELECTED_ITEM_INT32_PROPERTY));

						PKVariantInt32 newV(newValue);
						obj->set(PKListBox::SELECTED_ITEM_INT32_PROPERTY, &newV);

						dialog->listBoxSelectionChanged(cid, oldValue);
					}
				}

                if(msg == EN_CHANGE)
                {
                    int length    = Edit_GetTextLength((HWND)lParam);
                    wchar_t *text = new wchar_t[length+1];
                    Edit_GetText((HWND)lParam, text, length+1);
                    std::wstring t = text;

					PKVariantWString wt(t);
					obj->set(PKEdit::TEXT_WSTRING_PROPERTY, &wt);

                    dialog->editTextChanged(cid, t);
                    delete [] text;
                }

				if(dynamic_cast<PKButton*>(obj))
				{
					dialog->buttonClicked(cid);
				}
				
				if(dynamic_cast<PKGroupBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKGroupBox::CHECKED_BOOL_PROPERTY));
					dialog->checkBoxAction(cid, oldValue);
				}				

				if(dynamic_cast<PKCheckBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKCheckBox::CHECKED_BOOL_PROPERTY));

					LRESULT result = SendMessage(obj->getWindowHandle(), BM_GETCHECK, 0, 0);
					bool newValue = (result == BST_CHECKED); 

					PKVariantBool newV(newValue);
					obj->set(PKCheckBox::CHECKED_BOOL_PROPERTY, &newV);
					dialog->checkBoxAction(cid, oldValue);
				}				
			}
		}
		break;

    case WM_INITMENUPOPUP:
        {
            HMENU     menu = (HMENU) wParam;
            PKMenu *pkmenu = dialog->menu;

			// Run the callback for the menu items of this menu

            if(menu && pkmenu)
            {
				PKMenu *sub_menu = pkmenu->findSubmenuByHandle((void*) menu);

				if(sub_menu)
				{
					uint32_t count = sub_menu->getChildrenCount();

					for(uint32_t i=0; i < count; i++)
					{
						PKObject *child = sub_menu->getChildAt(i);

						PKMenuItem *item = dynamic_cast<PKMenuItem*>(child);

						if(item)
						{
							std::string id = PKVALUE_STRING(item->get(PKMenuItem::ID_STRING_PROPERTY));
							dialog->menuItemAboutToBeShown(sub_menu, item, id);
						}
					}
				}
			}

			// Handle undo/redo
			// Handle cut/copy/paste

            if(menu && pkmenu)
            {
                PKMenu *editMenu = pkmenu->findSubmenu("MENU_ID_EDIT");

                if(editMenu)
                {
					// We should first let the dialog know we are
					// abouto to show the edit menu, and therefore
					// all changes to edit controls should be commited
					dialog->commitEditControlChanges();

                    if(editMenu->getMenuHandle() == menu)
                    {
                        // Update cut/copy/paste items

                        PKClipboard clip;
                        editMenu->setItemEnabled("MENU_ID_PASTE", clip.containsText());

                        HWND focused = GetFocus();

                        editMenu->setItemEnabled("MENU_ID_CUT",  false);
                        editMenu->setItemEnabled("MENU_ID_COPY", false);

                        if(focused)
                        {
		                    PKControl *rootControl = dynamic_cast<PKControl*>(dialog->getChildAt(0));

                            if(rootControl)
                            {
                                PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWindow(focused));

                                if(focusControl && focusControl->hasSelection())
                                {
                                    editMenu->setItemEnabled("MENU_ID_CUT",  true);
                                    editMenu->setItemEnabled("MENU_ID_COPY", true);
                                }
                            }
                        }

						// Update undo/redo items

						if(PKUndoManager::getInstance()->canUndo())
						{
							std::wstring wtext = PK_i18n(L"Undo \"%1\"");
							wtext = PKStr::replaceString(L"%1", PKUndoManager::getInstance()->getUndoCommandDisplayName(), wtext);

							editMenu->updateItemText("MENU_ID_UNDO", wtext);
							editMenu->setItemEnabled("MENU_ID_UNDO", true);
						}
						else
						{
							editMenu->updateItemText("MENU_ID_UNDO", PK_i18n(L"Undo"));
							editMenu->setItemEnabled("MENU_ID_UNDO", false);
						}

						if(PKUndoManager::getInstance()->canRedo())
						{
							std::wstring wtext = PK_i18n(L"Redo \"%1\"");
							wtext = PKStr::replaceString(L"%1", PKUndoManager::getInstance()->getRedoCommandDisplayName(), wtext);

							editMenu->updateItemText("MENU_ID_REDO", wtext);
							editMenu->setItemEnabled("MENU_ID_REDO", true);
						}
						else
						{
							editMenu->updateItemText("MENU_ID_REDO", PK_i18n(L"Redo"));
							editMenu->setItemEnabled("MENU_ID_REDO", false);
						}
					}
                }
            } 
        }
        break;

   	case WM_CTLCOLORBTN:
        {
			HWND controlHWND = (HWND) lParam;
			HDC  hdc         = (HDC)  wParam;

			if(dialog->getChildrenCount() == 1)
			{
				PKControl *root = dynamic_cast<PKControl*>(dialog->getChildAt(0));

				if(root)
				{
    				PKView *obj = root->findObjectByWindow(controlHWND);

				    if(obj != NULL)
				    {
                        PKGroupBox *group = dynamic_cast<PKGroupBox*>(obj);

                        if(group)
                        {
                            return (BOOL) COLOR_3DFACE;
                        }

                        PKCheckBox *check = dynamic_cast<PKCheckBox*>(obj);
                        
                        if(check)
                        {
                            PKVariant *tr = check->get(PKCheckBox::TRANSPARENT_BOOL_PROPERTY);
                            bool transparent = PKVALUE_BOOL(tr);

                            if(transparent)
                            {
                                SetBkMode(hdc, TRANSPARENT);
                                return (LRESULT) GetStockObject(HOLLOW_BRUSH);
                            }
                        }
                    }
                }
            }
        }
        break;

	case WM_HSCROLL:
		{
			HWND controlHWND = (HWND) lParam;
            WORD action      = LOWORD(wParam);

			if(dialog->getChildrenCount() == 1)
			{
				PKControl *root = dynamic_cast<PKControl*>(dialog->getChildAt(0));

				if(root)
				{
    				PKView *obj = root->findObjectByWindow(controlHWND);

				    if(obj != NULL)
				    {
                        PKSlider *slider = dynamic_cast<PKSlider*>(obj);

                        if(slider && 
                          (action != SB_THUMBTRACK)) //TODO: Live tracking or NOT (add SB_THUMBTRACK)?
					    {
							int32_t previousValue = PKVALUE_INT32(slider->get(PKSlider::VALUE_INT32_PROPERTY));
							int32_t newValue = (int32_t) SendMessage(slider->getWindowHandle(), TBM_GETPOS, 0, 0);

							PKVariantInt32 newV(newValue);
							slider->set(PKSlider::VALUE_INT32_PROPERTY, &newV);

					    	dialog->sliderChanged(slider->getControlId(), previousValue);
					    }
				    }
			    }
            }
		}
		break;

    case WM_MEASUREITEM:
		{
            if(wParam == 0)
            {
                // Menu Item
                MEASUREITEMSTRUCT *lpmis = (MEASUREITEMSTRUCT *) lParam;

                if (lpmis==NULL)
                    break;

                lpmis->itemWidth += 8;

                if (lpmis->itemHeight < 20)
                    lpmis->itemHeight = 20;

                return TRUE;
            }
        }
		break; 

	case WM_DRAWITEM:
		{
            if(wParam == 0)
            {
                // Menu
                if(dialog->menu != NULL)
                {
                    dialog->menu->drawItem(wParam, lParam);
                }

                return TRUE;
            }

			// This is called for owner drawn
			// static controls such as the 
			// DWPImage class when it needs to
			// have an alpha channel...

			DRAWITEMSTRUCT *drawStruct = (DRAWITEMSTRUCT *) lParam;

			if(drawStruct != NULL)
			{
				if(dialog->getChildrenCount() == 1)
				{
					PKControl *root = dynamic_cast<PKControl*>(dialog->getChildAt(0));

					if(root)
					{
						PKView *obj = root->findObjectByWindow(drawStruct->hwndItem);

						if(obj != NULL)
						{
							if(dynamic_cast<PKImageView*>(obj))
							{
								PKImageView *img = dynamic_cast<PKImageView*>(obj);

								HBRUSH brush = NULL;
                                
                                // See if the image is a child of a control
                                // that draws its background 

                                if(!img->parentDrawsItsBackground())
                                {
                                    brush = (HBRUSH) GetClassLongPtr(dialog->window, GCLP_HBRBACKGROUND);
                                }

								// Draw the image

								img->alphaDraw(drawStruct->hDC, 
											   drawStruct->rcItem, 
											   brush);
								
								return TRUE;
							}
						}
					}
				}
			}
		}
		break;

	case WM_CTLCOLORSTATIC:
		{
			HWND control = (HWND) lParam;
			HDC  hdc     = (HDC)  wParam;

            // Handle links & panels

			if(dialog->getChildrenCount() == 1)
    		{
	    		PKControl *root = dynamic_cast<PKControl*>(dialog->getChildAt(0));

		    	if(root)
			    {
				    PKView *obj = root->findObjectByWindow(control);

                    if(obj != NULL)
	    	        {
                        // Check if it's a link

                        PKLink *link = dynamic_cast<PKLink*>(obj);

                        if(link)
                        {
                            PKVariant *color = link->get(PKLink::COLOR_COLOR_PROPERTY);
                            PKColor c = PKVALUE_COLOR(color);

                            SetBkMode(hdc, TRANSPARENT);
                            SetTextColor(hdc, RGB(c.r, c.g, c.b));
                        }

                        // Check if it's a label

                        PKLabel *label = dynamic_cast<PKLabel*>(obj);

                        if(label)
                        {
                            PKVariant *color = label->get(PKLink::COLOR_COLOR_PROPERTY);
                            PKColor c = PKVALUE_COLOR(color);

                            SetBkMode(hdc, TRANSPARENT);
                            SetTextColor(hdc, RGB(c.r, c.g, c.b));
                        }

                        // Check for a checkbox

						bool transparentCheck = false;

                        PKCheckBox *check = dynamic_cast<PKCheckBox*>(obj);
                        
                        if(check)
                        {
                            PKVariant *tr = check->get(PKCheckBox::TRANSPARENT_BOOL_PROPERTY);
                            transparentCheck = PKVALUE_BOOL(tr);

                            if(!transparentCheck)
                            {
								LRESULT result = CallWindowProc(dialog->lpOldProc, hWnd, msg, wParam, lParam);
								return result;
							}
                        }

                        // Check if the object is in a panel

                        PKObject *item = obj;

                        do
                        {
                            item = item->getParent();

                            if(item != NULL)
                            {
                                PKPanelControl  *panel1 = dynamic_cast<PKPanelControl *>(item);
                                PKGradientPanel *panel2 = dynamic_cast<PKGradientPanel *>(item);
                                PKScrollView    *scroll = dynamic_cast<PKScrollView *>(item);
                                PKImageView     *image  = dynamic_cast<PKImageView *>(item);

                                if(panel1)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) panel1->getBackgroundBrush();
                                    break;
                                }

                                if(panel2)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) GetStockObject(HOLLOW_BRUSH);
                                    break;
                                }

                                if(image)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) GetStockObject(HOLLOW_BRUSH);
                                    break;
                                }

                                if(scroll)
                                {
                                    SetBkMode(hdc, TRANSPARENT);
                                    return (LRESULT) scroll->getBackgroundBrush();
                                    break;
                                }
                            }
                        }
                        while(item != NULL);

                        // If we haven't hit anything
                        // we must return the brush for
                        // ourselves

                        HBRUSH brush = (HBRUSH) GetClassLongPtr(dialog->window, GCLP_HBRBACKGROUND);
                        return (LRESULT) brush;
                    }
                }
            }
		}
		break;

    case WM_NOTIFY:
		{
			// Looks for tab controls messages
			// that we get when the user switches
			// to a new tab, so we can show it

			NMHDR *nmhdr = (NMHDR *) lParam;

            PKControl *root = NULL;

            if(dialog->getChildrenCount() == 1)
			{
				root = dynamic_cast<PKControl*>(dialog->getChildAt(0));
            }

			if(nmhdr != NULL && root != NULL)
			{
				if(nmhdr->code == TVN_SELCHANGED)
				{
					// TreeView control selection
					PKTreeView *tree = dynamic_cast<PKTreeView*>(root->findObjectByWindow(nmhdr->hwndFrom));

					if(tree)
					{
						HTREEITEM item  = (HTREEITEM) TreeView_GetSelection(nmhdr->hwndFrom);
						std::string iid = tree->getIdForItem(item);

						dialog->treeViewItemSelected(iid);
					}
				}

				if(nmhdr->code == TCN_SELCHANGE)
				{
					// Check which tab this is
					// and what tab is currently
					// selected

					uint32_t tabId = (uint32_t) nmhdr->idFrom;

                    PKControl *tab = PKControl::getControlWithId(tabId);

					if(tab != NULL)
					{
                        PKTabControl *tabControl = dynamic_cast<PKTabControl*>(tab);

						if(tabControl)
						{
							uint32_t  currentTab = TabCtrl_GetCurSel(nmhdr->hwndFrom);
							tabControl->setCurrentTab(currentTab);

							return 0;
						}
					}
				}
			}
		}
		break;

    // Bitmap Button

    case WM_BITMAP_BUTTON_FIRE:
    case WM_STRETCH_BITMAP_BUTTON_FIRE:
        {
            HWND controlHWND = (HWND) wParam;

            PKControl *root = NULL;

            if(dialog->getChildrenCount() == 1)
			{
				root = dynamic_cast<PKControl*>(dialog->getChildAt(0));
            }

            if(root)
            {
                PKView *obj = root->findObjectByWindow(controlHWND);

			    if(obj != NULL)
			    {
                    dialog->buttonClicked(((PKControl*)obj)->getControlId());
                }
            }
        }
        break;

    // Toggle buttons
        
	case WM_MOUSEWHEEL:
		{  
            PKControl *root = NULL;

            if(dialog->getChildrenCount() == 1)
			{
				root = dynamic_cast<PKControl*>(dialog->getChildAt(0));
            }

            if(root)
            {
				int x;
				int y;
				unsigned int mod;

				int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

				dialog->getMouseParams(wParam, lParam, &x, &y, &mod);	

				POINT pt; pt.x = x; pt.y = y;
				ScreenToClient(hWnd, &pt);
				// ENHANCE: this only searches immediate children
				HWND childHWND = ChildWindowFromPoint(hWnd, pt);

				if(childHWND)
				{
					PKView *child = root->findObjectByWindow(childHWND);

					if(child)
					{
						child->walkParents(PKDialogScrollViewfunc, (void*)wParam);
					}
				}
			}
		}
		break;
    }   

	LRESULT result = CallWindowProc(dialog->lpOldProc, hWnd, msg, wParam, lParam);

    if(msg == WM_KEYDOWN    ||
       msg == WM_KEYUP      ||
       msg == WM_SYSKEYDOWN ||
       msg == WM_SYSKEYUP)
    {
        result = DefDlgProc(hWnd, msg, wParam, lParam);
    }

	return result;
}

#endif

#ifdef MACOSX

// This is the mac event handler
// control control notifications

pascal OSStatus PKDialog::PKDialogEventHandler(EventHandlerCallRef inHandlerRef, EventRef inEvent, void *inUserData)
{
    OSStatus result = eventNotHandledErr;
    
	PKDialog *dialog = (PKDialog *) inUserData;
	
	if(dialog == NULL)
	{
		return result;
	}

    UInt32 eventClass = GetEventClass(inEvent);

    switch(eventClass)
    {
		case kEventClassWindow:
		{
			UInt32 eventKind = GetEventKind(inEvent);
			
			if(eventKind = kEventWindowCursorChange)
			{
				// See if we are hitting a link control
				// and if we are, tell it to change it's cursor
				
				Point point;
				GetEventParameter(inEvent, kEventParamMouseLocation, typeQDPoint,
								  nil, sizeof(point), nil, &point);
				
				
				point.h -= dialog->getClientX();
				point.v -= dialog->getClientY();
				
				// Need to find the link controls
				// and call HandleControlSetCursor on them
				
				ControlPartCode code;
				
				ControlRef theControl = FindControlUnderMouse(point, dialog->macwindow, &code);
				
				bool handled = false;
				
				if(theControl)
				{
					if(dialog->getChildrenCount() == 1)
					{
						PKControl *root = dynamic_cast<PKControl*>(dialog->getChildAt(0));
						
						if(root)
						{
							PKView *obj = root->findObjectByControlRef(theControl);
							
							if(obj)
							{
								PKLink *link = dynamic_cast<PKLink*> (obj);
								
								if(link)
								{
									Boolean set = false;
									OSStatus err = HandleControlSetCursor(theControl, point, 0, &set);
									
									if(err == noErr)
									{
										handled = true;
										result = noErr;
									}
								}
							}
						}
					}
				}
				
				if(!handled)
				{
					Boolean set = false;
					OSStatus err = HandleControlSetCursor(theControl, point, 0, &set);
					if(err != noErr)
					{
						PKCursorTools::setCursor(PK_CURSOR_ARROW);
					}
				}
			}
		}
		break;
			
		case kEventClassControl:
		{
			ControlRef theControl;
			ControlID  controlID;
    
			GetEventParameter(inEvent, kEventParamDirectObject, typeControlRef, NULL, sizeof( ControlRef ), NULL, &theControl );
			GetControlID(theControl, &controlID);
	
			if(controlID.id != (SInt32) controlID.signature)
			{
				// Not one of our controls
				return result;
			}
			
			PKControl *obj = PKControl::getControlWithId(controlID.id);
			
			if(obj != NULL)
			{
				// Check for a tab selection change
				
				PKTabControl *tabControl = dynamic_cast<PKTabControl*>(obj);
				
				if(tabControl)
				{
					uint32_t currentTab = GetControlValue(theControl);
					tabControl->setCurrentTab(currentTab-1);
				}
				
				// Check for a button
				
				if(dynamic_cast<PKButton*>(obj))
				{
					dialog->buttonClicked(obj->getControlId());
				}
				
				// Check for a checkbox
				
				if(dynamic_cast<PKGroupBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKGroupBox::CHECKED_BOOL_PROPERTY));
					dialog->checkBoxAction(obj->getControlId(), oldValue);
				}
				
				if(dynamic_cast<PKCheckBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKCheckBox::CHECKED_BOOL_PROPERTY));
					
					PKCheckBox *box = dynamic_cast<PKCheckBox*>(obj);
					PKVariantBool newval(box->getChecked());
					box->set(PKCheckBox::CHECKED_BOOL_PROPERTY, &newval);
					
					dialog->checkBoxAction(obj->getControlId(), oldValue);
				}
				
				// Check for combobox
				
				if(dynamic_cast<PKComboBox*>(obj))
				{
					int32_t oldValue = PKVALUE_INT32(obj->get(PKComboBox::CURRENTITEM_INT32_PROPERTY));
					
					PKComboBox *box = dynamic_cast<PKComboBox*>(obj);
					PKVariantInt32 newval(box->getSelectedItem());
					box->set(PKComboBox::CURRENTITEM_INT32_PROPERTY, &newval);
					
					dialog->comboBoxChanged(obj->getControlId(), oldValue);
				}
				
				// List box
				
				if(dynamic_cast<PKListBox*>(obj))
				{
					int32_t oldValue = PKVALUE_INT32(obj->get(PKListBox::SELECTED_ITEM_INT32_PROPERTY));
					
					PKListBox *box = dynamic_cast<PKListBox*>(obj);
					PKVariantInt32 newval(box->getCurrentSelection());
					box->set(PKListBox::SELECTED_ITEM_INT32_PROPERTY, &newval);
					
					dialog->listBoxSelectionChanged(obj->getControlId(), oldValue);
				}
				
				// Little Arrows
				
				if(dynamic_cast<PKLittleArrows *> (obj))
				{
					PKLittleArrows *arrows = dynamic_cast<PKLittleArrows *> (obj);
					ControlPartCode code;
					
					GetEventParameter(inEvent, 
									  kEventParamControlPart, typeControlPartCode, 
									  NULL, sizeof( ControlPartCode ), NULL, &code );
					
					switch(code)
					{
						case kControlUpButtonPart:
						{
							arrows->handleValueUp();
						}
							break;
							
						case kControlDownButtonPart:
						{
							arrows->handleValueDown();
						}
							break;
					}					
				}
			}
		}
		break;
		
		case kEventClassPKSlider:
		{
			// Find out which slider it was

			int32_t id  = 0;
			void *param = NULL;
				
			GetEventParameter(inEvent, kEventParamUserData, typeVoidPtr,
							  nil, sizeof(param), nil, &param);
				
			id = (int32_t) param;				
			PKControl *obj = PKControl::getControlWithId(id);
			
			if(obj != NULL)
			{
				int32_t oldValue = PKVALUE_INT32(obj->get(PKSlider::VALUE_INT32_PROPERTY));
				dialog->sliderChanged(obj->getControlId(), oldValue);
			}
		}
		break;

		case kEventClassPKButton:
		case kEventClassPKStretchButton:
		{
			// Find out which button it was
			
			int32_t id  = 0;
			void *param = NULL;
			
			GetEventParameter(inEvent, kEventParamUserData, typeVoidPtr,
							  nil, sizeof(param), nil, &param);
			
			id = (int32_t) param;				
			PKControl *obj = PKControl::getControlWithId(id);
			
			if(obj != NULL)
			{
				dialog->buttonClicked(obj->getControlId());
			}
		}
		break;
									
	}
	

	return result;
}

#endif

PKDialog::PKDialog(std::wstring   id, // unique
				   PKWindowStyle  flags,
				   PKWindowHandle parent,
                   std::wstring   icon)

: PKWindow(id, PK_WINDOW_STYLE_DEFAULT | flags, parent, icon)
{
    this->built       = false;
	this->toolbar     = NULL;
	this->hasChildren = true;
	
#ifdef WIN32

	// Install the new window proc

    this->lpOldProc = SubclassWindow(this->window, (WNDPROC) DialogWinProc);
	SetWindowLongPtr(this->window, GWLP_USERDATA, (LONG)(LPVOID) (PKDialog *) this);

#endif

#ifdef LINUX

    this->current_focus = NULL;

    if(this->window != NULL)
    {
        gtk_window_set_policy(GTK_WINDOW(this->window), TRUE, TRUE, TRUE);
    }

#endif


#ifdef MACOSX

	// Install the addition event handler
	
	EventTypeSpec    eventTypes[5];
    
	eventTypes[0].eventClass = kEventClassControl;
	eventTypes[0].eventKind  = kEventControlHit;
			
	eventTypes[1].eventClass = kEventClassPKSlider;
	eventTypes[1].eventKind  = kEventPKSliderTrack;
	
	eventTypes[2].eventClass = kEventClassWindow;
	eventTypes[2].eventKind  = kEventWindowCursorChange;
			
	eventTypes[3].eventClass = kEventClassPKButton;
	eventTypes[3].eventKind  = kEventPKButtonTrack;

	eventTypes[4].eventClass = kEventClassPKStretchButton;
	eventTypes[4].eventKind  = kEventPKStretchButtonTrack;
	
	InstallEventHandler(GetWindowEventTarget(this->macwindow),
						NewEventHandlerUPP( PKDialog::PKDialogEventHandler ),
						5, eventTypes,
						(void *) this,
						&this->handler);	

#endif

    this->currentView = NULL;
    this->downView = NULL;
}

PKDialog::~PKDialog()
{
#ifdef WIN32
	SubclassWindow(this->window, (WNDPROC) this->lpOldProc);
	SetWindowLongPtr(this->window, GWLP_USERDATA, (LONG)(LPVOID) (PKWindow *) this);
#endif

#ifdef MACOSX
	RemoveEventHandler(this->handler);
#endif

    if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
            root->destroy();
        }
    }
}

void PKDialog::addFactory(IPKObjectFactory *factory)
{
    this->factories.push_back(factory);
}

bool PKDialog::loadFromXML(std::string xml)
{
	PKUIFactory factory;
	PKObjectLoader loader;

	loader.addObjectFactory(&factory);

    for(uint32_t i=0; i < this->factories.size(); i++)
    {
        loader.addObjectFactory(this->factories[i]);
    }
	
	PKObject *root = loader.loadFromXML(xml);
	
	if(root == NULL)
	{
		return false;
	}
	
	PKControl *rootControl = dynamic_cast<PKControl*>(root);
	
	if(rootControl == NULL)
	{
		return false;
	}
	
	this->addChild(rootControl);
	this->build();
	
	return true;
}

void PKDialog::build()
{
    if(this->built)
	{
	    return;
	}

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
#ifdef LINUX    
            if(this->toolbar != NULL)
            {
                root->prependChild(this->toolbar);
            }

            if(this->menu != NULL)
            {
                root->prependChild(this->menu);
            }
#endif	

			root->build();
			
#ifdef LINUX    
            if(this->window != NULL)
            {
                GtkWidget *root_widget = root->getWindowHandle();
                gtk_container_add(GTK_CONTAINER(this->window), root_widget);
            }
#endif	
		
            root->initialize(this);
            this->built = true;
		}
	}

    this->resizeToFit();
}

void PKDialog::resizeToFit()
{
#ifdef LINUX

    if(this->window != NULL)
    {
	    if(this->getChildrenCount() == 1)
	    {
		    PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

		    if(rootControl)
		    {
		        GtkWidget *ww = rootControl->getWindowHandle();
		        
                GtkRequisition req;
                gtk_widget_size_request(this->window, &req);

                gtk_window_set_resizable(GTK_WINDOW(this->window), TRUE);
                                
                gtk_window_set_default_size(GTK_WINDOW(this->window), req.width, req.height);
                gtk_window_resize(GTK_WINDOW(this->window), req.width, req.height);

                if(this->style & PK_WINDOW_STYLE_NO_RESIZE || 
                   this->style & PK_WINDOW_STYLE_NO_MAX)
                {
                    GdkGeometry geo;
                    
                    geo.min_width   = req.width;
                    geo.min_height  = req.height;
                    geo.max_width   = req.width;
                    geo.max_height  = req.height;
                    geo.base_width  = req.width;
                    geo.base_height = req.height;
                
                    GdkWindowHints hints = (GdkWindowHints) (GDK_HINT_BASE_SIZE | GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);
                
                    gtk_window_set_geometry_hints(GTK_WINDOW(this->window), 
                                                  GTK_WIDGET(ww), &geo, hints);           
                    
                    gtk_window_set_resizable(GTK_WINDOW(this->window), FALSE);
                }
            }
        }
    }

    return;

#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(rootControl)
		{
			int minW = rootControl->getMinimumWidth();
			int minH = rootControl->getMinimumHeight();

			this->resizeClientAreaTo(minW, minH);
		}
	}
}

void PKDialog::setToolbar(PKToolbar *tb)
{
	if(tb == NULL)
	{
		return;
	}
	
	this->toolbar = tb;
	tb->setWindow(this);

#ifdef WIN32

    if(this->getChildrenCount() == 0)
    {
        return;
    }        

    // We must add the toolbar as our child
    // So let's create a layout for it, and
    // add it as a child of the layout

    PKObject *currentChild = NULL;

    if(this->getChildrenCount() == 1)
    {
        currentChild = this->getChildAt(0);
    }

    if(currentChild)
    {
        // Remove current child

        this->removeChild(currentChild);

        // Setup the toolbar

        PKVariantInt32 layoutx(PK_LAYOUT_MAX);
        tb->set(PKControl::LAYOUTX_INT32_PROPERTY, &layoutx);

        PKVariantInt32 layouty(PK_LAYOUT_FIXED);
        tb->set(PKControl::LAYOUTY_INT32_PROPERTY, &layouty);

        PKVariantRect frame(0, 0, 50, PKToolbar::PK_TOOLBAR_HEIGHT);
        tb->set(PKControl::FRAME_RECT_PROPERTY, &frame);

        // Add a new Vertical Layout

        PKVLayout *mainLayout = new PKVLayout();

        PKVariantInt32 margin(0);
        mainLayout->set(PKVLayout::MARGIN_INT32_PROPERTY, &margin);
    
        PKVariantInt32 spacing(0);
        mainLayout->set(PKVLayout::SPACING_INT32_PROPERTY, &spacing);

        mainLayout->addChild(toolbar);
        mainLayout->addChild(currentChild);

        this->addChild(mainLayout);
    }

    this->resizeToFit();

#endif

#ifdef LINUX

    PKVariantInt32 layoutx(PK_LAYOUT_MAX);
    tb->set(PKControl::LAYOUTX_INT32_PROPERTY, &layoutx);

    PKVariantInt32 layouty(PK_LAYOUT_FIXED);
    tb->set(PKControl::LAYOUTY_INT32_PROPERTY, &layouty);

    PKVariantRect frame(0, 0, 50, PKToolbar::PK_TOOLBAR_HEIGHT);
    tb->set(PKControl::FRAME_RECT_PROPERTY, &frame);

    PKVLayout *root = dynamic_cast<PKVLayout*>(this->getChildAt(0));
    
    if(root)
    {
        GtkWidget *glayout = root->getWindowHandle();
        
        if(glayout)
  	    {
            // Window was already built, let's insert the toolbar
            
            root->addChild(this->toolbar);
            
            tb->build();
            
            gtk_box_pack_start(GTK_BOX(glayout), this->toolbar->getWindowHandle(), FALSE, FALSE, 0);
            
            if(this->hasMenu)
                gtk_box_reorder_child(GTK_BOX(glayout), this->toolbar->getWindowHandle(), 1);
            else
                gtk_box_reorder_child(GTK_BOX(glayout), this->toolbar->getWindowHandle(), 0);
                
            tb->initialize(this);
            this->resizeToFit();
	    }        	    
	    else
  	    {
            // Window was NOT built yet, toolbar will be
            // inserted automatically when the window is built
	    }        	    
    }        	    
#endif

#ifndef LINUX
    tb->build();
#endif

#ifdef WIN32
    tb->initialize(this);
#endif

#ifdef MACOSX
	
	if(this->macwindow)
	{
		HIToolbarRef toolbar = (HIToolbarRef) tb->getToolbarHandle();
		
		SetWindowToolbar(this->macwindow, toolbar);
		ChangeWindowAttributes(this->macwindow, kWindowToolbarButtonAttribute, 0);
		ShowHideWindowToolbar(this->macwindow, true, false);	
		SetAutomaticControlDragTrackingEnabledForWindow(this->macwindow, true);
	}
	
#endif	

}

//
// i18n
//

void PKChildrenWalkI18N(PKObject *child, void *context)
{
    if(child != NULL)
    {
        PKControl *control = dynamic_cast<PKControl*>(child);

        if(control)
        {
            control->updateUIStrings();
        }

        PKPropertyBag *props = child->getProperties();
    
        if(props != NULL)
        {
            uint32_t count = props->getNumberOfProperties();

            for(uint32_t i=0; i < count; i++)
            {
                PKProperty *p = props->getPropertyAt(i);

                if(p != NULL)
                {
                    PKVariant *value = p->getValue();

                    if(value->_type == PK_VARIANT_WSTRING)
                    {
                        PKVariantWStringI18N *i18n = dynamic_cast<PKVariantWStringI18N *>(value);

                        if(i18n)
                        {
                            PKVariantWStringI18N *newVal = new PKVariantWStringI18N(i18n->_original);
                            newVal->_val = PK_i18n(i18n->_original);
                            p->setValue(newVal);
                            delete newVal;
                        }
                    }
                    else if(value->_type == PK_VARIANT_WSTRINGLIST)
                    {
                        PKVariantWStringListI18N *i18n = dynamic_cast<PKVariantWStringListI18N *>(value);

                        if(i18n)
                        {
                            PKVariantWStringListI18N *newVal = new PKVariantWStringListI18N(i18n->_original);
                            
                            std::wstring translations;

                            uint32_t count = newVal->_original.size();

                            for(uint32_t i=0; i < count; i++)
                            {
                                translations += newVal->_original[i];

                                if(i != count-1)
                                {
                                    translations += L",";
                                }
                            }

                            translations = PK_i18n(translations);

                            std::vector<std::wstring> newValue;
                            PKStr::explode(translations, L',', &newValue);

                            newVal->_val = newValue;

                            p->setValue(newVal);
                            delete newVal;
                        }
                    }
                }
            }
        }
    }
}

void PKDialog::updateUIStrings()
{
    bool hidden = false;

    if(this->isShowing())
    {
        hidden = true;
        this->hide();
        PKTime::sleep(500);
    }

	// Update our toolbar
	
#ifdef MACOSX
	
	if(this->toolbar != NULL)
	{
		ShowHideWindowToolbar(this->macwindow, false, false);	
		SetWindowToolbar(this->macwindow, NULL);
		ChangeWindowAttributes(this->macwindow, 0, kWindowToolbarButtonAttribute);

        this->toolbar->updateUIStrings();

		HIToolbarRef toolbar = (HIToolbarRef) this->toolbar->getToolbarHandle();
	
		SetWindowToolbar(this->macwindow, toolbar);
		ChangeWindowAttributes(this->macwindow, kWindowToolbarButtonAttribute, 0);
		ShowHideWindowToolbar(this->macwindow, true, false);	
		SetAutomaticControlDragTrackingEnabledForWindow(this->macwindow, true);	
	}
	
#endif

    //
    // Walk our children and update them
    //

    if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
            root->walkChildren(PKChildrenWalkI18N, NULL);  
        }
    }

#ifndef LINUX
    // TODO
    this->resizeToFit();
#endif

#if(defined(WIN32) || defined(LINUX))
	
	if(this->toolbar != NULL)
	{
        this->toolbar->updateUIStrings();
	}

#endif
	
    // Call PKWindow
    PKWindow::updateUIStrings();

    if(hidden)
    {
        this->show();
    }
}

void PKDialog::resizeWindowAreaTo(int w, int h)
{
	PKWindow::resizeWindowAreaTo(w, h);

	if(this->getChildrenCount() == 1)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(rootControl)
		{
			int cw = this->getClientWidth();
			int ch = this->getClientHeight();

			rootControl->resize(cw, ch);
		}
	}
}

void PKDialog::resizeClientAreaTo(int w, int h)
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(rootControl)
		{
			rootControl->resize(w, h);
		}
	}

    // Resize window
	PKWindow::resizeClientAreaTo(w, h);
}

void PKDialog::windowResized()
{
	PKWindow::windowResized();

	if(this->getChildrenCount() == 1)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(rootControl)
		{
			int w = this->getClientWidth();
			int h = this->getClientHeight();

            int rw = rootControl->getW();
            int rh = rootControl->getH();

            if(w != rw || h != rh)
            {
    			rootControl->resize(w, h);
            }
		}
	}
}

void PKDialog::windowGetMinMax(uint32_t *minWidth,
							   uint32_t *minHeight,
							   uint32_t *maxWidth,
							   uint32_t *maxHeight)
{
	PKControl *mainLayout = NULL;

	if(this->getChildrenCount() == 1)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(rootControl)
		{
			mainLayout = rootControl;
		}
	}

	if(mainLayout != NULL)
	{
#ifdef WIN32

		uint32_t w = mainLayout->getMinimumWidth();
		uint32_t h = mainLayout->getMinimumHeight();

		RECT client;
		client.left   = 0;
		client.top    = 0;
		client.right  = w;
		client.bottom = h;

		AdjustWindowRectEx(&client, this->wstyle, 
						   false, this->exStyle);

		*minWidth  = (client.right - client.left);
		*minHeight = (client.bottom - client.top);

#endif
		
	}
}

// Menu

void PKDialog::doCut()
{
#ifdef WIN32
    HWND focus = GetFocus();

    if(focus)
    {
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWindow(focus));

            if(focusControl)
            {
                focusControl->doCut();
            }
        }
    }
#endif
	
#ifdef LINUX
    if(this->window)
    {    
        GtkWidget *focused = gtk_window_get_focus(GTK_WINDOW(this->window));
        
        if(focused)
        {
    		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

            if(rootControl)
            {
                PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWidget(focused));

                if(focusControl)
                {
                    focusControl->doCut();
                }
            }                
        }        
    }
#endif

#ifdef MACOSX
	ControlRef outControl = NULL;
	GetKeyboardFocus(this->macwindow, &outControl);	
	
	if(outControl)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));
		
        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByControlRef(outControl));
			
            if(focusControl)
            {
                focusControl->doCut();
            }
        }
	}
	
#endif
}

void PKDialog::doCopy()
{
#ifdef WIN32
    HWND focus = GetFocus();

    if(focus)
    {
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWindow(focus));

            if(focusControl)
            {
                focusControl->doCopy();
            }
        }
    }
#endif

#ifdef LINUX
    if(this->window)
    {    
        GtkWidget *focused = gtk_window_get_focus(GTK_WINDOW(this->window));
        
        if(focused)
        {
    		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

            if(rootControl)
            {
                PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWidget(focused));

                if(focusControl)
                {
                    focusControl->doCopy();
                }
            }                
        }        
    }
#endif

#ifdef MACOSX
	ControlRef outControl = NULL;
	GetKeyboardFocus(this->macwindow, &outControl);	
	
	if(outControl)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));
		
        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByControlRef(outControl));
			
            if(focusControl)
            {
                focusControl->doCopy();
            }
        }
	}
	
#endif
}

void PKDialog::doPaste()
{
#ifdef WIN32
    HWND focus = GetFocus();

    if(focus)
    {
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWindow(focus));

            if(focusControl)
            {
                focusControl->doPaste();
            }
        }
    }
#endif

#ifdef LINUX
    if(this->window)
    {    
        GtkWidget *focused = gtk_window_get_focus(GTK_WINDOW(this->window));
        
        if(focused)
        {
    		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

            if(rootControl)
            {
                PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWidget(focused));

                if(focusControl)
                {
                    focusControl->doPaste();
                }
            }                
        }        
    }
#endif

#ifdef MACOSX
	ControlRef outControl = NULL;
	GetKeyboardFocus(this->macwindow, &outControl);	
	
	if(outControl)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));
		
        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByControlRef(outControl));
			
            if(focusControl)
            {
                focusControl->doPaste();
            }
        }
	}
	
#endif
}

void PKDialog::doSelectAll()
{
#ifdef WIN32
    HWND focus = GetFocus();

    if(focus)
    {
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWindow(focus));

            if(focusControl)
            {
                focusControl->doSelectAll();
            }
        }
    }
#endif

#ifdef LINUX
    if(this->window)
    {    
        GtkWidget *focused = gtk_window_get_focus(GTK_WINDOW(this->window));
        
        if(focused)
        {
    		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

            if(rootControl)
            {
                PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWidget(focused));

                if(focusControl)
                {
                    focusControl->doSelectAll();
                }
            }                
        }        
    }
#endif

#ifdef MACOSX
	ControlRef outControl = NULL;
	GetKeyboardFocus(this->macwindow, &outControl);	
	
	if(outControl)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));
		
        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByControlRef(outControl));
			
            if(focusControl)
            {
                focusControl->doSelectAll();
            }
        }
	}
	
#endif
}


// Accessors

PKControl *PKDialog::findControl(std::string id)
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(rootControl)
		{
			if(PKVALUE_STRING(rootControl->get(PKControl::ID_STRING_PROPERTY)) == id)
			{
				return rootControl;
			}

			return dynamic_cast<PKControl*>(rootControl->findObjectById(id));
		}
	}

	return NULL;
}

PKControl *PKDialog::getFocusedControl()
{
#ifdef WIN32
    HWND focus = GetFocus();

    if(focus)
    {
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWindow(focus));

            if(focusControl)
            {
                return focusControl;
            }
        }
    }
#endif
	
#ifdef LINUX
    if(this->window)
    {    
        GtkWidget *focused = gtk_window_get_focus(GTK_WINDOW(this->window));
        
        if(focused)
        {
    		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));

            if(rootControl)
            {
                PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByWidget(focused));

                if(focusControl)
                {
                    return focusControl;
                }
            }                
        }        
    }
#endif

#ifdef MACOSX
	ControlRef outControl = NULL;
	GetKeyboardFocus(this->macwindow, &outControl);	
	
	if(outControl)
	{
		PKControl *rootControl = dynamic_cast<PKControl*>(this->getChildAt(0));
		
        if(rootControl)
        {
            PKControl *focusControl = dynamic_cast<PKControl*>(rootControl->findObjectByControlRef(outControl));
			
            if(focusControl)
            {
                return focusControl;
            }
        }
	}
	
#endif

    return NULL;
}

// Utilities

void PKDialog::enableControl(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
		obj->enable();
	}
}

void PKDialog::disableControl(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
		obj->disable();
	}
}

void PKDialog::setFocusToControl(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
#ifdef WIN32
        SetFocus(obj->getWindowHandle());
#endif

#ifdef LINUX
        gtk_widget_grab_focus(obj->getWindowHandle());
#endif

#ifdef MACOSX
		ControlRef ref = obj->getControlRef();
		SetKeyboardFocus(GetControlOwner(ref), ref, kControlFocusNextPart);
#endif
    }
}

void PKDialog::invalidateControl(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        obj->invalidate();
    }
}

void PKDialog::relayoutControl(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        obj->relayout();
    }
}

void PKDialog::setControlWidth(std::string id, uint32_t w)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
		PKVariant *fr = obj->get(PKControl::FRAME_RECT_PROPERTY);
		PKRect frame  = PKVALUE_RECT(fr);
		
		frame.w = w;
		PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
		obj->set(PKControl::FRAME_RECT_PROPERTY, &newFrame);
	}
}

void PKDialog::setControlHeight(std::string id, uint32_t h)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
		PKVariant *fr = obj->get(PKControl::FRAME_RECT_PROPERTY);
		PKRect frame  = PKVALUE_RECT(fr);
		
		frame.h = h;
		PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
		obj->set(PKControl::FRAME_RECT_PROPERTY, &newFrame);
	}
}

void PKDialog::setLayoutMargin(std::string id, uint32_t margin)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
		PKLayout *layout = dynamic_cast<PKLayout*>(obj);
		
		if(layout)
		{
			PKVariantInt32 m(margin);
			layout->set(PKLayout::MARGIN_INT32_PROPERTY, &m);
		}
		
		PKGridLayout *grid = dynamic_cast<PKGridLayout*>(obj);
		
		if(grid)
		{
			PKVariantInt32 m(margin);
			grid->set(PKGridLayout::MARGIN_INT32_PROPERTY, &m);
		}
	}		
}

void PKDialog::setLayoutSpacing(std::string id, uint32_t spacing)
{	
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
		PKLayout *layout = dynamic_cast<PKLayout*>(obj);
		
		if(layout)
		{
			PKVariantInt32 s(spacing);
			layout->set(PKLayout::SPACING_INT32_PROPERTY, &s);
		}

		PKGridLayout *grid = dynamic_cast<PKGridLayout*>(obj);
		
		if(grid)
		{
			PKVariantInt32 s(spacing);
			grid->set(PKGridLayout::SPACING_INT32_PROPERTY, &s);
		}
	}		
}

void PKDialog::setControlEnabled(std::string id, bool enabled)
{
	if(enabled)
	{
		this->enableControl(id);
	}
	else
	{
		this->disableControl(id);
	}
}

void PKDialog::setControlTreeHidden(std::string id, bool hidden)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
		obj->setHiddenRecursive(hidden);
	}
}

void PKDialog::setControlHidden(std::string id, bool hidden)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
		obj->setHidden(hidden);
	}
}

bool PKDialog::getControlHidden(std::string id)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
		return obj->getHidden();
	}    

    return false;
}

void PKDialog::showControl(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
		obj->show();
	}
}

void PKDialog::hideControl(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
		obj->hide();
	}
}

//
// Controls Helpers
// 

void PKDialog::setButtonText(std::string id, std::wstring text)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKButton *button = dynamic_cast<PKButton*>(obj);

		if(button)
		{
			button->setCaption(text);
		}
	}
}

void PKDialog::setCheckBoxText(std::string id, std::wstring text)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKCheckBox *check = dynamic_cast<PKCheckBox*>(obj);

		if(check)
		{
			PKVariantWString t(text);
			check->set(PKCheckBox::TEXT_WSTRING_PROPERTY, &t);
		}
	}
}

void PKDialog::setCheckBoxChecked(std::string id, bool checked)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
		bool disabled = PKUndoManager::getInstance()->disable();

        PKCheckBox *check = dynamic_cast<PKCheckBox*>(obj);

		if(check)
		{
			PKVariantBool c(checked);
			check->set(PKCheckBox::CHECKED_BOOL_PROPERTY, &c);
		}

        PKGroupBox *group = dynamic_cast<PKGroupBox*>(obj);

        if(group)
		{
			group->setChecked(checked);
		}

		if(disabled)
		{
			PKUndoManager::getInstance()->enable();
		}
	}
}

bool PKDialog::getCheckBoxChecked(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKCheckBox *check = dynamic_cast<PKCheckBox*>(obj);

		if(check)
		{
			return check->getChecked();
		}

        PKGroupBox *group = dynamic_cast<PKGroupBox*>(obj);

        if(group)
		{
			return group->getChecked();
		}
	}

	return false;
}

void PKDialog::setEditText(std::string id, std::wstring text)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKEdit *edit = dynamic_cast<PKEdit*>(obj);

		if(edit)
		{
			bool disabled = PKUndoManager::getInstance()->disable();

			edit->setText(text);

			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
		}
	}
}

void PKDialog::resetEditChanges(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKEdit *edit = dynamic_cast<PKEdit*>(obj);

		if(edit)
		{
			edit->resetChanges();
		}
	}
}

std::wstring PKDialog::getEditText(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKEdit *edit = dynamic_cast<PKEdit*>(obj);

		if(edit)
		{
			return edit->getText();
		}
	}

	return L"";
}

bool PKDialog::getBitmapToggleButtonToggled(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKBitmapToggleButton *toggle = dynamic_cast<PKBitmapToggleButton*>(obj);

		if(toggle)
		{
			return PKVALUE_BOOL(toggle->get(PKBitmapToggleButton::TOGGLED_BOOL_PROPERTY));
		}
	}

	return false;
}

void PKDialog::setBitmapToggleButtonToggled(std::string id, bool toggled)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKBitmapToggleButton *toggle = dynamic_cast<PKBitmapToggleButton*>(obj);

		if(toggle)
		{
			bool disabled = PKUndoManager::getInstance()->disable();

			PKVariantBool t(toggled);
			toggle->set(PKBitmapToggleButton::TOGGLED_BOOL_PROPERTY, &t);

			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
		}
	}
}

void PKDialog::setEditableLabelText(std::string id, std::wstring text)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
        PKEditableVectorLabel *edit = dynamic_cast<PKEditableVectorLabel*>(obj);
		
		if(edit)
		{
			bool disabled = PKUndoManager::getInstance()->disable();

			PKVariantWString wstr(text);
			edit->set(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY, &wstr);

			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
		}
	}
}

std::wstring PKDialog::getEditableLabelText(std::string id)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
        PKEditableVectorLabel *edit = dynamic_cast<PKEditableVectorLabel*>(obj);
		
		if(edit)
		{
			PKVariant *t = edit->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
			return PKVALUE_WSTRING(t);
		}
	}
	
	return L"";
}

void PKDialog::resetEditableLabelChanges(std::string id)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
        PKEditableVectorLabel *edit = dynamic_cast<PKEditableVectorLabel*>(obj);
		
		if(edit)
		{
			edit->resetChanges();
		}
	}
}

void PKDialog::setLabelText(std::string id, std::wstring text)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKLabel *label = dynamic_cast<PKLabel*>(obj);

		if(label)
		{
			label->setText(text);
		}
	}
}

void PKDialog::clearComboBoxItems(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKComboBox *combo = dynamic_cast<PKComboBox*>(obj);

		if(combo)
		{
			combo->clearItems();
		}
	}
}

void PKDialog::addComboBoxItem(std::string id, std::wstring text)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKComboBox *combo = dynamic_cast<PKComboBox*>(obj);

		if(combo)
		{
			combo->addItem(text);
		}
	}
}

void PKDialog::selectComboBoxItem(std::string id, int index)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKComboBox *combo = dynamic_cast<PKComboBox*>(obj);

		if(combo)
		{
			bool disabled = PKUndoManager::getInstance()->disable();
			
			combo->selectItem(index);
			
			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
		}
	}
}

int PKDialog::getComboBoxSelectedItem(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKComboBox *combo = dynamic_cast<PKComboBox*>(obj);

		if(combo)
		{
			return combo->getSelectedItem();
		}
	}

	return -1;
}

std::wstring PKDialog::getComboBoxSelectedText(std::string id)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
        PKComboBox *combo = dynamic_cast<PKComboBox*>(obj);
		
		if(combo)
		{
			return combo->getSelectedText();
		}
	}
	
	return L"";
}

void PKDialog::setSliderValue(std::string id, int32_t value)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKSlider *slider = dynamic_cast<PKSlider*>(obj);

        if(slider)
		{
			bool disabled = PKUndoManager::getInstance()->disable();

			slider->setValue(value);

			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
		}
	}
}

int32_t PKDialog::getSliderValue(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKSlider *slider = dynamic_cast<PKSlider*>(obj);

        if(slider)
		{
			return slider->getValue();
		}
	}

	return 0;
}

void PKDialog::setProgressValue(std::string id, int32_t value)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKProgressBar *progress = dynamic_cast<PKProgressBar*>(obj);

		if(progress)
		{
			progress->setValue(value);
			return;
		}

		PKBitmapProgressBar *bitmap = dynamic_cast<PKBitmapProgressBar *>(obj);

		if(bitmap)
		{
			PKVariantInt32 val(value);
			bitmap->set(PKBitmapProgressBar::VALUE_INT32_PROPERTY, &val);
		}
	}
}

int32_t PKDialog::getProgressValue(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKProgressBar *progress = dynamic_cast<PKProgressBar*>(obj);

		if(progress)
		{
			return progress->getValue();
		}

		PKBitmapProgressBar *bitmap = dynamic_cast<PKBitmapProgressBar *>(obj);

		if(bitmap)
		{
			return PKVALUE_INT32(bitmap->get(PKBitmapProgressBar::VALUE_INT32_PROPERTY));
		}
	}

	return 0;
}

void PKDialog::startAnimation(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKAnimationControl *anim = dynamic_cast<PKAnimationControl*>(obj);

		if(anim)
		{
			anim->start();
		}
	}
}

void PKDialog::stopAnimation(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKAnimationControl *anim = dynamic_cast<PKAnimationControl*>(obj);

		if(anim)
		{
			anim->stop();
		}
	}
}


void PKDialog::loadImageFromResource(std::string id, std::wstring image)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKImageView *imageO = dynamic_cast<PKImageView*>(obj);

		if(imageO)
		{
			imageO->loadImageFromResource(image);
		}
	}
}

void PKDialog::loadImageFromFilename(std::string id, std::wstring image)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKImageView *imageO = dynamic_cast<PKImageView*>(obj);

		if(imageO)
		{
			imageO->loadImageFromFilename(image);
		}
	}
}

void PKDialog::setSelectorControlSelection(std::string id, int32_t selection)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKSelectorControl *sel = dynamic_cast<PKSelectorControl*>(obj);

        if(sel)
        {
			bool disabled = PKUndoManager::getInstance()->disable();
            
			PKVariantInt32 s(selection);
            sel->set(PKSelectorControl::SELECTION_INT32_PROPERTY, &s);
			
			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
        }
    }
}

int32_t PKDialog::getSelectorControlSelection(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKSelectorControl *sel = dynamic_cast<PKSelectorControl*>(obj);

        if(sel)
        {
            return PKVALUE_INT32(sel->get(PKSelectorControl::SELECTION_INT32_PROPERTY));
        }
    }

    return -1;
}

void PKDialog::setIconTabControlTab(std::string id, int32_t tabNum)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKIconTabControl *tab = dynamic_cast<PKIconTabControl*>(obj);

        if(tab)
        {
			bool disabled = PKUndoManager::getInstance()->disable();
            
			tab->switchToTab(tabNum);
			
			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
        }
    }
}

int32_t PKDialog::getIconTabControlTab(std::string id)
{
	PKControl *obj = this->findControl(id);

	if(obj != NULL)
	{
        PKIconTabControl *tab = dynamic_cast<PKIconTabControl*>(obj);

        if(tab)
        {
            return (int32_t) PKVALUE_UINT32(tab->get(PKIconTabControl::CURRENT_TAB_UINT32_PROPERTY));
        }
    }

    return -1;
}

void PKDialog::setArrowTabControlTab(std::string id, int32_t tabNum)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
        PKArrowTabControl *tab = dynamic_cast<PKArrowTabControl*>(obj);
		
        if(tab)
        {
			bool disabled = PKUndoManager::getInstance()->disable();

			tab->switchToTab(tabNum);

			if(disabled)
			{
				PKUndoManager::getInstance()->enable();
			}
        }
    }
}

int32_t PKDialog::getArrowTabControlTab(std::string id)
{
	PKControl *obj = this->findControl(id);
	
	if(obj != NULL)
	{
        PKArrowTabControl *tab = dynamic_cast<PKArrowTabControl*>(obj);
		
        if(tab)
        {
            return (int32_t) PKVALUE_UINT32(tab->get(PKArrowTabControl::CURRENT_TAB_UINT32_PROPERTY));
        }
    }
	
    return -1;
}

void PKDialog::setToolbarSegmentedItemSelection(std::string id, int32_t sel)
{
    if(this->toolbar)
    {
		bool disabled = PKUndoManager::getInstance()->disable();

		this->toolbar->setSegmentedItemSelection(id, sel);

		if(disabled)
		{
			PKUndoManager::getInstance()->enable();
		}
    }
}

void PKDialog::commitEditControlChanges()
{
	// See which control has the focus
	// and if we need to do anything about
	// it at this point

	PKControl *focused = this->getFocusedControl();

	if(focused != NULL)
	{
		// See if it is an edit control
		PKEdit *edit = dynamic_cast<PKEdit*>(focused);

		if(edit != NULL)
		{
			edit->commitChanges();
		}

		// Do the same for vector editable label control

        PKVectorControlRoot *root = dynamic_cast<PKVectorControlRoot*>(focused);

        if(root)
        {
		    root->commitChanges();
        }
    }
}

// From PKWindow

#ifdef WIN32

HBRUSH PKDialog::getBackgroundBrush()
{
    return GetSysColorBrush(COLOR_3DFACE);
}

#endif

//
// Mouse events
//
// Sent by the window when the mouse
// hovers, or is down/up as well as for
// double clicks and mouse wheel
//

void PKDialog::mouseDown(PKButtonType button,
					     int x,
					     int y,
						 unsigned int mod)
{
#ifdef WIN32

	POINT pt; pt.x = x; pt.y = y;
	
	HWND hwndItem = ChildWindowFromPoint(this->window, pt);
	
	if(hwndItem && this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));
		
		if(root)
		{
			PKView *obj = root->findObjectByWindow(hwndItem);
			
			if(obj != NULL)
			{
				if(dynamic_cast<PKCustomControl*>(obj))
				{
					PKCustomControl *custom = (PKCustomControl *) obj;
					
					ClientToScreen(this->window, &pt);
					ScreenToClient(hwndItem, &pt);
					
					custom->mouseDown(button, pt.x, pt.y, mod);
					this->downView = custom;
				}
			}
		}
	}
	
#endif
	
#ifdef MACOSX
	
	ControlPartCode controlId;
	
	Point pt;
	pt.h = x; pt.v = y;
	
	ControlRef theControl = FindControlUnderMouse(pt, this->macwindow, &controlId);

	if(theControl)
	{
		if(this->getChildrenCount() == 1)
		{
			PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));
			
			if(root)
			{
				PKView *obj = root->findObjectByControlRef(theControl);
				
				if(obj != NULL)
				{
					if(dynamic_cast<PKCustomControl*>(obj))
					{
						PKCustomControl *custom = (PKCustomControl *) obj;
						
						HIPoint point;
						point.x = 0;
						point.y = 0;
						
						WindowRef window = this->macwindow;
						
						HIViewRef mContent;
						
						HIViewFindByID(HIViewGetRoot(window),
									   kHIViewWindowContentID, &mContent);		
						
						HIViewConvertPoint(&point, custom->getControlRef(), mContent);
						
						int xx = (int) pt.h - (int) point.x + custom->getScrollOriginX();
						int yy = (int) pt.v - (int) point.y + custom->getScrollOriginY();
						
						custom->mouseDown(button, xx, yy, mod);
						this->downView = custom;
					}
				}
			}
		}
	}
		
#endif
}

void PKDialog::mouseUp(PKButtonType button,
					   int x,
					   int y,
					   unsigned int mod)
{
#ifdef WIN32

    POINT pt; pt.x = x; pt.y = y;

    if(this->downView != NULL)
    {
        PKCustomControl *customOld = (PKCustomControl *) this->downView;
        HWND hwndItem = customOld->getWindowHandle();

        ClientToScreen(this->window, &pt);
        ScreenToClient(hwndItem, &pt);

        customOld->mouseUp(button, pt.x, pt.y, mod);
        this->downView = NULL;
    }

#endif
	
#ifdef MACOSX
	
    Point pt; pt.h = x; pt.v = y;
	
    if(this->downView != NULL)
    {
        PKCustomControl *customOld = (PKCustomControl *) this->downView;
		
		HIPoint point;
		point.x = 0;
		point.y = 0;
		
		WindowRef window = this->macwindow;
		
		HIViewRef mContent;
		
		HIViewFindByID(HIViewGetRoot(window),
					   kHIViewWindowContentID, &mContent);		
		
		HIViewConvertPoint(&point, customOld->getControlRef(), mContent);
		
		int xx = (int) pt.h - (int) point.x + customOld->getScrollOriginX();
		int yy = (int) pt.v - (int) point.y + customOld->getScrollOriginY();
		
        customOld->mouseUp(button, xx, yy, mod);
        this->downView = NULL;
    }	
	
#endif
}

void PKDialog::mouseDblClick(PKButtonType button,
						     int x,
						     int y,
						     unsigned int mod)
{
#ifdef WIN32
    POINT pt; pt.x = x; pt.y = y;

    HWND hwndItem = ChildWindowFromPoint(this->window, pt);

	if(hwndItem && this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			PKView *obj = root->findObjectByWindow(hwndItem);

			if(obj != NULL)
			{
				if(dynamic_cast<PKCustomControl*>(obj))
				{
                    PKCustomControl *custom = (PKCustomControl *) obj;

                    ClientToScreen(this->window, &pt);
                    ScreenToClient(hwndItem, &pt);

                    custom->mouseDblClick(button, pt.x, pt.y, mod);
                    this->downView = custom;
                }
            }
        }
    }
#endif

#ifdef MACOSX
	
	ControlPartCode controlId;
	
	Point pt;
	pt.h = x; pt.v = y;
	
	ControlRef theControl = FindControlUnderMouse(pt, this->macwindow, &controlId);
	
	if(theControl)
	{
		if(this->getChildrenCount() == 1)
		{
			PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));
			
			if(root)
			{
				PKView *obj = root->findObjectByControlRef(theControl);
				
				if(obj != NULL)
				{
					if(dynamic_cast<PKCustomControl*>(obj))
					{
						PKCustomControl *custom = (PKCustomControl *) obj;
						
						HIPoint point;
						point.x = 0;
						point.y = 0;
						
						WindowRef window = this->macwindow;
						
						HIViewRef mContent;
						
						HIViewFindByID(HIViewGetRoot(window),
									   kHIViewWindowContentID, &mContent);		
						
						HIViewConvertPoint(&point, custom->getControlRef(), mContent);
						
						int xx = (int) pt.h - (int) point.x + custom->getScrollOriginX();
						int yy = (int) pt.v - (int) point.y + custom->getScrollOriginY();
						
						custom->mouseDblClick(button, xx, yy, mod);
						this->downView = custom;
					}
				}
			}
		}
	}
	
#endif	
}

void PKDialog::mouseMove(int x,
					     int y,
					     unsigned int mod)
{
#ifdef WIN32

    // We need to check where the event occured
    // and if there is a control underneath it

    POINT pt; pt.x = x; pt.y = y;

    HWND hwndItem = ChildWindowFromPoint(this->window, pt);;

	if(hwndItem && this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			PKView *obj = root->findObjectByWindow(hwndItem);

			if(obj != NULL)
			{
                if(this->currentView != NULL && this->currentView != obj)
                {
                    PKCustomControl *customOld = (PKCustomControl *) this->currentView;
                    customOld->mouseLeave();
                }

				if(dynamic_cast<PKCustomControl*>(obj))
				{
                    this->currentView = obj;
                    PKCustomControl *custom = (PKCustomControl *) obj;

                    ClientToScreen(this->window, &pt);
                    ScreenToClient(hwndItem, &pt);

                    custom->mouseMove(pt.x, pt.y, mod);
                }
            }
	        else
	        {
		        if(this->currentView != NULL)
		        {
			        PKCustomControl *customOld = (PKCustomControl *) this->currentView;
			        customOld->mouseLeave();
		        }
	        }
        }
    }
	else
	{
		if(this->currentView != NULL)
		{
			PKCustomControl *customOld = (PKCustomControl *) this->currentView;
			customOld->mouseLeave();
		}
	}
	
#endif
	
#ifdef MACOSX
	
	ControlPartCode controlId;
	
	Point pt;
	pt.h = x; pt.v = y;
	
	ControlRef theControl = FindControlUnderMouse(pt, this->macwindow, &controlId);
	
	if(theControl)
	{
		if(this->getChildrenCount() == 1)
		{
			PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));
			
			if(root)
			{
				PKView *obj = root->findObjectByControlRef(theControl);
				
				if(obj != NULL)
				{
					if(this->currentView != NULL && this->currentView != obj)
					{
						PKCustomControl *customOld = (PKCustomControl *) this->currentView;
						customOld->mouseLeave();
					}

					if(dynamic_cast<PKCustomControl*>(obj))
					{
						this->currentView = obj;
						PKCustomControl *custom = (PKCustomControl *) obj;
						
						HIPoint point;
						point.x = 0;
						point.y = 0;
						
						WindowRef window = this->macwindow;
						
						HIViewRef mContent;
						
						HIViewFindByID(HIViewGetRoot(window),
									   kHIViewWindowContentID, &mContent);		
						
						HIViewConvertPoint(&point, custom->getControlRef(), mContent);
						
						int xx = (int) pt.h - (int) point.x + custom->getScrollOriginX();
						int yy = (int) pt.v - (int) point.y + custom->getScrollOriginY();
						
						custom->mouseMove(xx, yy, mod);
					}
				}
			}
		}
	}
	else
	{
		if(this->currentView != NULL)
		{
			PKCustomControl *customOld = (PKCustomControl *) this->currentView;
			customOld->mouseLeave();
		}
	}
	
#endif
	
}

void PKDialog::mouseLeave()
{
    if(this->currentView != NULL)
    {
        PKCustomControl *customOld = (PKCustomControl *) this->currentView;
        customOld->mouseLeave();
    }
}

void PKDialog::mouseWheel(PKButtonType button,
						  int x,
						  int y,
						  unsigned int mod)
{

}

//
// keyDown() / keyUp()
//
// Sent when the window has
// the focus and a key is 
// pressed/released.
//

void PKDialog::keyDown(unsigned int modifier,
					   PKKey        keyCode,
					   unsigned int virtualCode)
{
	// IMPLEMENT WHEN NEEDED
}

void PKDialog::keyUp(unsigned int modifier,
				     PKKey        keyCode,
				     unsigned int virtualCode)
{
	// IMPLEMENT WHEN NEEDED
}



// Override those in your class

void PKDialog::toolbarItemClicked(PKToolbar *toolbar,
								  PKToolbarItem *item,
								  std::string id)
{
}

void PKDialog::toolbarSegmentedItemSwitched(PKToolbar *toolbar,
								            PKToolbarSegmentedItem *item,
								            std::string id, 
                                            int32_t selection)
{
}

void PKDialog::buttonClicked(std::string id)
{
}

void PKDialog::checkBoxAction(std::string id, bool previousValue)
{
}

void PKDialog::comboBoxChanged(std::string id, int32_t previousValue)
{
}

void PKDialog::sliderChanged(std::string id, int32_t previousValue)
{
}

void PKDialog::linkClicked(std::string id)
{
}

void PKDialog::treeViewItemRightClicked(std::string id, 
										int32_t x, 
										int32_t y)
{
}

void PKDialog::treeViewItemSelected(std::string id)
{
}

void PKDialog::editTextChanged(std::string id, 
                               std::wstring text)
{

}

void PKDialog::editTextEditingStarted(std::string id)
{

}

void PKDialog::editTextEditingEnded(std::string id, 
								    std::wstring oldValue, 
								    std::wstring newValue)
{

}

void PKDialog::bitmapToggleButtonToggled(std::string id,
                                   PKBitmapToggleButton *button,
								   bool previousValue)
{

}

void PKDialog::stretchToggleButtonToggled(std::string id,
                                    PKStretchToggleButton *button,
									bool previousValue)
{

}

void PKDialog::listBoxSelectionChanged(std::string id, int32_t previousValue)
{

}

void PKDialog::editableVectorLabelTextChanged(std::string id)
{
	
}

void PKDialog::editableVectorLabelEditStarted(std::string id)
{

}

void PKDialog::editableVectorLabelEditEnded(std::string id, 
    									    std::wstring oldValue, 
	        							    std::wstring newValue)
{

}

void PKDialog::customControlClicked(PKButtonType button,
                                    std::string id,
                                    PKCustomControl *control,
                                    std::string partCode)
{
    
}

void PKDialog::customControlDoubleClicked(PKButtonType button,
										  std::string id,
										  PKCustomControl *control,
										  std::string partCode)
{
	
}

void PKDialog::vectorControlRootMouseMove(std::string id,
                                          int32_t x,
                                          int32_t y)
{
    
}

void PKDialog::vectorControlRootMouseLeave(std::string id)
{
    
}

void PKDialog::selectorControlChanged(std::string id, int32_t oldValue)
{

}

