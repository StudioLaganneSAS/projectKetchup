//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CLIPBOARD_H
#define PK_CLIPBOARD_H

#include "PKTypes.h"
#include "PKCritSec.h"
#include <string>

//
// PKClipboard
//

class PKClipboard
{
public:

    PKClipboard();
    ~PKClipboard();

    void clearClipboard();

    void addText(std::wstring text);

    bool containsText();
    std::wstring getText();

private:
    PKCritSec mutex;
};

#endif // PK_CLIPBOARD_H
