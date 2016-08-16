//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKEditableVectorLabel.h"
#include "PKVectorEngine.h"
#include "PKVectorControlRoot.h"
#include "PKStr.h"
#include "PKClipboard.h"
#include "PKDialog.h"

enum {

    PK_EVL_MENU_ID_CUT  = 2345,
    PK_EVL_MENU_ID_COPY,
    PK_EVL_MENU_ID_PASTE,
    PK_EVL_MENU_ID_SELECT_ALL,
};

//
// PROPERTIES
//

std::string PKEditableVectorLabel::TEXT_WSTRING_PROPERTY                     = "text";
std::string PKEditableVectorLabel::BACKGROUND_COLOR_COLOR_PROPERTY           = "backgroundColor";
std::string PKEditableVectorLabel::BORDER_COLOR_COLOR_PROPERTY               = "borderColor";
std::string PKEditableVectorLabel::HOVER_BACKGROUND_COLOR_COLOR_PROPERTY     = "hoverBackgroundColor";
std::string PKEditableVectorLabel::HOVER_BORDER_COLOR_COLOR_PROPERTY         = "hoverBorderColor";
std::string PKEditableVectorLabel::TEXT_COLOR_COLOR_PROPERTY                 = "textColor";
std::string PKEditableVectorLabel::SELECTED_TEXT_COLOR_COLOR_PROPERTY        = "selectedTextColor";
std::string PKEditableVectorLabel::SELECTION_COLOR_COLOR_PROPERTY            = "selectionColor";
std::string PKEditableVectorLabel::BORDER_WIDTH_UINT32_PROPERTY              = "borderWidth";
std::string PKEditableVectorLabel::FONT_FAMILY_STRING_PROPERTY               = "fontFamily";
std::string PKEditableVectorLabel::BOLD_BOOL_PROPERTY                        = "bold";
std::string PKEditableVectorLabel::DEFAULT_TEXT_WSTRING_PROPERTY             = "defaultText";
std::string PKEditableVectorLabel::DEFAULT_TEXT_COLOR_COLOR_PROPERTY         = "defaultTextColor";
std::string PKEditableVectorLabel::OFFSET_UINT32_PROPERTY                    = "offset";
std::string PKEditableVectorLabel::EDITABLE_BOOL_PROPERTY                    = "editable";
std::string PKEditableVectorLabel::CENTER_BOOL_PROPERTY                      = "center";



//
// PKEditableVectorLabel
//

