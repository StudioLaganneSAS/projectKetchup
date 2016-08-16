//
//
//

#include "PKStdDialog.h"
#include "PKStdDialogXML.h"
#include "PKLabel.h"
#include "PKButton.h"
#include "PKStr.h"

//
// Properties
//

std::string PKStdDialog::HEADER_WSTRING_PROPERTY     = "header";
std::string PKStdDialog::TEXT_WSTRING_PROPERTY       = "text";
std::string PKStdDialog::ICON_WSTRING_PROPERTY       = "icon";

//
// PKStdDialog
//
 
PKStdDialog::PKStdDialog(std::wstring  id,
						 PKStdDialogType type,
						 PKWindowHandle  parent,
						 std::wstring    caption,
						 std::wstring    icon)
:PKModalDialog(id, 
               PK_WINDOW_STYLE_DEFAULT | 
               PK_WINDOW_STYLE_NO_MAX  | 
               PK_WINDOW_STYLE_NO_RESIZE,
		       parent, icon)
{
	PKOBJECT_ADD_WSTRING_PROPERTY(header, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(text, L"");
	PKOBJECT_ADD_WSTRING_PROPERTY(icon, L"");

	PKVariantWString c(caption);
	this->set(PKWindow::CAPTION_WSTRING_PROPERTY, &c);

	this->type = type;
}

PKStdDialog::~PKStdDialog()
{
}

int32_t PKStdDialog::run()
{
	PKVariant    *img = this->get(PKStdDialog::ICON_WSTRING_PROPERTY);
	std::wstring icon = PKVALUE_WSTRING(img);
	
	std::string xml = PKStdDialogXML;
	xml = PKStr::replaceString("[[REPLACE_IMG]]", PKStr::wStringToUTF8string(icon), xml);

	this->loadFromXML(xml);

	// Setup texts

	PKVariant *h    = this->get(PKStdDialog::HEADER_WSTRING_PROPERTY);
	PKLabel *header = dynamic_cast<PKLabel*>(this->findControl("ID_HEADER"));
	header->set(PKLabel::TEXT_WSTRING_PROPERTY, h);	

	PKVariant *t  = this->get(PKStdDialog::TEXT_WSTRING_PROPERTY);
	PKLabel *text = dynamic_cast<PKLabel*>(this->findControl("ID_TEXT"));
	text->set(PKLabel::TEXT_WSTRING_PROPERTY, t);	

	// Setup according to dialog type

	switch(this->type)
	{
	case PK_STD_DIALOG_OK:
		{
			this->hideControl("ID_BUTTON1");
			this->hideControl("ID_BUTTON2");

			this->setButtonText("ID_BUTTON3", PK_i18n(L"OK"));

            PKVariantBool d(true);
            PKButton *b = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON3"));
            b->set(PKButton::DEFAULT_BOOL_PROPERTY, &d);
		}
		break;
	case PK_STD_DIALOG_CLOSE:
		{
			this->hideControl("ID_BUTTON1");
			this->hideControl("ID_BUTTON2");

			this->setButtonText("ID_BUTTON3", PK_i18n(L"Close"));

            PKVariantBool d(true);
            PKButton *b = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON3"));
            b->set(PKButton::DEFAULT_BOOL_PROPERTY, &d);
		}
		break;
	case PK_STD_DIALOG_OK_CANCEL:
		{
			this->hideControl("ID_BUTTON1");

#ifdef WIN32
			this->setButtonText("ID_BUTTON2", PK_i18n(L"OK"));
			this->setButtonText("ID_BUTTON3", PK_i18n(L"Cancel"));

            PKVariantBool d1(true);
            PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
            b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

            PKVariantBool d2(false);
            PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON3"));
            b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);

#endif

#ifdef LINUX
			this->setButtonText("ID_BUTTON3", PK_i18n(L"OK"));
			this->setButtonText("ID_BUTTON2", PK_i18n(L"Cancel"));

            PKVariantBool d1(false);
            PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
            b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

            PKVariantBool d2(true);
            PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON3"));
            b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);

#endif

#ifdef MACOSX
			this->setButtonText("ID_BUTTON2", PK_i18n(L"OK"));
			this->setButtonText("ID_BUTTON3", PK_i18n(L"Cancel"));

            PKVariantBool d1(false);
            PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
            b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

            PKVariantBool d2(true);
            PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON3"));
            b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);
#endif
		}
		break;
	case PK_STD_DIALOG_YES_NO:
		{
			this->hideControl("ID_BUTTON1");

			this->setButtonText("ID_BUTTON2", PK_i18n(L"Yes"));
			this->setButtonText("ID_BUTTON3", PK_i18n(L"No"));

            PKVariantBool d1(true);
            PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
            b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

            PKVariantBool d2(false);
            PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON3"));
            b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);
		}
		break;
	case PK_STD_DIALOG_YES_NO_CANCEL:
		{
			this->setButtonText("ID_BUTTON1", PK_i18n(L"Cancel"));
			this->setButtonText("ID_BUTTON2", PK_i18n(L"Yes"));
			this->setButtonText("ID_BUTTON3", PK_i18n(L"No"));

            PKVariantBool d1(true);
            PKButton *b1 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON2"));
            b1->set(PKButton::DEFAULT_BOOL_PROPERTY, &d1);

            PKVariantBool d2(false);
            PKButton *b2 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON3"));
            b2->set(PKButton::DEFAULT_BOOL_PROPERTY, &d2);

            PKVariantBool d3(false);
            PKButton *b3 = dynamic_cast<PKButton*>(this->findControl("ID_BUTTON1"));
            b3->set(PKButton::DEFAULT_BOOL_PROPERTY, &d3);
        }
		break;
	}

	// Resize

	this->resizeToFit();

	// Run

    return PKModalDialog::run();
}

