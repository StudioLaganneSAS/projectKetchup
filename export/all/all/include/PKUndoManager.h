//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_UNDO_MANAGER_H
#define PK_UNDO_MANAGER_H

#include "PKTypes.h"
#include <string>
#include <vector>

class PKUndoableCommand;

//
// PKUndoManagerCallback
//

class PKUndoManagerCommitCallback
{
public:
    virtual void undoManagerCommitCallback() = 0;
};

//
// PKUndoManager
//

class PKUndoManager
{
	friend class PKUndoableCommand;

public:

	//
	// Static singleton getter & cleanup
	//

	static PKUndoManager *getInstance();
	static void cleanup();

	//
	// Member functions
	// 

	void pushCommandAndRunIt(PKUndoableCommand *cmd);
	void pushCommandButDontRunIt(PKUndoableCommand *cmd);

	bool canUndo();
	bool canRedo();

	void undo();
	void redo();

	std::wstring getUndoCommandDisplayName();
	std::wstring getRedoCommandDisplayName();

	void clearUndoAndRedoState();

	bool disable();
	void enable();

    // These are for controls that
    // want a chance to commit something
    // before anything else is added to
    // the undo list. It is useful for 
    // edit controls, they register when
    // they gain focus, that way they are sure
    // that they will be able to commit
    // when they loose the focus to another
    // action being added to undo.

    void addCommitCallback(PKUndoManagerCommitCallback *callback);
	void removeCommitCallback(PKUndoManagerCommitCallback *callback);

protected:
	
	PKUndoManager();
	~PKUndoManager();

	static PKUndoManager *_instance;

	std::vector<PKUndoableCommand *> undo_list;
	std::vector<PKUndoableCommand *> redo_list;

    std::vector<PKUndoManagerCommitCallback *> callbacks;

	bool protectedFromCommandPush;
};

#endif // PK_UNDO_MANAGER_H
