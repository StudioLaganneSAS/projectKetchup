//
//
//

#ifndef PK_STD_DIALOG_H
#define PK_STD_DIALOG_H

#include "PKModalDialog.h"

//
// PKStdButtonId
//

typedef enum {

    PK_STD_BUTTON_OK,
    PK_STD_BUTTON_CANCEL,
    PK_STD_BUTTON_YES,
    PK_STD_BUTTON_NO,
    PK_STD_BUTTON_CLOSE,

} PKStdButtonId;

//
// PKStdDialogType
//

typedef enum {

   PK_STD_DIALOG_OK,
   PK_STD_DIALOG_CLOSE,
   PK_STD_DIALOG_OK_CANCEL,
   PK_STD_DIALOG_YES_NO,
   PK_STD_DIALOG_YES_NO_CANCEL,    

} PKStdDialogType;

//
// PKStdDialog
//

class PKStdDialog : public PKModalDialog
{
public:

	//
	// PROPERTIES
	//

	static std::string HEADER_WSTRING_PROPERTY;
	static std::string TEXT_WSTRING_PROPERTY;
	static std::string ICON_WSTRING_PROPERTY;

public:

	PKStdDialog(std::wstring    id,
				PKStdDialogType type,
			    PKWindowHandle  parent,
				std::wstring    caption,
				std::wstring    icon = L"");

	~PKStdDialog();

    int32_t run();

protected:

    // From PKWindow/PKDialog

	void windowClosed();
    void buttonClicked(std::string id);

private:

	PKStdDialogType type;
};

#endif // PK_STD_DIALOG_H
