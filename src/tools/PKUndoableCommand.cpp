//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKUndoableCommand.h"

//
// PKUndoableCommand
//

PKUndoableCommand::PKUndoableCommand()
{
	this->displayName = L"";
}

PKUndoableCommand::~PKUndoableCommand()
{
}

void PKUndoableCommand::setDisplayName(std::wstring name)
{
	this->displayName = name;
}

std::wstring PKUndoableCommand::getDisplayName()
{
	return this->displayName;
}
