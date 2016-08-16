//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_OPEN_DIALOG_H
#define PK_OPEN_DIALOG_H

#include "PKTypes.h"
#include "PKUI.h"

#include <string>
#include <vector>

//
// PKOpenDialog 
//

class PKOpenDialog
{
public:

	PKOpenDialog();
	~PKOpenDialog();

	void setTitle(std::wstring title);

    void setInitialFolder(std::wstring folder);

    void setInitialFilename(std::wstring filename);

	void addFilterEntry(std::wstring extensions,
				        std::wstring description);


	// Main Function

	std::wstring askForOpenFilename(PKWindowHandle parent);

private:

	std::wstring  title;
	std::wstring  filename;
	std::wstring  folder;
	
	std::vector <std::wstring> exts;
	std::vector <std::wstring> descs;
	
public:
	
#ifdef MACOSX
	std::wstring macDialogResult;
	WindowRef parent;
#endif
};

#endif // PK_OPEN_DIALOG_H
