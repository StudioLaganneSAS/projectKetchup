//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_MODAL_DIALOG_H
#define PK_MODAL_DIALOG_H

#include "PKDialog.h"

//
// PKModalDialog
//

class PKModalDialog : public PKDialog
{
public:

    PKModalDialog(std::wstring  id,
       			  PKWindowStyle flags,
			      PKWindowHandle parent = NULL,
                  std::wstring   icon   = L"");

    ~PKModalDialog();

    //
    // Main 
    //

    virtual int32_t run();

    //
    // Util, overide to terminate dialog
    //

    virtual void terminateDialog(int32_t returnCode);

private:

	bool           exit;
	PKWindowHandle parentW;
    int32_t        returnCode;
};

#endif // PK_MODAL_DIALOG_H