void PKStdDialog::windowClosed()
{
    this->terminateDialog(PK_STD_BUTTON_CANCEL);
}

void PKStdDialog::buttonClicked(std::string id)
{
    switch(this->type)
    {
	case PK_STD_DIALOG_OK:
		{
            this->terminateDialog(PK_STD_BUTTON_OK);
		}
		break;
	case PK_STD_DIALOG_CLOSE:
		{
            this->terminateDialog(PK_STD_BUTTON_CLOSE);
		}
		break;
	case PK_STD_DIALOG_OK_CANCEL:
		{
#ifdef WIN32
	        if(id == "ID_BUTTON2")
	        {
		        this->terminateDialog(PK_STD_BUTTON_OK);
	        }

	        if(id == "ID_BUTTON3")
	        {
		        this->terminateDialog(PK_STD_BUTTON_CANCEL);
	        }
#endif

#ifdef LINUX
	        if(id == "ID_BUTTON3")
	        {
		        this->terminateDialog(PK_STD_BUTTON_OK);
	        }

	        if(id == "ID_BUTTON2")
	        {
		        this->terminateDialog(PK_STD_BUTTON_CANCEL);
	        }
#endif

#ifdef MACOSX
	        if(id == "ID_BUTTON2")
	        {
		        this->terminateDialog(PK_STD_BUTTON_CANCEL);
	        }

	        if(id == "ID_BUTTON3")
	        {
		        this->terminateDialog(PK_STD_BUTTON_OK);
	        }
#endif
		}
		break;
	case PK_STD_DIALOG_YES_NO:
		{
	        if(id == "ID_BUTTON2")
	        {
		        this->terminateDialog(PK_STD_BUTTON_YES);
	        }

	        if(id == "ID_BUTTON3")
	        {
		        this->terminateDialog(PK_STD_BUTTON_NO);
	        }
		}
		break;
	case PK_STD_DIALOG_YES_NO_CANCEL:
		{
	        if(id == "ID_BUTTON1")
	        {
		        this->terminateDialog(PK_STD_BUTTON_CANCEL);
	        }

            if(id == "ID_BUTTON2")
	        {
		        this->terminateDialog(PK_STD_BUTTON_YES);
	        }

	        if(id == "ID_BUTTON3")
	        {
		        this->terminateDialog(PK_STD_BUTTON_NO);
	        }
		}
		break;
    }
}