PKEditableVectorLabel::PKEditableVectorLabel()
{
	PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(defaultText, L"NOT_SET");
		
    PKColor bkg = PKColorCreate(0, 0, 0, 0);
    PKOBJECT_ADD_COLOR_PROPERTY(backgroundColor, bkg);

    PKColor brd = PKColorCreate(240, 240, 240, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(borderColor, brd);

    PKColor hbkg = PKColorCreate(250, 250, 250, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(hoverBackgroundColor, hbkg);

    PKColor hbrd = PKColorCreate(160, 170, 210, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(hoverBorderColor, hbrd);
    
    PKColor txt = PKColorCreate(0, 0, 0, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(textColor, txt);

    PKColor stxt = PKColorCreate(240, 240, 240, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(selectedTextColor, stxt);

    PKColor sbkg = PKColorCreate(180, 200, 240, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(selectionColor, sbkg);

    PKColor dft = PKColorCreate(200, 200, 200, 255);
    PKOBJECT_ADD_COLOR_PROPERTY(defaultTextColor, dft);

    PKOBJECT_ADD_UINT32_PROPERTY(borderWidth, 1);
    PKOBJECT_ADD_STRING_PROPERTY(fontFamily, "Arial");
    PKOBJECT_ADD_BOOL_PROPERTY(bold, false);
    PKOBJECT_ADD_UINT32_PROPERTY(offset, 5);
    PKOBJECT_ADD_BOOL_PROPERTY(editable, true);
    PKOBJECT_ADD_BOOL_PROPERTY(center, false);

    // Setup

    this->built = false;
    this->needsFontSizeUpdate = true;
    this->fontSize = 0;
	this->editing = false;
	this->hover = false;

    this->barShown = true;
    this->setTimerDelay(500);
    this->barPos = 0;

    this->selectionStart = -1;
    this->selectionSize  = 0;
    this->selectionStartedFrom = 0;
	
	this->needsOffsetUpdate = false;
	this->mouseIsDown = false;
    this->popupShowing = false;

    // Create popup menu

    this->menu = new PKPopupMenu();
    this->menu->setMenuCallback(this);

    this->menu->insertItem(PK_EVL_MENU_ID_CUT,
                           PK_i18n(L"Cut"));
    this->menu->insertItem(PK_EVL_MENU_ID_COPY,
                           PK_i18n(L"Copy"));
    this->menu->insertItem(PK_EVL_MENU_ID_PASTE,
                           PK_i18n(L"Paste"));
    this->menu->insertSeparator();
    this->menu->insertItem(PK_EVL_MENU_ID_SELECT_ALL,
                           PK_i18n(L"Select All"));
                           
#ifdef LINUX
    this->im_context = gtk_im_multicontext_new();

    this->signal_id_1 = g_signal_connect(G_OBJECT(this->im_context), "commit",
                                         G_CALLBACK (PKEditableVectorLabel::gtk_im_commit), this);    
             
    this->signal_id_2 = g_signal_connect(G_OBJECT(this->im_context), "retrieve-surrounding",
                                         G_CALLBACK (PKEditableVectorLabel::gtk_im_retrieve_surroundings), this);                                       
#endif
}

PKEditableVectorLabel::~PKEditableVectorLabel()
{
    this->stopTimer();

#ifdef LINUX
    g_object_unref(this->im_context);
#endif

    this->menu->release();
}

void PKEditableVectorLabel::build()
{
	PKVectorControl::build();
	this->built = true;

	PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);

    this->_startText = text;

#ifdef LINUX

    if(this->controlRoot != NULL)
    {
        GtkWidget *root = this->controlRoot->getWindowHandle();
        
        if(root != NULL)
        {    
            gtk_im_context_set_client_window(this->im_context, root->window);
    
            g_signal_connect(G_OBJECT(root), "key-press-event",
                             G_CALLBACK(PKEditableVectorLabel::gtk_vector_key_press_event_callback), this);  

            g_signal_connect(G_OBJECT(root), "key-release-event",
                             G_CALLBACK(PKEditableVectorLabel::gtk_vector_key_release_event_callback), this);      
        }
    }

#endif
}

void PKEditableVectorLabel::updateUIStrings()
{
    this->menu->updateItem(PK_EVL_MENU_ID_CUT,
                           PK_i18n(L"Cut"));
    this->menu->updateItem(PK_EVL_MENU_ID_COPY,
                           PK_i18n(L"Copy"));
    this->menu->updateItem(PK_EVL_MENU_ID_PASTE,
                           PK_i18n(L"Paste"));
    this->menu->updateItem(PK_EVL_MENU_ID_SELECT_ALL,
                           PK_i18n(L"Select All"));
}

void PKEditableVectorLabel::doCut()
{
    if(this->selectionSize == 0)
        return;

    if(this->selectionStart == -1)
        return;

    PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

    std::wstring substring = text.substr(this->selectionStart, this->selectionSize);
    text.erase(this->selectionStart, this->selectionSize);

    PKClipboard clip;
    clip.addText(substring);

	this->selectionSize  = 0;
	this->selectionStart = -1;

    if(text.size() == 0)
    {
        this->barPos = 0;
    }
	
    PKVariantWString newText(text);
    this->set(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY, &newText);

	if(this->dialog != NULL)
	{
		PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
		std::string sid = PKVALUE_STRING(id);
		
		this->dialog->editableVectorLabelTextChanged(sid);
	}
}

void PKEditableVectorLabel::doCopy()
{
    if(this->selectionSize == 0)
        return;

    if(this->selectionStart == -1)
        return;

    PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);

    std::wstring substring = text.substr(this->selectionStart, this->selectionSize);

    PKClipboard clip;
    clip.addText(substring);
}

void PKEditableVectorLabel::doPaste()
{
    PKClipboard clip;
    std::wstring text = clip.getText();
    this->characterInput(text);
}

void PKEditableVectorLabel::doSelectAll()
{
    this->selectAll();
}

bool PKEditableVectorLabel::hasSelection()
{
    return (this->selectionSize > 0);
}

void PKEditableVectorLabel::commitChanges()
{
	if(this->getDialog())
	{
        std::wstring newText = PKVALUE_WSTRING(this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY));

        if(this->_startText != newText)
        {
            std::wstring oldText = this->_startText;
            this->_startText = newText;

		    this->getDialog()->editableVectorLabelEditEnded(this->getControlId(),
												            oldText,
												            newText);
        }
	}
}

void PKEditableVectorLabel::undoManagerCommitCallback()
{
	if(this->getDialog())
	{
		if(this->getDialog()->getFocusedControl() == this)
		{
		    this->commitChanges();
		}
	}
}

void PKEditableVectorLabel::resetChanges()
{
    this->_startText = PKVALUE_WSTRING(this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY));
}

void PKEditableVectorLabel::menuCancelled()
{
    this->popupShowing = false;
}

void PKEditableVectorLabel::menuCallback(unsigned int id)
{
    this->popupShowing = false;

    // Handle popup menu

    if(id == PK_EVL_MENU_ID_SELECT_ALL)
    {
		this->selectAll();
    }

    if(id == PK_EVL_MENU_ID_CUT)
    {
        this->doCut();
    }

    if(id == PK_EVL_MENU_ID_COPY)
    {
        this->doCopy();
    }

    if(id == PK_EVL_MENU_ID_PASTE)
    {
        this->doPaste();
    }
}

void PKEditableVectorLabel::selectAll()
{
	PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);
	uint32_t size = text.size();
	
	if(size > 0)
	{
		this->selectionStartedFrom = 0;
		this->selectionStart = 0;
		this->selectionSize = size;
		this->barPos = size;
		this->invalidate();
	}
}


void PKEditableVectorLabel::timerCallbackFunction()
{
    // Cursor animations
    this->barShown = !this->barShown;
    this->invalidate();
}

void PKEditableVectorLabel::drawVector(PKVectorEngine *engine,
		                               int32_t x, int32_t y)
{
	if(!this->built)
	{
		return;
	}
	
	if(engine == NULL)
	{
		return;
	}
	
	int w = this->getW();
	int h = this->getH();

    int margin = h/8;

	PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);
	uint32_t tsize = text.size();

	PKVariant *dt = this->get(PKEditableVectorLabel::DEFAULT_TEXT_WSTRING_PROPERTY);
	std::wstring dtext = PKVALUE_WSTRING(dt);
    uint32_t dtsize = dtext.size();

    PKVariant *f = this->get(PKEditableVectorLabel::FONT_FAMILY_STRING_PROPERTY);
	std::wstring wfamily = PKStr::stringToWString(PKVALUE_STRING(f));
	
	PKVariant *bp = this->get(PKEditableVectorLabel::BOLD_BOOL_PROPERTY);
	bool bold = PKVALUE_BOOL(bp);
	
	PKVariant *cp = this->get(PKEditableVectorLabel::CENTER_BOOL_PROPERTY);
	bool center = PKVALUE_BOOL(cp);

    PKVFontStyle style = (bold ? PKV_FONT_STYLE_BOLD : PKV_FONT_STYLE_NORMAL);
	
	PKVariant *off  = this->get(PKEditableVectorLabel::OFFSET_UINT32_PROPERTY);
	uint32_t offset = PKVALUE_UINT32(off);	

    uint32_t normalFontSize = h - 2*margin;

    if(this->needsFontSizeUpdate)
    {
        this->fontSize = normalFontSize;

        // Check if text fits and
        // otherwise decrease font size

        if(text != L"")
        {
            uint32_t textSize = engine->getStringWidth(wfamily, this->fontSize, style, text);

            if(textSize > (w - 2*offset))
            {
                // Need to adjust
                do
                {
                    this->fontSize--;
                    textSize = engine->getStringWidth(wfamily, this->fontSize, style, text);
                }
                while(this->fontSize > 0 && (textSize > (w - 2*offset)));

                if(fontSize == 0)
                {
                    return;
                }
            }
        }
        else
        {
            if(dtext != L"")
            {
                uint32_t textSize = engine->getStringWidth(wfamily, this->fontSize, style, dtext);

                if(textSize > (w - 2*offset))
                {
                    // Need to adjust
                    do
                    {
                        this->fontSize--;
                        textSize = engine->getStringWidth(wfamily, this->fontSize, style, dtext);
                    }
                    while(this->fontSize > 0 && (textSize > (w - 2*offset)));

                    if(fontSize == 0)
                    {
                        return;
                    }
                }
            }
        }

        this->needsFontSizeUpdate = false;
    }
	
	double offs  = 0;
	double offx  = 0;
	double offx2 = 0;
	double offx3 = 0;
#ifdef LINUX
	offs = (double)margin*3/4;
#endif
#ifdef MACOSX
	offs = (double)margin*2/3;
#endif
#ifdef WIN32
    offx  = (double)this->fontSize / 6.0;
    offx2 = 0;
    offx3 = (double)this->fontSize / 6.0;
#endif
#ifdef MACOSX
    offx  = -(double)this->fontSize / 8;
    offx2 = -(double)this->fontSize / 8;
    offx3 = 0;
#endif
#ifdef LINUX
    offx  = -(double)this->fontSize / 8;
    offx2 = -(double)this->fontSize / 8;
    offx3 = 0;
#endif

	if(this->needsOffsetUpdate)
	{
		this->letter_offsets.clear();

		if(tsize > 0)
		{
			// Move the selection marker to the right place
			
			for(uint32_t i=1; i<=tsize; i++)
			{
				std::wstring substring = text.substr(0, i);
				
				this->letter_offsets.push_back((int)(engine->getStringWidth(wfamily, 
																	  this->fontSize,
																	  style,
																	  substring)+offset-offx));
			}
		}
	}
	
	// Draw bkg and border
	
	if(this->hover || this->editing)
	{
        if(!this->editing)
        {
    		PKVariant *b = this->get(PKEditableVectorLabel::HOVER_BACKGROUND_COLOR_COLOR_PROPERTY);
    		PKColor bkg = PKVALUE_COLOR(b);
		
	    	if(bkg.a != 0)
	    	{
	    		engine->fillRect(x, y, w, h, bkg);
	    	}
        }

		PKVariant *brd = this->get(PKEditableVectorLabel::HOVER_BORDER_COLOR_COLOR_PROPERTY);
		PKColor border = PKVALUE_COLOR(brd);
		
		PKVariant *bwp = this->get(PKEditableVectorLabel::BORDER_WIDTH_UINT32_PROPERTY);
		uint32_t bw = PKVALUE_UINT32(bwp);
		
		if(border.a != 0)
		{
			engine->drawRect(x, y, w, h, border, bw);
		}
	}
	else
	{	
		PKVariant *b = this->get(PKEditableVectorLabel::BACKGROUND_COLOR_COLOR_PROPERTY);
		PKColor bkg = PKVALUE_COLOR(b);

		if(bkg.a != 0)
		{
			engine->fillRect(x, y, w, h, bkg);
		}

		PKVariant *brd = this->get(PKEditableVectorLabel::BORDER_COLOR_COLOR_PROPERTY);
		PKColor border = PKVALUE_COLOR(brd);

		PKVariant *bwp = this->get(PKEditableVectorLabel::BORDER_WIDTH_UINT32_PROPERTY);
		uint32_t bw = PKVALUE_UINT32(bwp);

		if(border.a != 0)
		{
			engine->drawRect(x, y, w, h, border, bw);
		}
	}
		
    // Draw selection

    if(this->selectionSize > 0)
    {
        std::wstring substring_start = text.substr(0, this->selectionStart);
        std::wstring substring_end   = text.substr(0, this->selectionStart + this->selectionSize);

        PKVariant *selc = this->get(PKEditableVectorLabel::SELECTION_COLOR_COLOR_PROPERTY);
		PKColor selectionColor = PKVALUE_COLOR(selc);

        uint32_t woffs = engine->getStringWidth(wfamily, this->fontSize, style, substring_start);
        uint32_t woffe = engine->getStringWidth(wfamily, this->fontSize, style, substring_end);

        engine->fillRect(x+offset-offx2+(woffs == 0 ? 0 : woffs - 2*offx3), y+margin,
                        (woffs == 0 ? woffe-woffs-offx2-2*offx3 : woffe-woffs-offx2), 
                         h-2*margin, selectionColor);
    }

    // Draw text & cursor

	if(text == L"" && !this->editing)
	{
		PKVariant *txtc = this->get(PKEditableVectorLabel::DEFAULT_TEXT_COLOR_COLOR_PROPERTY);
		PKColor textColor = PKVALUE_COLOR(txtc);
		
        int x_offset = x+offset-offx;

        if(center)
        {
            uint32_t size = engine->getStringWidth(wfamily, this->fontSize, style, dtext);
            x_offset = x + (w - size)/2;
        }

        engine->drawString(wfamily, this->fontSize, 
					       style, 
					       textColor, 
					       x_offset, 
                           y+margin-offs+(normalFontSize - this->fontSize)/2, 
                           dtext);
	}
	else
	{	
		PKVariant *txtc = this->get(PKEditableVectorLabel::TEXT_COLOR_COLOR_PROPERTY);
		PKColor textColor = PKVALUE_COLOR(txtc);
		
        int x_offset = x+offset-offx;

        if(center)
        {
            uint32_t size = engine->getStringWidth(wfamily, this->fontSize, style, text);
            x_offset = x + (w - size)/2;
        }

        engine->drawString(wfamily, this->fontSize, 
						   style, 
						   textColor, 
						   x_offset, 
                           y+margin-offs+(normalFontSize - this->fontSize)/2, 
                           text);
	
        if(this->editing && this->barShown)
        {
            if(this->barPos == 0)
            {
                engine->drawLine((int)(x+offset-offx2), 
                                 y+3*margin/2, 
                                 (int)(x+offset-offx2), 
                                 y+h-3*margin/2, 
                                 textColor, 1);
            }
            else
            {
                std::wstring substr = text.substr(0, this->barPos);
                int woffset = engine->getStringWidth(wfamily, this->fontSize, style, substr);

                engine->drawLine((int)(x+offset-offx2-offx-offx3+woffset), 
                                 y+3*margin/2, 
                                 (int)(x+offset-offx2-offx-offx3+woffset), 
                                 y+h-3*margin/2, 
                                 textColor, 1);
            }
        }
    }
}

