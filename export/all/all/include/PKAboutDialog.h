//
//
//

#ifndef PK_ABOUT_DIALOG_H
#define PK_ABOUT_DIALOG_H

#include "PKModalDialog.h"
#include "PKStdDialog.h"

class PKAboutDialog : public PKModalDialog
{
public:

	PKAboutDialog(std::wstring    id,
				  PKWindowHandle  parent,
				  std::wstring    caption,
				  std::wstring    version,
				  std::wstring    copyright,
				  std::wstring    icon = L"",
				  std::wstring    logo = L"");

	~PKAboutDialog();

    // Run

    int32_t run();

protected:
        
    // From PKWindow/PKDialog

	void keyUp(unsigned int modifier,
			   PKKey        keyCode,
			   unsigned int virtualCode);

    void windowClosed();
    void buttonClicked(std::string id);

private:
};

#endif // PK_ABOUT_DIALOG_H
