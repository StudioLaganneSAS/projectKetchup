//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKAlertDialog.h"
#include "PKStr.h"

//
// C-Func for quick messages
//

int32_t PKAlert(std::string    imgResource,
                PKWindowHandle parent,
			    std::wstring   title,
				std::wstring   text,
                std::wstring   icon,
				std::wstring   button1, 
				std::wstring   button2, 
				std::wstring   button3)
{
    int32_t result = -2;

    PKAlertDialog *alert = new PKAlertDialog(imgResource, 
                                             parent, 
                                             title, 
                                             text,
                                             icon,
                                             button1, 
                                             button2, 
                                             button3);

    if(alert)
    {
        result = alert->run();
        alert->release();
    }

    return result;
}

//
// PKAlertDialog
//

PKAlertDialog::PKAlertDialog(std::string    imgResource,
                             PKWindowHandle parent,
							 std::wstring   title,
							 std::wstring   text,
                             std::wstring   icon,
							 std::wstring   button1,
							 std::wstring   button2,
							 std::wstring   button3)

: PKDialog(L"PKALERTDIALOG", PK_WINDOW_STYLE_NO_MAX | 
                             PK_WINDOW_STYLE_NO_MIN | 
                             PK_WINDOW_STYLE_NO_RESIZE,
                             NULL, icon)
{
    this->parentW = parent;
    this->exit = false;

	PKVariantWString caption(title);
	this->set(PKWindow::CAPTION_WSTRING_PROPERTY, &caption);

    std::string xml = "<PKVLayout layoutx=\"2\" spacing=\"10\">";

    if(imgResource == "")
    {
        xml +=        "  <PKSpacer type=\"1\" layouty=\"1\" frame=\"0,0,0,5\"/>";
    }

    xml +=            "  <PKHLayout layoutx=\"2\" margin=\"0\">"
					  "    <PKSpacer layoutx=\"2\"/>";

    if(imgResource != "")
    {
        xml +=        "    <PKImageView image=\"res|" + imgResource + "\"/>";
    }

    xml +=            "    <PKLabel id=\"TEXT\" text=\"CHANGE_IT\"/>"
					  "    <PKSpacer layoutx=\"2\"/>"
					  "  </PKHLayout>";

    if(imgResource == "")
    {
        xml +=        "  <PKSpacer type=\"1\" layouty=\"1\" frame=\"0,0,0,5\"/>";
    }

    xml +=            "  <PKHLayout layoutx=\"2\" margin=\"0\">"
					  "    <PKSpacer layoutx=\"2\"/>";

    if(button3 != L"")
    {
        xml +=        "    <PKButton id=\"BUTTON3\" caption=\"3\"/>";
    }

    if(button2 != L"")
    {
	    xml +=        "    <PKButton id=\"BUTTON2\" caption=\"2\"/>";
    }

    xml +=            "    <PKButton id=\"BUTTON1\" caption=\"1\"/>"
#ifdef WIN32
					  "    <PKSpacer layoutx=\"2\"/>"
#endif
					  "  </PKHLayout>"
					  "</PKVLayout>";
	
    this->loadFromXML(xml);

	this->setLabelText("TEXT", text);

	this->setButtonText("BUTTON1", button1);
	
	if(button2 == L"")
	{
		this->hideControl("BUTTON2");
	}
    else
    {
    	this->setButtonText("BUTTON2", button2);
    }

	if(button3 == L"")
	{
		this->hideControl("BUTTON3");
	}
    else
    {
    	this->setButtonText("BUTTON3", button3);
    }
	
#ifndef LINUX
	this->resizeToFit();
#endif
	
	if(this->getWindowWidth() < 300)
	{
		this->resizeWindowAreaTo(300, this->getWindowHeight());
	}
}

PKAlertDialog::~PKAlertDialog()
{
	
}

void PKAlertDialog::windowClosed()
{
	this->lastButton = -1;
	this->hide();

#ifdef WIN32
    if(this->parentW != NULL)
    {
        EnableWindow(parentW, TRUE);
        SetFocus(parentW);
        SetWindowPos(parentW, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
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

void PKAlertDialog::buttonClicked(std::string id)
{
	if(id == "BUTTON1")
	{
		this->lastButton = 0;
	}
	
	if(id == "BUTTON2")
	{
		this->lastButton = 1;
	}

	if(id == "BUTTON3")
	{
		this->lastButton = 2;
	}
	
	this->hide();
	
#ifdef WIN32
    if(this->parentW != NULL)
    {
        EnableWindow(parentW, TRUE);
        SetFocus(parentW);
        SetWindowPos(parentW, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
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

int32_t PKAlertDialog::run()
{
	this->lastButton = -1;
    this->exit = false;
	
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

        PlaySound((LPCTSTR)SND_ALIAS_SYSTEMEXCLAMATION, 
                   NULL, SND_ASYNC|SND_ALIAS_ID);

#endif

#ifdef LINUX

        gtk_window_set_modal(GTK_WINDOW(this->window), TRUE);
        gtk_window_set_position(GTK_WINDOW(this->window), GTK_WIN_POS_CENTER_ON_PARENT);
        gtk_window_set_transient_for(GTK_WINDOW(this->window), GTK_WINDOW(this->parentW));
    
#endif

#ifdef MACOSX

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
	
	this->show();
	
#ifdef WIN32

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
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
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
	
	return this->lastButton;
}