// From PKVectorControl

void PKEditableVectorLabel::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKEditableVectorLabel::TEXT_WSTRING_PROPERTY)
	{
		this->needsOffsetUpdate   = true;
        this->needsFontSizeUpdate = true;
		
    	this->invalidate();
    }
	
	if(prop->getName() == PKControl::FRAME_RECT_PROPERTY)
	{
		this->needsOffsetUpdate   = true;
        this->needsFontSizeUpdate = true;
	}
	
	PKVectorControl::selfPropertyChanged(prop, oldValue);
}

// Misc

uint32_t PKEditableVectorLabel::getBarPosForMouseX(int x)
{
	uint32_t bsize = this->letter_offsets.size();
	
	if(bsize == 0)
	{
		return 0;
	}
	
	int32_t bpos = 0;
	
	for(bpos = 0; bpos<(int)bsize; bpos++)
	{
		int x1 = this->letter_offsets[bpos];
		
		if(x < x1)
		{
			if(bpos-1 >0)
			{
				int x2 = this->letter_offsets[bpos-1];
				
				if(x <= (x2 + (x1-x2)/2))
				{
					return bpos;
				}
				else
				{
					return bpos+1;
				}
			}
			else
			{
				return bpos;
			}
		}
	}
	
	return bpos;
}

