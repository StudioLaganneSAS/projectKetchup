//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PKALERTDIALOG_H
#define PKALERTDIALOG_H

#include "PKDialog.h"

//
// C-Func for quick messages
//

int32_t PKAlert(std::string    imgResource,
                PKWindowHandle parent,
			    std::wstring   title,
				std::wstring   text,
                std::wstring   icon,
				std::wstring   button1 = L"OK", // Defaults to OK
				std::wstring   button2 = L"",   // Defaults to empty
				std::wstring   button3 = L"");

//
// PKAlertDialog
//

class PKAlertDialog : public PKDialog
{
public:
	
	PKAlertDialog(std::string    imgResource,
                  PKWindowHandle parent,
				  std::wstring   title,
				  std::wstring   text,
                  std::wstring   icon,
				  std::wstring   button1 = L"OK", // Defaults to OK
				  std::wstring   button2 = L"",   // Defaults to empty
				  std::wstring   button3 = L"");  // Defaults to empty
	
	int32_t run();
	
public:

	void windowClosed();

	void buttonClicked(std::string id);

protected:

	~PKAlertDialog();	
	int32_t lastButton;
    PKWindowHandle parentW;
    bool exit;
};

#endif // PKALERTDIALOG_H
