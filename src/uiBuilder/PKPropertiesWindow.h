//
//
//

#ifndef PK_PROPERTIES_WINDOW_H
#define PK_PROPERTIES_WINDOW_H

#include "PKDialog.h"

//
// PKPropertiesWindow
//

class PKPropertiesWindow : public PKDialog
{
public:

	PKPropertiesWindow();
	~PKPropertiesWindow();

	// Setup

	void setPreviewWindow(PKDialog *p);

	// Bind an object's properties

	void setObject(PKObject *obj);
	void applyUpdates();

	// Overrides

    void buttonClicked(std::string id);

	void comboBoxChanged(std::string id, int32_t previousValue);

	void editTextChanged(std::string id, 
                         std::wstring text);


private:

	PKDialog *previewWindow;
	PKObject *currentObject;
	std::vector<std::string> updateList;
	bool updating;
};

#endif // PK_PROPERTIES_WINDOW_H
