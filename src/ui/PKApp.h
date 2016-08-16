//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_APP_H
#define PK_APP_H

#include "PKTypes.h"
#include "PKUI.h"
#include "PKStr.h"
#include "PKMenu.h"
#include "PKMenuItem.h"
#include "PKWindow.h"
#include "PKBitmapOps.h"
#include "PKVLayout.h"
#include "PKSafeEvents.h"
#include "PKCustomControl.h"
#include "PKClipboard.h"
#include "PKUndoManager.h"
#include "PKDialog.h"

#ifdef MACOSX
#include <unistd.h>
#include <Carbon/Carbon.h>
#endif // MACOSX

class PKApp
{
public:

    // PKApp
    PKApp();
    ~PKApp();

    // Utilities

    void quit();
	void installRootMenu(PKWindow *window, PKMenu *menu);
	void updateUIStrings();
	
    // Implement these

    virtual int32_t appStartupDelegate(std::wstring commandLine);
    virtual int32_t appShutdownDelegate();
};

#endif // PK_APP_H

