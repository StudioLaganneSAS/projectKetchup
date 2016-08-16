//
//
//

#include "PKNumberInputDialog.h"
#include "PKNumberInputDialogXML.h"
#include "PKLabel.h"
#include "PKButton.h"
#include "PKEdit.h"
#include "PKStr.h"
#include "PKLittleArrows.h"

//
// Properties
//

std::string PKNumberInputDialog::TEXT_WSTRING_PROPERTY         = "text";
std::string PKNumberInputDialog::ICON_WSTRING_PROPERTY         = "icon";
std::string PKNumberInputDialog::DEFAULT_TEXT_WSTRING_PROPERTY = "defaultText";

//
// PKNumberInputDialog
//
 
PKNumberInputDialog::PKNumberInputDialog(std::wstring  id,
						                PKWindowHandle  parent,
						                std::wstring    caption,
						                std::wstring    icon)
:PKModalDialog(id, 
               PK_WINDOW_STYLE_DEFAULT | 
               PK_WINDOW_STYLE_NO_MAX  | 
               PK_WINDOW_STYLE_NO_RESIZE,
		       parent, icon)
{
	PKOBJECT_ADD_WSTRING_PROPERTY(text, L"NOT_SET");
	PKOBJECT_ADD_WSTRING_PROPERTY(icon, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(defaultText, L"");

	PKVariantWString c(caption);
	this->set(PKWindow::CAPTION_WSTRING_PROPERTY, &c);
}

PKNumberInputDialog::~PKNumberInputDialog()
{
}

int32_t PKNumberInputDialog::run()
{
	PKVariant    *img = this->get(PKNumberInputDialog::ICON_WSTRING_PROPERTY);
	std::wstring icon = PKVALUE_WSTRING(img);
	
	std::string xml = PKNumberInputDialogXML;
	xml = PKStr::replaceString("[[REPLACE_IMG]]", PKStr::wStringToUTF8string(icon), xml);

	this->loadFromXML(xml);

	// Setup texts

	PKVariant *t  = this->get(PKNumberInputDialog::TEXT_WSTRING_PROPERTY);
	PKLabel *text = dynamic_cast<PKLabel*>(this->findControl("ID_TEXT"));
	text->set(PKLabel::TEXT_WSTRING_PROPERTY, t);	

	PKVariant *dt  = this->get(PKNumberInputDialog::DEFAULT_TEXT_WSTRING_PROPERTY);
    this->setEditText("ID_EDIT", PKVALUE_WSTRING(dt));

	// Setup Little Arrows

	PKLittleArrows *arrows = dynamic_cast<PKLittleArrows *>(this->findControl("ID_ARROWS"));
	PKEdit         *edit   = dynamic_cast<PKEdit *>(this->findControl("ID_EDIT"));

	arrows->setBuddyControl(edit);

    // Setup buttons

#ifdef WIN32
		this->setButtonText("ID_BUTTON1", PK_i18n(L"OK"));
		this->setButtonText("ID_BUTTON2", PK_i18n(L"Cancel"));

        PKVariantBool d1(true);
        PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON1"));
        b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

        PKVariantBool d2(false);
        PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
        b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);

#endif
#ifdef LINUX
		this->setButtonText("ID_BUTTON2", PK_i18n(L"OK"));
		this->setButtonText("ID_BUTTON1", PK_i18n(L"Cancel"));

        PKVariantBool d1(false);
        PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON1"));
        b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

        PKVariantBool d2(true);
        PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
        b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);

#endif
#ifdef MACOSX
		this->setButtonText("ID_BUTTON2", PK_i18n(L"OK"));
		this->setButtonText("ID_BUTTON1", PK_i18n(L"Cancel"));

        PKVariantBool d1(false);
        PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON1"));
        b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

        PKVariantBool d2(true);
        PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
        b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);
#endif

	// Resize
	this->resizeToFit();

	// Focus
	this->setFocusToControl("ID_EDIT");
	this->doSelectAll();

    // Run
    return PKModalDialog::run();
}

int32_t PKNumberInputDialog::getInputNumber()
{
	return PKStr::wstringToInt32(this->getEditText("ID_EDIT"));
}

void PKNumberInputDialog::windowClosed()
{
    this->terminateDialog(PK_STD_BUTTON_CANCEL);
}

void PKNumberInputDialog::buttonClicked(std::string id)
{
#ifdef WIN32
    if(id == "ID_BUTTON1")
    {
        this->terminateDialog(PK_STD_BUTTON_OK);
    }

    if(id == "ID_BUTTON2")
    {
        this->terminateDialog(PK_STD_BUTTON_CANCEL);
    }
#endif

#ifdef LINUX
    if(id == "ID_BUTTON2")
    {
        this->terminateDialog(PK_STD_BUTTON_OK);
    }

    if(id == "ID_BUTTON1")
    {
        this->terminateDialog(PK_STD_BUTTON_CANCEL);
    }
#endif

#ifdef MACOSX
    if(id == "ID_BUTTON1")
    {
        this->terminateDialog(PK_STD_BUTTON_CANCEL);
    }

    if(id == "ID_BUTTON2")
    {
        this->terminateDialog(PK_STD_BUTTON_OK);
    }
#endif
}