// From PKCustomControl

void PKEditableVectorLabel::mouseDown(PKButtonType button,
					                  int x,
					                  int y,
					                  unsigned int mod)
{
	PKVariant *e = this->get(PKEditableVectorLabel::EDITABLE_BOOL_PROPERTY);
	
	if(!PKVALUE_BOOL(e))
	{
		return;
	}
    
	if(button == PK_BUTTON_RIGHT)
    {
        // Show popup menu

        PKWindowHandle window = NULL;
        if(this->controlRoot)
        {
            window = controlRoot->getWindowHandle();
        }

        // See which items we need to enable

        PKClipboard clipboard;
        this->menu->enableItem(PK_EVL_MENU_ID_PASTE, clipboard.containsText());
        this->menu->enableItem(PK_EVL_MENU_ID_CUT, (this->selectionSize > 0));
        this->menu->enableItem(PK_EVL_MENU_ID_COPY, (this->selectionSize > 0));

        // Popup

        this->popupShowing = true;
        this->menu->popup(window);
    }
	else
	{
		// Clear selection unless shift is held down
		
		if(mod & PK_MOD_SHIFT)
		{
			if(this->selectionSize > 0)
			{
				// There is already a selection, so let's
				// extend it in the required direction
				
				uint32_t size = this->selectionSize;
				uint32_t bpos = this->getBarPosForMouseX(x);

				if(bpos > (this->selectionStart + this->selectionSize))
				{
					// Extend to the right
					this->selectionSize = (bpos - this->selectionStart);
				}
				else if((int) bpos < this->selectionStart)
				{
					// Extend to the left
					this->selectionSize  += (this->selectionStart - bpos);					
					this->selectionStart  = bpos;
				}
				
				if(size != this->selectionSize)
				{
					this->invalidate();
				}
			}
			else
			{
				// There is no selection so we just start a new one

				uint32_t bpos = this->getBarPosForMouseX(x);
				this->barPos  = bpos;
				
				this->selectionStartedFrom = bpos;
				
				this->mouseIsDown = true;
				this->invalidate();
			}
		}
		else
		{
			if(this->selectionSize > 0)
			{
				this->selectionStartedFrom = 0;
				this->selectionSize  = 0;
				this->selectionStart = - 1;
			}

			uint32_t bpos = this->getBarPosForMouseX(x);
			this->barPos  = bpos;
	
			this->selectionStartedFrom = bpos;
		
			this->mouseIsDown = true;
			this->invalidate();
		}
	}
}

