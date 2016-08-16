//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKModalDialog.h"

#ifdef WIN32
extern HWND pkActiveWindow;
#endif

//
//
//

PKModalDialog::PKModalDialog(std::wstring   id,
   			                 PKWindowStyle  flags,
		                     PKWindowHandle parent,
                             std::wstring   icon)
: PKDialog(id, flags, parent, icon) 
{
	this->exit       = false;
	this->parentW    = parent;
    this->returnCode = -1;    
}

PKModalDialog::~PKModalDialog()
{

}

//
// Main 
//

int32_t PKModalDialog::run()
{
	// Place according to parent

    if(this->parentW != NULL)
    {
#ifdef WIN32

        RECT parentRect;
        GetWindowRect(parentW, &parentRect);

        int parentWidth  = (parentRect.right - parentRect.left);
        int parentHeight = (parentRect.bottom - parentRect.top);

        int x = parentRect.left + (parentWidth  - this->getWindowWidth())/2;
        int y = parentRect.top  + (parentHeight - this->getWindowHeight())/2;

        this->moveWindowTo(x, y);

        EnableWindow(parentW, FALSE);

#endif

#ifdef LINUX

        gtk_window_set_modal(GTK_WINDOW(this->window), TRUE);
        gtk_window_set_position(GTK_WINDOW(this->window), GTK_WIN_POS_CENTER_ON_PARENT);
        gtk_window_set_transient_for(GTK_WINDOW(this->window), GTK_WINDOW(this->parentW));
    
#endif

#ifdef MACOSX
		
		// Sheet window are placed automatically

		Rect parentRect;
		GetWindowBounds(GetWindowFromPort(this->parentW), kWindowStructureRgn, &parentRect);
		
        int parentWidth  = (parentRect.right - parentRect.left);
        int parentHeight = (parentRect.bottom - parentRect.top);
		
        int x = parentRect.left + (parentWidth  - this->getWindowWidth())/2;
        int y = parentRect.top  + (parentHeight - this->getWindowHeight())/2;
		
        this->moveWindowTo(x, y);
		
#endif
	}
	else
	{
		this->centerOnScreen();
	}

	// Show
	this->show();

    // Setup button ID
    this->returnCode = -1;

	// Run event loop

#ifdef WIN32

    this->exit = false;

    BOOL bRet;
    MSG msg;

    while((bRet = GetMessage(&msg, NULL, 0, 0 )) != 0)
    { 
        if(bRet == -1)
        {
            // handle the error and possibly exit
            this->exit = true;
            break;
        }
        else
        {
		    if(pkActiveWindow == NULL || 
			  !IsDialogMessage(pkActiveWindow, &msg))
	        {
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            }
        }

        if(this->exit)
        {
            break;
        }
    } 

#endif

#ifdef LINUX
    gtk_main();
#endif

#ifdef MACOSX
	RunAppModalLoopForWindow(this->macwindow);
#endif

	return this->returnCode;
}

//
// Util, overide to terminate dialog
//

void PKModalDialog::terminateDialog(int32_t rc)
{
    this->returnCode = rc;
	this->hide();

#ifdef WIN32
    if(this->parentW != NULL)
    {
        EnableWindow(parentW, TRUE);
        SetActiveWindow(parentW);
        SetFocus(parentW);

        SetWindowPos(parentW, HWND_TOPMOST, 
                     0, 0, 0, 0, 
                     SWP_NOMOVE|SWP_NOSIZE);

        SetWindowPos(parentW, HWND_NOTOPMOST, 
                     0, 0, 0, 0, 
                     SWP_NOMOVE|SWP_NOSIZE);
    }
#endif

#ifdef LINUX
    gtk_main_quit();
#endif

#ifdef MACOSX
	QuitAppModalLoopForWindow(this->macwindow);	
#endif

    this->exit = true;    
}
