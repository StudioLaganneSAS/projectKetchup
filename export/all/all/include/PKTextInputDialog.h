//
//
//

#ifndef PK_TEXT_INPUT_DIALOG_H
#define PK_TEXT_INPUT_DIALOG_H

#include "PKStdDialog.h"
#include "PKModalDialog.h"

class PKTextInputDialog : public PKModalDialog
{
public:

	//
	// PROPERTIES
	//

	static std::string TEXT_WSTRING_PROPERTY;
	static std::string ICON_WSTRING_PROPERTY;
    static std::string DEFAULT_TEXT_WSTRING_PROPERTY;

public:

	PKTextInputDialog(std::wstring    id,
				      PKWindowHandle  parent,
				      std::wstring    caption,
				      std::wstring    icon = L"");

	~PKTextInputDialog();

    int32_t run();

    std::wstring getInputText();

protected:

    // From PKWindow/PKDialog

	void windowClosed();
    void buttonClicked(std::string id);

private:
};

#endif // PK_TEXT_INPUT_DIALOG_H