void PKEditableVectorLabel::mouseDblClick(PKButtonType button,
										  int x,
										  int y,
										  unsigned int mod)
{
	PKVariant *e = this->get(PKEditableVectorLabel::EDITABLE_BOOL_PROPERTY);
	
	if(!PKVALUE_BOOL(e))
	{
		return;
	}

	if(button == PK_BUTTON_LEFT)
	{
		this->selectAll();
	}
}

void PKEditableVectorLabel::mouseUp(PKButtonType button,
					                  int x,
					                  int y,
					                  unsigned int mod)
{
	this->mouseIsDown = false;
}

void PKEditableVectorLabel::mouseMove(int x,
									  int y,
									  unsigned int mod)
{
	if(this->hover == false)
	{
		this->hover = true;
		this->invalidate();
	}
	
	if(this->mouseIsDown)
	{
		// Selecting

		uint32_t old_size = this->selectionSize;
		uint32_t new_bpos = this->getBarPosForMouseX(x);

		if(new_bpos < this->selectionStartedFrom)
		{
			this->selectionStart = new_bpos;
			this->selectionSize  = this->selectionStartedFrom - new_bpos;
		}
		else if(new_bpos > this->selectionStartedFrom)
		{
			this->selectionStart = this->selectionStartedFrom;
			this->selectionSize  = new_bpos - this->selectionStartedFrom;
		}
		else
		{
			this->selectionSize = 0;
			this->selectionStart = -1;
		}		
		
		if(this->selectionSize != old_size)
		{
			this->invalidate();
		}
	}
}

void PKEditableVectorLabel::mouseLeave()
{
	this->hover = false;
	this->invalidate();
}

void PKEditableVectorLabel::focusAcquired(PKDirection dir)
{
    PKUndoManager::getInstance()->addCommitCallback(this);	
	
    PKVectorControl::focusAcquired(dir);

    this->_startText = PKVALUE_WSTRING(this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY));

    if(this->dialog != NULL)
	{
		PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
		std::string sid = PKVALUE_STRING(id);
		
		this->dialog->editableVectorLabelEditStarted(sid);
	}
	
    this->editing = true;
    this->barShown = true;
    this->invalidate();

    this->startTimer();

    if(this->hasFocus() || dir == PK_DIR_NONE)
    {    
        return;    
    }

	this->selectAll();

#ifdef LINUX
    gtk_im_context_reset(this->im_context);
    gtk_im_context_focus_in(this->im_context);
#endif
}

void PKEditableVectorLabel::focusLost()
{
    PKUndoManager::getInstance()->removeCommitCallback(this);

    PKVectorControl::focusLost();

    if(this->popupShowing)
    {
        return;
    }    

	if(this->getDialog())
	{
        std::wstring newText = PKVALUE_WSTRING(this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY));

        if(this->_startText != newText)
        {
            std::wstring oldText = this->_startText;
            this->_startText = newText;

            this->getDialog()->editableVectorLabelEditEnded(this->getControlId(),
												            oldText,
												            newText);
        }
	}

    this->selectionSize = 0;
    this->selectionStart = -1;
    this->selectionStartedFrom = 0;

    this->editing = false;
    this->stopTimer();
    this->barShown = false;

#ifdef LINUX
    gtk_im_context_reset(this->im_context);
    gtk_im_context_focus_out(this->im_context);
#endif

    this->invalidate();
}

