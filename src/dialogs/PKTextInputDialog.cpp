//
//
//

#include "PKTextInputDialog.h"
#include "PKTextInputDialogXML.h"
#include "PKLabel.h"
#include "PKButton.h"
#include "PKEdit.h"
#include "PKStr.h"

//
// Properties
//

std::string PKTextInputDialog::TEXT_WSTRING_PROPERTY         = "text";
std::string PKTextInputDialog::ICON_WSTRING_PROPERTY         = "icon";
std::string PKTextInputDialog::DEFAULT_TEXT_WSTRING_PROPERTY = "defaultText";

//
// PKTextInputDialog
//
 
PKTextInputDialog::PKTextInputDialog(std::wstring  id,
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

PKTextInputDialog::~PKTextInputDialog()
{
}

int32_t PKTextInputDialog::run()
{
	PKVariant    *img = this->get(PKTextInputDialog::ICON_WSTRING_PROPERTY);
	std::wstring icon = PKVALUE_WSTRING(img);
	
	std::string xml = PKTextInputDialogXML;
	xml = PKStr::replaceString("[[REPLACE_IMG]]", PKStr::wStringToUTF8string(icon), xml);

	this->loadFromXML(xml);

	// Setup texts

	PKVariant *t  = this->get(PKTextInputDialog::TEXT_WSTRING_PROPERTY);
	PKLabel *text = dynamic_cast<PKLabel*>(this->findControl("ID_TEXT"));
	text->set(PKLabel::TEXT_WSTRING_PROPERTY, t);	

	PKVariant *dt  = this->get(PKTextInputDialog::DEFAULT_TEXT_WSTRING_PROPERTY);
    this->setEditText("ID_EDIT", PKVALUE_WSTRING(dt));

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

std::wstring PKTextInputDialog::getInputText()
{
    return this->getEditText("ID_EDIT");
}

void PKTextInputDialog::windowClosed()
{
    this->terminateDialog(PK_STD_BUTTON_CANCEL);
}

void PKTextInputDialog::buttonClicked(std::string id)
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


