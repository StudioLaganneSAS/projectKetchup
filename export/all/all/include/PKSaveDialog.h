//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SAVE_DIALOG_H
#define PK_SAVE_DIALOG_H

#include "PKTypes.h"
#include "PKUI.h"

#include <string>
#include <vector>

//
// PKSaveDialog 
//

class PKSaveDialog
{
public:

	PKSaveDialog();
	~PKSaveDialog();

	void setTitle(std::wstring title);
	void setInitialFilename(std::wstring filename);

	void addFilterEntry(std::wstring extensions,
				        std::wstring description);


	// Main Function

	std::wstring askForSaveFilename(PKWindowHandle parent);

private:

	std::wstring  title;
	std::wstring  filename;
	
	std::vector <std::wstring> exts;
	std::vector <std::wstring> descs;

public:
	
#ifdef MACOSX
	std::wstring macDialogResult;
	WindowRef parent;
#endif
};

#endif // PK_SAVE_DIALOG_H