bool PKEditableVectorLabel::keyDown(unsigned int mod,
			                        PKKey        keyCode,
			                        unsigned int virtualCode)
{
    if(!this->editing)
    {
        return false;
    }

	if(mod & PK_MOD_COMMAND)
	{
		return false;
	}
	
    // DEL

    if(keyCode == PK_KEY_DELETE)
    {
        PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
        std::wstring text = PKVALUE_WSTRING(t);
        uint32_t size = text.size();

        if(this->selectionSize > 0)
        {
            // Delete selection
            text.erase(this->selectionStart, this->selectionSize);
            this->barPos = this->selectionStart;
            this->selectionSize = 0;
            this->selectionStart = -1;
            this->selectionStartedFrom = 0;

            PKVariantWString newText(text);
            this->set(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY, &newText);

			if(this->dialog != NULL)
			{
				PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
				std::string sid = PKVALUE_STRING(id);
				
				this->dialog->editableVectorLabelTextChanged(sid);
			}
		}
        else
        {
            // Delete at current position

            if(this->barPos != size)
            {
                text.erase(this->barPos, 1);
                PKVariantWString newText(text);
                this->set(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY, &newText);

				if(this->dialog != NULL)
				{
					PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
					std::string sid = PKVALUE_STRING(id);
					
					this->dialog->editableVectorLabelTextChanged(sid);
				}
			}
        }

        return true;
    }

    // BACKSPACE

    if(keyCode == PK_KEY_BACKSPACE)
    {
        PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
        std::wstring text = PKVALUE_WSTRING(t);

        if(this->selectionSize > 0)
        {
            // Delete selection
            text.erase(this->selectionStart, this->selectionSize);
            this->barPos = this->selectionStart;
            this->selectionSize = 0;
            this->selectionStart = -1;
            this->selectionStartedFrom = 0;

            PKVariantWString newText(text);
            this->set(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY, &newText);

			if(this->dialog != NULL)
			{
				PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
				std::string sid = PKVALUE_STRING(id);
				
				this->dialog->editableVectorLabelTextChanged(sid);
			}
		}
        else
        {
            // Delete before current position

            if(this->barPos > 0)
            {
                text.erase(this->barPos-1, 1);
                this->barPos--;
                PKVariantWString newText(text);
                this->set(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY, &newText);

				if(this->dialog != NULL)
				{
					PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
					std::string sid = PKVALUE_STRING(id);
					
					this->dialog->editableVectorLabelTextChanged(sid);
				}
			}
        }

        return true;
    }

    // Home, Up arrow

    if(keyCode == PK_KEY_HOME ||
       keyCode == PK_KEY_UP)
    {
        if(mod & PK_MOD_SHIFT)
        {
            // Selection
            
            if(this->selectionStart == -1)
            {
                this->selectionStartedFrom = this->barPos;
                this->selectionStart = 0;
                this->selectionSize  = this->barPos;
                this->barPos = 0;

                if(this->selectionSize == 0)
                {
                    this->selectionStart = -1;
                    this->selectionStartedFrom = 0;
                }

                this->invalidate();
            }
            else
            {
                this->selectionStart = 0;
                this->selectionSize  = this->selectionStartedFrom;
                this->barPos = 0;

                if(this->selectionSize == 0)
                {
                    this->selectionStart = -1;
                    this->selectionStartedFrom = 0;
                }

                this->invalidate();
            }
        }
        else
        {
            // Move 

            if(this->selectionSize > 0)
            {
                this->selectionStartedFrom = 0;
                this->selectionSize  = 0;
                this->selectionStart = - 1;
            }

            this->barPos = 0;
            this->barShown = true;
            this->invalidate();
        }

        return true;
    }

    // End, Down arrow

    if(keyCode == PK_KEY_END ||
       keyCode == PK_KEY_DOWN)
    {
        PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
        std::wstring text = PKVALUE_WSTRING(t);
        uint32_t size = text.size();

        if(mod & PK_MOD_SHIFT)
        {
            // Selection
            
            if(this->selectionStart == -1)
            {
                this->selectionStartedFrom = this->barPos;
                this->selectionStart = this->barPos;
                this->selectionSize  = size-this->barPos;
                this->barPos = size;
                this->barShown = true;

                if(this->selectionSize == 0)
                {
                    this->selectionStart = -1;
                    this->selectionStartedFrom = 0;
                }

                this->invalidate();
            }
            else
            {
                this->selectionStart = this->selectionStartedFrom;
                this->selectionSize  = size - this->selectionStartedFrom;
                this->barPos = size;
                this->barShown = true;

                if(this->selectionSize == 0)
                {
                    this->selectionStart = -1;
                    this->selectionStartedFrom = 0;
                }

                this->invalidate();
            }
        }
        else
        {
            // Move 
            if(this->selectionSize > 0)
            {
                this->selectionStartedFrom = 0;
                this->selectionSize  = 0;
                this->selectionStart = - 1;
            }

            this->barPos = size;
            this->barShown = true;
            this->invalidate();
        }

        return true;
    }

    // Left arrow

    if(keyCode == PK_KEY_LEFT)
    {
        PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
        std::wstring text = PKVALUE_WSTRING(t);
        uint32_t size = text.size();

        if(mod & PK_MOD_SHIFT)
        {
            if(this->barPos <= 0)
            {
                return true;
            }

            // Selection handling

            if(mod & PK_MOD_CONTROL)
            {
                // Whole words
            
                // Calculate where we land

                std::wstring substr = text.substr(0, this->barPos);
                uint32_t ssize = substr.size();
                uint32_t index = ssize-1;

                // skip spaces first

                bool stopped = false;
                while(index > 0)
                {
                    if(substr[index] == L' ')
                    {
                        index--;
                    }
                    else
                    {
                        stopped = true;
                        break;
                    }
                }

                if(this->selectionStart == -1)
                {
                    this->selectionStartedFrom = this->barPos;
                    this->selectionStart = this->barPos;
                    this->selectionSize  = 0;
                }

                if(!stopped)
                {
                    // We reached the start

                    this->selectionStart = 0;
                    this->selectionSize  = this->selectionStartedFrom;
                    this->barPos = 0;
                    this->barShown = true;

                    if(this->selectionSize == 0)
                    {
                        this->selectionStart = -1;
                        this->selectionStartedFrom = 0;
                    }

                    this->invalidate();
                    return true;
                }
                else
                {
                    // Find the next space
                    std::wstring subsubstr = substr.substr(0, index);
                    int pos = subsubstr.find_last_of(L' ');

                    if(pos == std::wstring::npos)
                    {
                        // We reached the start also

                        this->selectionStart = 0;
                        this->selectionSize  = this->selectionStartedFrom;
                        this->barPos = 0;
                        this->barShown = true;

                        if(this->selectionSize == 0)
                        {
                            this->selectionStart = -1;
                            this->selectionStartedFrom = 0;
                        }

                        this->invalidate();
                        return true;
                    }
                    else
                    {
                        // Let's move to the new pos
                        int newBarPos = pos+1;

                        if(this->barPos < this->selectionStartedFrom)
                        {
                            this->selectionStart -= (this->barPos - newBarPos);
                            this->selectionSize  += (this->barPos - newBarPos);
                        }
                        else
                        {
                            if(newBarPos < (int) this->selectionStartedFrom)
                            {
                                this->selectionStart = newBarPos;
                                this->selectionSize  = (this->selectionStartedFrom - newBarPos);
                            }
                            else
                            {
                                this->selectionSize -= (this->barPos - newBarPos);
                            }
                        }

                        if(this->selectionSize == 0)
                        {
                            this->selectionStart = -1;
                            this->selectionStartedFrom = 0;
                        }

                        this->barPos   = newBarPos;
                        this->barShown = true;
                        this->invalidate();
                    }
                }
            }
            else
            {
                // One char at a time

                if(this->selectionStart == -1)
                {
                    this->selectionStartedFrom = this->barPos;
                    this->selectionStart = this->barPos-1;
                    this->selectionSize  = 1;
                    this->barPos--;
                    this->invalidate();
                }
                else
                {
                    if(this->barPos > this->selectionStartedFrom)
                    {
                        this->selectionSize--;
                        this->barPos--;

                        if(this->selectionSize == 0)
                        {
                            this->selectionStart = -1;
                            this->selectionStartedFrom = 0;
                        }

                        this->invalidate();
                    }
                    else
                    {
                        this->selectionStart--;
                        this->selectionSize++;
                        this->barPos--;
                        this->invalidate();
                    }
                }
            }
        }
        else
        {
            // Clear selection

            if(this->selectionSize > 0)
            {
                this->selectionStartedFrom = 0;
                this->selectionSize  = 0;
                this->selectionStart = - 1;
                this->invalidate();
            }

            if(this->barPos <= 0)
            {
                return true;
            }

            // Cursor movement handling

            if(mod & PK_MOD_CONTROL)
            {
                // Whole words

                std::wstring substr = text.substr(0, this->barPos);
                uint32_t ssize = substr.size();
                uint32_t index = ssize-1;

                // skip spaces first

                bool stopped = false;
                while(index > 0)
                {
                    if(substr[index] == L' ')
                    {
                        index--;
                    }
                    else
                    {
                        stopped = true;
                        break;
                    }
                }

                if(!stopped)
                {
                    // We reached the start
                    this->barPos = 0;
                    this->barShown = true;
                    this->invalidate();
                    return true;
                }
                else
                {
                    // Find the next space
                    std::wstring subsubstr = substr.substr(0, index);
                    int pos = subsubstr.find_last_of(L' ');

                    if(pos == std::wstring::npos)
                    {
                        // We reached the start
                        this->barPos = 0;
                        this->barShown = true;
                        this->invalidate();
                        return true;
                    }
                    else
                    {
                        // Let's move to the new pos
                        this->barPos = pos+1;
                        this->barShown = true;
                        this->invalidate();
                    }
                }
            }
            else
            {
                // One char at a time
                if(this->barPos > 0)
                {
                    this->barPos--;
                    this->barShown = true;
                    this->invalidate();
                }
            }
        }

        return true;
    }

    // Right arrow

    if(keyCode == PK_KEY_RIGHT)
    {
        PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
        std::wstring text = PKVALUE_WSTRING(t);
        uint32_t size = text.size();

        if(mod & PK_MOD_SHIFT)
        {
            if(this->barPos >= size)
            {
                return true;
            }

            // Selection handling

            if(mod & PK_MOD_CONTROL)
            {
                // Whole words
            
                // Calculate where we land

                std::wstring substr = text.substr(this->barPos);
                uint32_t ssize = substr.size();
                uint32_t index = 0;

                // skip spaces first

                bool stopped = false;
                while(index < ssize)
                {
                    if(substr[index] == L' ')
                    {
                        index++;
                    }
                    else
                    {
                        stopped = true;
                        break;
                    }
                }

                if(this->selectionStart == -1)
                {
                    this->selectionStartedFrom = this->barPos;
                    this->selectionStart = this->barPos;
                    this->selectionSize  = 0;
                }

                if(!stopped)
                {
                    // We reached the end

                    this->selectionStart = this->selectionStartedFrom;
                    this->selectionSize  = size - this->selectionStart;
                    this->barPos = size;
                    this->barShown = true;

                    if(this->selectionSize == 0)
                    {
                        this->selectionStart = -1;
                        this->selectionStartedFrom = 0;
                    }

                    this->invalidate();
                    return true;
                }
                else
                {
                    // Find the next space
                    std::wstring subsubstr = substr.substr(index);
                    int pos = subsubstr.find_first_of(L' ');

                    if(pos == std::wstring::npos)
                    {
                        // We reached the end
                        this->selectionStart = this->selectionStartedFrom;
                        this->selectionSize  = size - this->selectionStart;
                        this->barPos = size;
                        this->barShown = true;

                        if(this->selectionSize == 0)
                        {
                            this->selectionStart = -1;
                            this->selectionStartedFrom = 0;
                        }

                        this->invalidate();
                        return true;
                    }
                    else
                    {
                        // Let's move to the new pos
                        int newBarPos = this->barPos + (index + pos + 1);

                        if(this->barPos < this->selectionStartedFrom)
                        {
                            if(newBarPos < (int) this->selectionStartedFrom)
                            {
                                this->selectionStart += (newBarPos - this->barPos);
                                this->selectionSize  -= (newBarPos - this->barPos);
                            }
                            else
                            {
                                this->selectionStart = this->selectionStartedFrom;
                                this->selectionSize  = (newBarPos - this->selectionStartedFrom);
                            }
                        }
                        else
                        {
                            this->selectionSize += (newBarPos - this->barPos);
                        }

                        if(this->selectionSize == 0)
                        {
                            this->selectionStart = -1;
                            this->selectionStartedFrom = 0;
                        }

                        this->barPos   = newBarPos;
                        this->barShown = true;
                        this->invalidate();
                    }
                }
            }
            else
            {
                // One char at a time
            
                if(this->selectionStart == -1)
                {
                    this->selectionStartedFrom = this->barPos;
                    this->selectionStart = this->barPos;
                    this->selectionSize  = 1;
                    this->barPos++;
                    this->invalidate();
                }
                else
                {
                    if(this->barPos < this->selectionStartedFrom)
                    {
                        this->selectionStart++;
                        this->selectionSize--;

                        if(this->selectionSize == 0)
                        {
                            this->selectionStart = -1;
                            this->selectionStartedFrom = 0;
                        }

                        this->barPos++;
                        this->invalidate();
                    }
                    else
                    {
                        this->selectionSize++;
                        this->barPos++;
                        this->invalidate();
                    }
                }
            }
        }
        else
        {
            // Clear selection

            if(this->selectionSize > 0)
            {
                this->selectionStartedFrom = 0;
                this->selectionSize  = 0;
                this->selectionStart = - 1;
                this->invalidate();
            }

            if(this->barPos >= size)
            {
                return true;
            }

            // Cursor movement handling

            if(mod & PK_MOD_CONTROL)
            {
                // Whole words

                std::wstring substr = text.substr(this->barPos);
                uint32_t ssize = substr.size();
                uint32_t index = 0;

                // skip spaces first

                bool stopped = false;
                while(index < ssize)
                {
                    if(substr[index] == L' ')
                    {
                        index++;
                    }
                    else
                    {
                        stopped = true;
                        break;
                    }
                }

                if(!stopped)
                {
                    // We reached the end
                    this->barPos = size;
                    this->barShown = true;
                    this->invalidate();
                    return true;
                }
                else
                {
                    // Find the next space
                    std::wstring subsubstr = substr.substr(index);
                    int pos = subsubstr.find_first_of(L' ');

                    if(pos == std::wstring::npos)
                    {
                        // We reached the end
                        this->barPos = size;
                        this->barShown = true;
                        this->invalidate();
                        return true;
                    }
                    else
                    {
                        // Let's move to the new pos
                        this->barPos += (index + pos + 1);
                        this->barShown = true;
                        this->invalidate();
                    }
                }
            }
            else
            {
                // One char at a time
                if(this->barPos < size)
                {
                    this->barPos++;
                    this->barShown = true;
                    this->invalidate();
                }
            }
        }

        return true;
    }

    return false;
}                 

