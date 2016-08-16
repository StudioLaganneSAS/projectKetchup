//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKUndoManager.h"
#include "PKUndoableCommand.h"

//
// PKUndoManager
//

PKUndoManager *PKUndoManager::_instance = NULL;

//
// static getInstance();
//

PKUndoManager *PKUndoManager::getInstance()
{
	if(_instance == NULL)
	{
		_instance = new PKUndoManager();
	}

	return _instance;
}

void PKUndoManager::cleanup()
{
	if(_instance != NULL)
	{
		delete _instance;
		_instance = NULL;
	}
}

//
// PKUndoManager
//

PKUndoManager::PKUndoManager()
{	
	this->protectedFromCommandPush = false;
}

PKUndoManager::~PKUndoManager()
{
	this->clearUndoAndRedoState();
}

bool PKUndoManager::disable()
{
	if(!this->protectedFromCommandPush)
	{
		this->protectedFromCommandPush = true;
		return true;
	}

	return false;
}

void PKUndoManager::enable()
{
	this->protectedFromCommandPush = false;
}

void PKUndoManager::addCommitCallback(PKUndoManagerCommitCallback *callback)
{
    if(callback == NULL)
    {
        return;
    }

	bool found = false;
	
	if(this->callbacks.size() > 0)
    {
        for(uint32_t i=0; i<this->callbacks.size(); i++)
        {
            if(this->callbacks[i] == callback)
			{
				found = true;
				break;
			}
        }
    }
	
	if(found)
	{
		return;
	}
	
    this->callbacks.push_back(callback);
}

void PKUndoManager::removeCommitCallback(PKUndoManagerCommitCallback *callback)
{
	if(this->callbacks.size() > 0)
    {
        for(uint32_t i=0; i<this->callbacks.size(); i++)
        {
            if(this->callbacks[i] == callback)
			{
				this->callbacks.erase(this->callbacks.begin()+i);
				break;
			}
        }
    }	
}


void PKUndoManager::pushCommandAndRunIt(PKUndoableCommand *cmd)
{
	if(cmd == NULL)
	{
		return;
	}

    // Before we proceed, we must see
    // if any pending changes need to be
    // commited

    if(this->callbacks.size() > 0)
    {
        for(uint32_t i=0; i<this->callbacks.size(); i++)
        {
            this->callbacks[i]->undoManagerCommitCallback();
        }
    }

	// Run it
	cmd->doCommand();

	if(this->protectedFromCommandPush)
	{
		// This means we're running from an existing
		// undo() or redo() call and therefore we
		// shouldn't change the stacks at all

		delete cmd;
		return;
	}

	// Add it to undo

	this->undo_list.push_back(cmd);

	// Since we're adding a command
	// we should clear up the redo stack

	for(uint32_t i=0; i<this->redo_list.size(); i++)
	{
		delete this->redo_list[i];
	}

	this->redo_list.clear();
}

void PKUndoManager::pushCommandButDontRunIt(PKUndoableCommand *cmd)
{
	if(cmd == NULL)
	{
		return;
	}

    // Before we proceed, we must see
    // if any pending changes need to be
    // commited

    if(this->callbacks.size() > 0)
    {
        for(uint32_t i=0; i<this->callbacks.size(); i++)
        {
            this->callbacks[i]->undoManagerCommitCallback();
        }
    }

    if(this->protectedFromCommandPush)
	{
		// This means we're running from an existing
		// undo() or redo() call and therefore we
		// shouldn't change the stacks at all

		delete cmd;
		return;
	}

	// Don't run it but
	// add it to undo

	this->undo_list.push_back(cmd);

	// Since we're adding a command
	// we should clear up the redo stack

	for(uint32_t i=0; i<this->redo_list.size(); i++)
	{
		delete this->redo_list[i];
	}

	this->redo_list.clear();
}

bool PKUndoManager::canUndo()
{
	return (this->undo_list.size() > 0);
}

bool PKUndoManager::canRedo()
{
	return (this->redo_list.size() > 0);
}

void PKUndoManager::undo()
{
	if(this->undo_list.size() > 0)
	{
		// Pop the last-added command
		PKUndoableCommand *cmd = this->undo_list[this->undo_list.size() - 1];
		this->undo_list.pop_back();

		// Undo the command
		this->protectedFromCommandPush = true;
		cmd->undoCommand();
		this->protectedFromCommandPush = false;

		// Add it to the redo list
		this->redo_list.push_back(cmd);
	}
}

void PKUndoManager::redo()
{
	if(this->redo_list.size() > 0)
	{
		// Pop the last-added command
		PKUndoableCommand *cmd = this->redo_list[this->redo_list.size() - 1];
		this->redo_list.pop_back();

		// Re-do the command
		this->protectedFromCommandPush = true;
		cmd->redoCommand();
		this->protectedFromCommandPush = false;

		// Add it back to the undo list
		this->undo_list.push_back(cmd);
	}
}

std::wstring PKUndoManager::getUndoCommandDisplayName()
{
	if(this->undo_list.size() > 0)
	{
		return this->undo_list[this->undo_list.size() - 1]->getDisplayName();
	}

	return L"";
}

std::wstring PKUndoManager::getRedoCommandDisplayName()
{
	if(this->redo_list.size() > 0)
	{
		return this->redo_list[this->redo_list.size() - 1]->getDisplayName();
	}

	return L"";
}

void PKUndoManager::clearUndoAndRedoState()
{
	// Clear the lists

	for(uint32_t i=0; i<this->redo_list.size(); i++)
	{
		delete this->redo_list[i];
	}

	this->redo_list.clear();

	for(uint32_t i=0; i<this->undo_list.size(); i++)
	{
		delete this->undo_list[i];
	}

	this->undo_list.clear();
}
