//
//
//

#ifndef PK_NUMBER_INPUT_DIALOG_H
#define PK_NUMBER_INPUT_DIALOG_H

#include "PKStdDialog.h"
#include "PKModalDialog.h"

class PKNumberInputDialog : public PKModalDialog
{
public:

	//
	// PROPERTIES
	//

	static std::string TEXT_WSTRING_PROPERTY;
	static std::string ICON_WSTRING_PROPERTY;
    static std::string DEFAULT_TEXT_WSTRING_PROPERTY;

public:

	PKNumberInputDialog(std::wstring    id,
				        PKWindowHandle  parent,
				        std::wstring    caption,
				        std::wstring    icon = L"");

	~PKNumberInputDialog();

    int32_t run();

    int32_t getInputNumber();

protected:

    // From PKWindow/PKDialog

	void windowClosed();
    void buttonClicked(std::string id);

private:
};

#endif // PK_NUMBER_INPUT_DIALOG_H