void PKEditableVectorLabel::characterInput(std::wstring wstr)
{
    if(wstr.size() == 1)
    {
        if(wstr[0] < 32) // Backspace, Tab, Enter, etc
        {
            return;
        }

        if(wstr[0] == 127) // Delete
        {
            return;
        }
    }

    PKVariant *t = this->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
    std::wstring text = PKVALUE_WSTRING(t);
    
    // Check if we have a selection first
    // as we'll need to clear it now

    if(this->selectionSize > 0)
    {
        // Delete selection
        text.erase(this->selectionStart, this->selectionSize);
        this->barPos = this->selectionStart;
        this->selectionSize = 0;
        this->selectionStart = -1;
        this->selectionStartedFrom = 0;
    }

    // Replace line breaks & tabs in wstr

    wstr = PKStr::replaceString(L"\n", L"", wstr);
    wstr = PKStr::replaceString(L"\r", L"", wstr);
    wstr = PKStr::replaceString(L"\t", L"", wstr);

    // Insert now

    uint32_t size = text.size();

	if(this->barPos > size)
	{
		this->barPos = size;
	}
	
    if(this->barPos == 0)
    {
        text = wstr + text;
    }
    else if(this->barPos == size)
    {
        text = text + wstr;
	}
    else
    {
        std::wstring start = text.substr(0, this->barPos);
        std::wstring  end  = text.substr(this->barPos);

        text = start + wstr + end;
    }

    this->barPos += wstr.size();
    PKVariantWString nt(text);
    this->set(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY, &nt);

	if(this->dialog != NULL)
	{
		PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
		std::string sid = PKVALUE_STRING(id);
		
		this->dialog->editableVectorLabelTextChanged(sid);
	}
}

