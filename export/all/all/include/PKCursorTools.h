//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_CURSOR_TOOLS_H
#define PK_CURSOR_TOOLS_H

#include "PKTypes.h"

typedef enum {

    PK_CURSOR_NONE,
    PK_CURSOR_ARROW,
    PK_CURSOR_HAND,

} PKCursorID;

//
// PKCursorTools
//

class PKCursorTools
{
public:

    static void setCursor(PKCursorID id);

    static void hideCursor();
    static void showCursor();
};


#endif // PK_CURSOR_TOOLS_H
