//
//
//

#include "PKAboutDialog.h"
#include "PKAboutDialog_XML.h"
#include "PKLabel.h"
#include "PKButton.h"
#include "PKStr.h"

//
// PKTextInputDialog
//
 
PKAboutDialog::PKAboutDialog(std::wstring  id,
						     PKWindowHandle  parent,
						     std::wstring    caption,
						     std::wstring    version,
						     std::wstring    copyright,
						     std::wstring    icon,
						     std::wstring    logo)

:PKModalDialog(id, PK_WINDOW_STYLE_DEFAULT | 
			       PK_WINDOW_STYLE_NO_MAX  | 
			       PK_WINDOW_STYLE_NO_RESIZE,
		           parent, icon)
{
	PKVariantWString cpt(PK_i18n(L"About") + caption);
	this->set(PKWindow::CAPTION_WSTRING_PROPERTY, &cpt);

	std::string xml = aboutXML;
	xml = PKStr::replaceString("[[REPLACE]]", PKStr::wStringToUTF8string(logo), xml);

	this->loadFromXML(xml);

	// Setup texts

    PKVariantWString t(caption);
	PKLabel *title = dynamic_cast<PKLabel*>(this->findControl("ID_HEADER_TITLE"));
	title->set(PKLabel::TEXT_WSTRING_PROPERTY, &t);	

    PKVariantWString s(version);
	PKLabel *subtitle = dynamic_cast<PKLabel*>(this->findControl("ID_HEADER_SUBTITLE"));
	subtitle->set(PKLabel::TEXT_WSTRING_PROPERTY, &s);	

    PKVariantWString c(copyright);
	PKLabel *cp = dynamic_cast<PKLabel*>(this->findControl("ID_COPYRIGHT"));
	cp->set(PKLabel::TEXT_WSTRING_PROPERTY, &c);	
}

PKAboutDialog::~PKAboutDialog()
{
}

int32_t PKAboutDialog::run()
{	
	// Resize
	this->resizeToFit();
    return PKModalDialog::run();
}

void PKAboutDialog::windowClosed()
{
    this->terminateDialog(PK_STD_BUTTON_CANCEL);
}

void PKAboutDialog::buttonClicked(std::string id)
{
    this->terminateDialog(PK_STD_BUTTON_OK);
}

void PKAboutDialog::keyUp(unsigned int modifier,
			              PKKey        keyCode,
			              unsigned int virtualCode)
{
    if(keyCode == PK_KEY_ESCAPE)
    {
        this->terminateDialog(PK_STD_BUTTON_CANCEL);
    }
}