bool PKEditableVectorLabel::wantsKeyboardFocus()
{
	// Yeah we do, if we're editable
	
	PKVariant *e = this->get(PKEditableVectorLabel::EDITABLE_BOOL_PROPERTY);
	return PKVALUE_BOOL(e);
}

#ifdef LINUX

gboolean PKEditableVectorLabel::gtk_vector_key_press_event_callback(GtkWidget   *widget,
                                                                    GdkEventKey *event,
                                                                    gpointer     user_data)
{
    PKEditableVectorLabel *ctrl = (PKEditableVectorLabel *) user_data;

    if(ctrl == NULL)
        return FALSE;

    if(ctrl->hasFocus())
    {      
        gtk_im_context_filter_keypress(ctrl->im_context, event);
    }                                                 

    return FALSE; // Let the others receive it
}                                                 

gboolean PKEditableVectorLabel::gtk_vector_key_release_event_callback(GtkWidget   *widget,
                                                                      GdkEventKey *event,
                                                                      gpointer     user_data)
{
    PKEditableVectorLabel *ctrl = (PKEditableVectorLabel *) user_data;

    if(ctrl == NULL)
        return FALSE;

    if(ctrl->hasFocus())
    {      
        gtk_im_context_filter_keypress(ctrl->im_context, event);    
    }                                                 

    return FALSE; // Let the others receive it
}                                                 

void PKEditableVectorLabel::gtk_im_commit(GtkIMContext *context,
                                          gchar        *str,
                                          gpointer      user_data)
{
    PKEditableVectorLabel *ctrl = (PKEditableVectorLabel *) user_data;

    if(ctrl == NULL)
        return;

    if(ctrl->im_context != context)
        return;

    std::wstring txt = PKStr::utf8StringToWstring(str);
    
    if(txt.size() > 0)
    {
        ctrl->characterInput(txt);    
    }                             
    
    return;                    
}                                                 
                                          
gboolean PKEditableVectorLabel::gtk_im_retrieve_surroundings(GtkIMContext *context,
                                                             gpointer      user_data)
{
    PKEditableVectorLabel *ctrl = (PKEditableVectorLabel *) user_data;

    if(ctrl == NULL)
        return FALSE;

	PKVariant *t = ctrl->get(PKEditableVectorLabel::TEXT_WSTRING_PROPERTY);
	std::wstring text = PKVALUE_WSTRING(t);
    
    std::string utf8 = PKStr::wStringToUTF8string(text);
    
    gtk_im_context_set_surrounding(context, utf8.c_str(), -1, ctrl->barPos);
    
    return TRUE;                    
}                                                 


#endif // LINUX

