//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_UNDOABLE_COMMAND_H
#define PK_UNDOABLE_COMMAND_H

#include "PKTypes.h"
#include <string>

//
// PKUndoableCommand
// 

class PKUndoableCommand
{
	friend class PKUndoManager;

public:

	PKUndoableCommand();
	virtual ~PKUndoableCommand();

	void         setDisplayName(std::wstring name);
	std::wstring getDisplayName();

	//
	// Implement these
	//

	virtual void doCommand()   = 0;
	virtual void undoCommand() = 0;
	virtual void redoCommand() = 0;

private:
	std::wstring displayName;
};

#endif // PK_UNDOABLE_COMMAND_H
