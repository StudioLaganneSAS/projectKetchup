//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_CHOOSE_FOLDER_DIALOG_H
#define PK_CHOOSE_FOLDER_DIALOG_H

#include "PKTypes.h"
#include "PKUI.h"

#include <string>

//
// PKChooseFolderDialog
//

class PKChooseFolderDialog
{
public:

	PKChooseFolderDialog();
	~PKChooseFolderDialog();

	void setTitle(std::wstring title);

    void setInitialFolder(std::wstring filename);
	std::wstring getInitialFolder();

	// Main Function

	std::wstring showAndGetFolder(PKWindowHandle parent);

private:

	std::wstring  title;
	std::wstring  folder;
};

#endif // PK_CHOOSE_FOLDER_DIALOG_H
