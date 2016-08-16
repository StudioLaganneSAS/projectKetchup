///
//
///

#include "PKPropertiesWindow.h"
#include "PKComboBox.h"
#include "PKLabel.h"
#include "PKEdit.h"
#include "PKHLayout.h"
#include "PKStr.h"
#include "PKButton.h"
#include "PKOpenDialog.h"

//
// PKPropertiesWindow
//

PKPropertiesWindow::PKPropertiesWindow()
: PKDialog(L"PK_PROPERTIES_WINDOW",
		   PK_WINDOW_STYLE_DEFAULT | PK_WINDOW_STYLE_NO_MAX,
		   NULL, L"ICON")
{
	std::string xml = "<PKVLayout layoutx=\"2\" layouty=\"2\" id=\"ID_ROOT\" margin=\"5\" spacing=\"2\">"
			 		  "  <PKLabel layoutx=\"2\" id=\"ID_HEADER\" text=\"Properties:\"/>"
                      "  <PKLineControl layoutx=\"2\" color=\"150,150,150,255\" />"
					  "    <PKGridLayout id=\"ID_GRID\" cols=\"2\" layoutx=\"2\" layouty=\"2\">"
					  "    </PKGridLayout>"
                      "  <PKLineControl layoutx=\"2\" color=\"150,150,150,255\" />"
					  "  <PKHLayout layoutx=\"2\">"
					  "    <PKSpacer type=\"1\" layoutx=\"2\" />"
					  "    <PKButton id=\"ID_APPLY\" caption=\"Apply\" enabled=\"false\" default=\"true\"/>"
					  "  </PKHLayout>"
					  "</PKVLayout>";

	PKVariantWString caption(L"Properties");
	this->set(PKWindow::CAPTION_WSTRING_PROPERTY, &caption);

	this->loadFromXML(xml);
	this->currentObject = NULL;
	this->previewWindow = NULL;
	this->updating = false;
}

PKPropertiesWindow::~PKPropertiesWindow()
{
}

void PKPropertiesWindow::setPreviewWindow(PKDialog *p)
{
	this->previewWindow = p;
}

void PKPropertiesWindow::setObject(PKObject *obj)
{
	// Get a handle to the grid layout
	// and clear its children before adding
	// back items for the new properties

	this->updating = true;

	if(obj == NULL)
	{
		PKControl *grid = this->findControl("ID_GRID");

		if(grid)
		{
			grid->deleteAllChildControls();
		}

		this->updateList.clear();
		this->updating = false;

		return;
	}

	if(this->currentObject != NULL)
	{
		// Apply pending updates first

		if(this->updateList.size() > 0)
		{
			this->applyUpdates();
		}
	}

	this->updateList.clear();
	this->currentObject = obj;

	PKControl *grid = this->findControl("ID_GRID");

	if(grid)
	{
		grid->deleteAllChildControls();

		PKPropertyBag *props = obj->getProperties();

		for(uint32_t i=0; i < props->getNumberOfProperties(); i++)
		{
			std::string name = props->getPropertyAt(i)->getName();

			// Create label for property name

			PKLabel *label = new PKLabel();
			grid->addChild(label);
			label->setText(PKStr::toUpper(PKStr::stringToWString(name.substr(0,1))) + 
		 			   	   PKStr::stringToWString(name.substr(1)) + L":");
			label->build();
			label->initialize(this);

			// Create control for property value

			PKVariant *value = props->getPropertyAt(i)->getValue();

			switch(value->_type)
			{
				case PK_VARIANT_NULL:
				case PK_VARIANT_VOIDP:
					{
						// Create an empty label, since
						// this is not editable

						PKLabel *label = new PKLabel();
						grid->addChild(label);
						label->setText(L"N/A");
						label->build();
						label->initialize(this);
						label->disable();
					}
					break;
				
				case PK_VARIANT_BOOL:
					{
						PKComboBox *combo = new PKComboBox();
						PKVariantString id(name);
						combo->set(PKControl::ID_STRING_PROPERTY, &id);

						grid->addChild(combo);
						combo->addItem(L"True");
						combo->addItem(L"False");
						combo->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						combo->build();
						combo->initialize(this);
						
						if(PKVALUE_BOOL(value))
						{
							combo->selectItem(0);
						}
						else
						{
							combo->selectItem(1);
						}
					}
					break;
				
				case PK_VARIANT_INT64:
				case PK_VARIANT_INT32:
				case PK_VARIANT_INT16:
				case PK_VARIANT_INT8:
				case PK_VARIANT_UINT64:
				case PK_VARIANT_UINT32:
				case PK_VARIANT_UINT16:
				case PK_VARIANT_UINT8:
				case PK_VARIANT_DOUBLE:
					{
						PKEdit *edit = new PKEdit();
						PKVariantString id(name);
						edit->set(PKControl::ID_STRING_PROPERTY, &id);

						grid->addChild(edit);
						PKVariantRect frame(0, 0, 150, 20);
						edit->set(PKControl::FRAME_RECT_PROPERTY, &frame);
						PKVariantInt32 type(PK_EDIT_TYPE_NUMBER);
						edit->set(PKEdit::TYPE_INT32_PROPERTY, &type);
						edit->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						edit->build();
						edit->initialize(this);
						edit->setText(PKStr::stringToWString(value->toString()));
					}
					break;

				case PK_VARIANT_WSTRING:
					{
						PKHLayout *layout = new PKHLayout();
						layout->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						layout->setMargin(0);
						layout->setSpacing(2);
						grid->addChild(layout);
						
						PKEdit *edit = new PKEdit();
						PKVariantString id(name);
						edit->set(PKControl::ID_STRING_PROPERTY, &id);
						edit->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						layout->addChild(edit);

						std::wstring wstr = PKVALUE_WSTRING(value);

						if(wstr == L"NOT_SET")
						{
							layout->build();
							layout->initialize(this);
							edit->setText(L"");
						}
						else if(wstr == L"IMG_NOT_SET")
						{
							PKButton *button = new PKButton();
							PKVariantString id("BUTTON_" + name);
							button->set(PKControl::ID_STRING_PROPERTY, &id);
							layout->addChild(button);
							button->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_FIXED);
							PKVariantRect frame(0, 0, 25, 21);
							button->set(PKControl::FRAME_RECT_PROPERTY, &frame);
							PKVariantBool ov(true);
							button->set(PKButton::OVERRIDE_MIN_SIZE_BOOL_PROPERTY, &ov);
							button->setCaption(L"...");
							layout->build();
							layout->initialize(this);
							edit->setText(L"");
						}
						else
						{
							layout->build();
							layout->initialize(this);
							edit->setText(wstr);
						}
					}
					break;
								
				case PK_VARIANT_STRING:
				case PK_VARIANT_STRINGLIST:
				case PK_VARIANT_WSTRINGLIST:
					{
						PKEdit *edit = new PKEdit();
						PKVariantString id(name);
						edit->set(PKControl::ID_STRING_PROPERTY, &id);

						grid->addChild(edit);
						PKVariantRect frame(0, 0, 150, 20);
						edit->set(PKControl::FRAME_RECT_PROPERTY, &frame);
						edit->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						edit->build();
						edit->initialize(this);
						
						if(value->_type == PK_VARIANT_STRING ||
						   value->_type == PK_VARIANT_STRINGLIST)
						{
							std::string str = value->toString();

							if(str == "NOT_SET")
							{
								edit->setText(L"");
							}
							else
							{
								edit->setText(PKStr::stringToWString(str));
							}
						}

						if(value->_type == PK_VARIANT_WSTRINGLIST)
						{
							std::vector<std::wstring> strlist = PKVALUE_WSTRINGLIST(value);
							std::wstring wstr;

							for(uint32_t i=0; i < strlist.size(); i++)
							{
								wstr += strlist[i];
								if(i != strlist.size()-1)
								{
									wstr += L",";
								}
							}

							edit->setText(wstr);
						}
					}
					break;
				
				case PK_VARIANT_POINT:
					{
						PKHLayout *layout = new PKHLayout();
						layout->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						layout->setMargin(0);
						layout->setSpacing(2);
						grid->addChild(layout);

						PKLabel *label1 = new PKLabel();
						layout->addChild(label1);
						label1->setText(L"x:");
						label1->build();
						label1->initialize(this);

						PKEdit *edit1 = new PKEdit();
						PKVariantString id(name+":x");
						edit1->set(PKControl::ID_STRING_PROPERTY, &id);

						layout->addChild(edit1);
						PKVariantInt32 type1(PK_EDIT_TYPE_NUMBER);
						edit1->set(PKEdit::TYPE_INT32_PROPERTY, &type1);
						PKVariantRect frame1(0,0,30,20);
						edit1->set(PKControl::FRAME_RECT_PROPERTY, &frame1);
						edit1->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit1->build();
						edit1->initialize(this);
						edit1->setText(PKStr::int32ToWstring(PKVALUE_POINT(value).x));

						PKLabel *label2 = new PKLabel();
						layout->addChild(label2);
						label2->setText(L"y:");
						label2->build();
						label2->initialize(this);

						PKEdit *edit2 = new PKEdit();
						PKVariantString id2(name+":y");
						edit2->set(PKControl::ID_STRING_PROPERTY, &id2);

						layout->addChild(edit2);
						PKVariantInt32 type2(PK_EDIT_TYPE_NUMBER);
						edit2->set(PKEdit::TYPE_INT32_PROPERTY, &type2);
						PKVariantRect frame2(0,0,30,20);
						edit2->set(PKControl::FRAME_RECT_PROPERTY, &frame2);
						edit2->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit2->build();
						edit2->initialize(this);
						edit2->setText(PKStr::int32ToWstring(PKVALUE_POINT(value).y));
					}
					break;

				case PK_VARIANT_RECT:
					{
						PKHLayout *layout = new PKHLayout();
						layout->setMargin(0);
						layout->setSpacing(2);
						layout->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						grid->addChild(layout);

						PKLabel *label1 = new PKLabel();
						layout->addChild(label1);
						label1->setText(L"x:");
						label1->build();
						label1->initialize(this);

						PKEdit *edit1 = new PKEdit();
						PKVariantString id(name+":x");
						edit1->set(PKControl::ID_STRING_PROPERTY, &id);

						layout->addChild(edit1);
						PKVariantInt32 type1(PK_EDIT_TYPE_NUMBER);
						edit1->set(PKEdit::TYPE_INT32_PROPERTY, &type1);
						PKVariantRect frame1(0,0,30,20);
						edit1->set(PKControl::FRAME_RECT_PROPERTY, &frame1);
						edit1->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit1->build();
						edit1->initialize(this);
						edit1->setText(PKStr::int32ToWstring(PKVALUE_RECT(value).x));

						PKLabel *label2 = new PKLabel();
						layout->addChild(label2);
						label2->setText(L"y:");
						label2->build();
						label2->initialize(this);

						PKEdit *edit2 = new PKEdit();
						PKVariantString id2(name+":y");
						edit2->set(PKControl::ID_STRING_PROPERTY, &id2);

						layout->addChild(edit2);
						PKVariantInt32 type2(PK_EDIT_TYPE_NUMBER);
						edit2->set(PKEdit::TYPE_INT32_PROPERTY, &type2);
						PKVariantRect frame2(0,0,30,20);
						edit2->set(PKControl::FRAME_RECT_PROPERTY, &frame2);
						edit2->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit2->build();
						edit2->initialize(this);
						edit2->setText(PKStr::int32ToWstring(PKVALUE_RECT(value).y));

						PKLabel *label3 = new PKLabel();
						layout->addChild(label3);
						label3->setText(L"w:");
						label3->build();
						label3->initialize(this);

						PKEdit *edit3 = new PKEdit();
						PKVariantString id3(name+":w");
						edit3->set(PKControl::ID_STRING_PROPERTY, &id3);

						layout->addChild(edit3);
						PKVariantInt32 type3(PK_EDIT_TYPE_NUMBER);
						edit3->set(PKEdit::TYPE_INT32_PROPERTY, &type3);
						PKVariantRect frame3(0,0,30,20);
						edit3->set(PKControl::FRAME_RECT_PROPERTY, &frame3);
						edit3->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit3->build();
						edit3->initialize(this);
						edit3->setText(PKStr::int32ToWstring(PKVALUE_RECT(value).w));

						PKLabel *label4 = new PKLabel();
						layout->addChild(label4);
						label4->setText(L"h:");
						label4->build();
						label4->initialize(this);

						PKEdit *edit4 = new PKEdit();
						PKVariantString id4(name+":h");
						edit4->set(PKControl::ID_STRING_PROPERTY, &id4);

						layout->addChild(edit4);
						PKVariantInt32 type4(PK_EDIT_TYPE_NUMBER);
						edit4->set(PKEdit::TYPE_INT32_PROPERTY, &type4);
						PKVariantRect frame4(0,0,30,20);
						edit4->set(PKControl::FRAME_RECT_PROPERTY, &frame4);
						edit4->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit4->build();
						edit4->initialize(this);
						edit4->setText(PKStr::int32ToWstring(PKVALUE_RECT(value).h));
					}
					break;

				case PK_VARIANT_COLOR:
					{
						PKHLayout *layout = new PKHLayout();
						layout->setMargin(0);
						layout->setSpacing(2);
						layout->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						grid->addChild(layout);

						PKLabel *label1 = new PKLabel();
						layout->addChild(label1);
						label1->setText(L"r:");
						label1->build();
						label1->initialize(this);

						PKEdit *edit1 = new PKEdit();
						PKVariantString id(name+":r");
						edit1->set(PKControl::ID_STRING_PROPERTY, &id);

						layout->addChild(edit1);
						PKVariantInt32 type1(PK_EDIT_TYPE_NUMBER);
						edit1->set(PKEdit::TYPE_INT32_PROPERTY, &type1);
						PKVariantRect frame1(0,0,30,20);
						edit1->set(PKControl::FRAME_RECT_PROPERTY, &frame1);
						edit1->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit1->build();
						edit1->initialize(this);
						edit1->setText(PKStr::int32ToWstring(PKVALUE_COLOR(value).r));

						PKLabel *label2 = new PKLabel();
						layout->addChild(label2);
						label2->setText(L"g:");
						label2->build();
						label2->initialize(this);

						PKEdit *edit2 = new PKEdit();
						PKVariantString id2(name+":g");
						edit2->set(PKControl::ID_STRING_PROPERTY, &id2);

						layout->addChild(edit2);
						PKVariantInt32 type2(PK_EDIT_TYPE_NUMBER);
						edit2->set(PKEdit::TYPE_INT32_PROPERTY, &type2);
						PKVariantRect frame2(0,0,30,20);
						edit2->set(PKControl::FRAME_RECT_PROPERTY, &frame2);
						edit2->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit2->build();
						edit2->initialize(this);
						edit2->setText(PKStr::int32ToWstring(PKVALUE_COLOR(value).g));

						PKLabel *label3 = new PKLabel();
						layout->addChild(label3);
						label3->setText(L"b:");
						label3->build();
						label3->initialize(this);

						PKEdit *edit3 = new PKEdit();
						PKVariantString id3(name+":b");
						edit3->set(PKControl::ID_STRING_PROPERTY, &id3);

						layout->addChild(edit3);
						PKVariantInt32 type3(PK_EDIT_TYPE_NUMBER);
						edit3->set(PKEdit::TYPE_INT32_PROPERTY, &type3);
						PKVariantRect frame3(0,0,30,20);
						edit3->set(PKControl::FRAME_RECT_PROPERTY, &frame3);
						edit3->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit3->build();
						edit3->initialize(this);
						edit3->setText(PKStr::int32ToWstring(PKVALUE_COLOR(value).b));

						PKLabel *label4 = new PKLabel();
						layout->addChild(label4);
						label4->setText(L"a:");
						label4->build();
						label4->initialize(this);

						PKEdit *edit4 = new PKEdit();
						PKVariantString id4(name+":a");
						edit4->set(PKControl::ID_STRING_PROPERTY, &id4);

						layout->addChild(edit4);
						PKVariantInt32 type4(PK_EDIT_TYPE_NUMBER);
						edit4->set(PKEdit::TYPE_INT32_PROPERTY, &type4);
						PKVariantRect frame4(0,0,30,20);
						edit4->set(PKControl::FRAME_RECT_PROPERTY, &frame4);
						edit4->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit4->build();
						edit4->initialize(this);
						edit4->setText(PKStr::int32ToWstring(PKVALUE_COLOR(value).a));
					}
					break;

				case PK_VARIANT_INT32RANGE:
					{
						PKHLayout *layout = new PKHLayout();
						layout->setMargin(0);
						layout->setSpacing(2);
						layout->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						grid->addChild(layout);

						PKLabel *label1 = new PKLabel();
						layout->addChild(label1);
						label1->setText(L"Min:");
						label1->build();
						label1->initialize(this);

						PKEdit *edit1 = new PKEdit();
						PKVariantString id(name+":min");
						edit1->set(PKControl::ID_STRING_PROPERTY, &id);

						layout->addChild(edit1);
						PKVariantInt32 type1(PK_EDIT_TYPE_NUMBER);
						edit1->set(PKEdit::TYPE_INT32_PROPERTY, &type1);
						PKVariantRect frame1(0,0,30,20);
						edit1->set(PKControl::FRAME_RECT_PROPERTY, &frame1);
						edit1->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit1->build();
						edit1->initialize(this);
						edit1->setText(PKStr::int32ToWstring(PKVALUE_INT32RANGE(value).min));

						PKLabel *label2 = new PKLabel();
						layout->addChild(label2);
						label2->setText(L"Max:");
						label2->build();
						label2->initialize(this);

						PKEdit *edit2 = new PKEdit();
						PKVariantString id2(name+":max");
						edit2->set(PKControl::ID_STRING_PROPERTY, &id2);

						layout->addChild(edit2);
						PKVariantInt32 type2(PK_EDIT_TYPE_NUMBER);
						edit2->set(PKEdit::TYPE_INT32_PROPERTY, &type2);
						PKVariantRect frame2(0,0,30,20);
						edit2->set(PKControl::FRAME_RECT_PROPERTY, &frame2);
						edit2->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit2->build();
						edit2->initialize(this);
						edit2->setText(PKStr::int32ToWstring(PKVALUE_INT32RANGE(value).max));
					}
					break;

				case PK_VARIANT_INT64RANGE:
					{
						PKHLayout *layout = new PKHLayout();
						layout->setMargin(0);
						layout->setSpacing(2);
						layout->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						grid->addChild(layout);

						PKLabel *label1 = new PKLabel();
						layout->addChild(label1);
						label1->setText(L"Min:");
						label1->build();
						label1->initialize(this);

						PKEdit *edit1 = new PKEdit();
						PKVariantString id(name+":min");
						edit1->set(PKControl::ID_STRING_PROPERTY, &id);

						layout->addChild(edit1);
						PKVariantInt32 type1(PK_EDIT_TYPE_NUMBER);
						edit1->set(PKEdit::TYPE_INT32_PROPERTY, &type1);
						PKVariantRect frame1(0,0,30,20);
						edit1->set(PKControl::FRAME_RECT_PROPERTY, &frame1);
						edit1->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit1->build();
						edit1->initialize(this);
						edit1->setText(PKStr::int64ToWstring(PKVALUE_INT64RANGE(value).min));

						PKLabel *label2 = new PKLabel();
						layout->addChild(label2);
						label2->setText(L"Max:");
						label2->build();
						label2->initialize(this);

						PKEdit *edit2 = new PKEdit();
						PKVariantString id2(name+":max");
						edit2->set(PKControl::ID_STRING_PROPERTY, &id2);

						layout->addChild(edit2);
						PKVariantInt32 type2(PK_EDIT_TYPE_NUMBER);
						edit2->set(PKEdit::TYPE_INT32_PROPERTY, &type2);
						PKVariantRect frame2(0,0,30,20);
						edit2->set(PKControl::FRAME_RECT_PROPERTY, &frame2);
						edit2->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit2->build();
						edit2->initialize(this);
						edit2->setText(PKStr::int64ToWstring(PKVALUE_INT64RANGE(value).max));
					}
					break;

				case PK_VARIANT_DOUBLERANGE:
					{
						PKHLayout *layout = new PKHLayout();
						layout->setMargin(0);
						layout->setSpacing(2);
						layout->setLayoutPolicy(PK_LAYOUT_MAX, PK_LAYOUT_MIN);
						grid->addChild(layout);

						PKLabel *label1 = new PKLabel();
						layout->addChild(label1);
						label1->setText(L"Min:");
						label1->build();
						label1->initialize(this);

						PKEdit *edit1 = new PKEdit();
						PKVariantString id(name+":min");
						edit1->set(PKControl::ID_STRING_PROPERTY, &id);

						layout->addChild(edit1);
						PKVariantInt32 type1(PK_EDIT_TYPE_NUMBER);
						edit1->set(PKEdit::TYPE_INT32_PROPERTY, &type1);
						PKVariantRect frame1(0,0,30,20);
						edit1->set(PKControl::FRAME_RECT_PROPERTY, &frame1);
						edit1->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit1->build();
						edit1->initialize(this);
						edit1->setText(PKStr::doubleToWstring(PKVALUE_DOUBLERANGE(value).min));

						PKLabel *label2 = new PKLabel();
						layout->addChild(label2);
						label2->setText(L"Max:");
						label2->build();
						label2->initialize(this);

						PKEdit *edit2 = new PKEdit();
						PKVariantString id2(name+":max");
						edit2->set(PKControl::ID_STRING_PROPERTY, &id2);

						layout->addChild(edit2);
						PKVariantInt32 type2(PK_EDIT_TYPE_NUMBER);
						edit2->set(PKEdit::TYPE_INT32_PROPERTY, &type2);
						PKVariantRect frame2(0,0,30,20);
						edit2->set(PKControl::FRAME_RECT_PROPERTY, &frame2);
						edit2->setLayoutPolicy(PK_LAYOUT_FIXED, PK_LAYOUT_MIN);
						edit2->build();
						edit2->initialize(this);
						edit2->setText(PKStr::doubleToWstring(PKVALUE_DOUBLERANGE(value).max));
					}
					break;

				default:
					{
						PKLabel *label = new PKLabel();
						grid->addChild(label);
						label->setText(L"Not Supported");
						label->build();
						label->initialize(this);
						label->disable();
					}
					break;
			}
		}

		this->resizeToFit();
		this->disableControl("ID_APPLY");
	}

	this->updating = false;
}

void PKPropertiesWindow::comboBoxChanged(std::string id, int32_t previousValue)
{
	if(this->updating)
	{
		return;
	}

	this->enableControl("ID_APPLY");
	this->updateList.push_back(id);
}

void PKPropertiesWindow::editTextChanged(std::string id, 
										 std::wstring text)
{
	if(this->updating)
	{
		return;
	}

	std::vector<std::string> results;
	PKStr::explode(id, ':', &results);

	if(results.size() == 0)
	{
		return;
	}

	this->enableControl("ID_APPLY");
	this->updateList.push_back(results[0]);
}

void PKPropertiesWindow::buttonClicked(std::string id)
{
	if(id == "ID_APPLY")
	{
		// Disable the button
		this->disableControl("ID_APPLY");

		// Update the object
		this->applyUpdates();
	}

	if(PKStr::findFirstOf(id, "BUTTON_") == 0)
	{
		std::string propID = id.substr(7);
		
		PKOpenDialog open;

    	open.setTitle(L"Please chodose a PNG file to open:");
	    open.addFilterEntry(L"*.png", L"PNG Files");

		std::wstring result = open.askForOpenFilename(this->getWindowHandle());

		if(result != L"")
		{
			std::wstring prop = L"file|" + result;
			PKVariantWString img(prop);
			this->currentObject->set(propID, &img);
            this->setEditText(propID, prop);

			if(this->previewWindow)
			{
				this->previewWindow->resizeToFit();
			}
		}
	}
}

void PKPropertiesWindow::applyUpdates()
{
	if(this->currentObject == NULL)
	{
		return;
	}

	for(uint32_t i=0; i<this->updateList.size(); i++)
	{
		std::string id = this->updateList[i];
		PKVariant *value = this->currentObject->get(id);

		if(value)
		{
			switch(value->_type)
			{
			case PK_VARIANT_NULL:
			case PK_VARIANT_VOIDP:
				break;
				
			case PK_VARIANT_BOOL:
				{
					int result = this->getComboBoxSelectedItem(id);

					if(result == 0)
					{
						PKVariantBool b(true);
						this->currentObject->set(id, &b);
					}
					else
					{
						PKVariantBool b(false);
						this->currentObject->set(id, &b);
					}
				}
				break;

			case PK_VARIANT_INT64:
				{
					std::wstring val = this->getEditText(id);
					int64_t dval = PKStr::wstringToInt64(val);
					PKVariantInt64 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_INT32:
				{
					std::wstring val = this->getEditText(id);
					int32_t dval = PKStr::wstringToInt32(val);
					PKVariantInt32 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_INT16:
				{
					std::wstring val = this->getEditText(id);
					int16_t dval = (int16_t) PKStr::wstringToInt32(val);
					PKVariantInt16 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_INT8:
				{
					std::wstring val = this->getEditText(id);
					int8_t dval = (int8_t) PKStr::wstringToInt32(val);
					PKVariantInt8 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_UINT64:
				{
					std::wstring val = this->getEditText(id);
					uint64_t dval = PKStr::wstringToUint64(val);
					PKVariantUInt64 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_UINT32:
				{
					std::wstring val = this->getEditText(id);
					uint32_t dval = PKStr::wstringToUInt32(val);
					PKVariantUInt32 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_UINT16:
				{
					std::wstring val = this->getEditText(id);
					uint16_t dval = (uint16_t) PKStr::wstringToUInt32(val);
					PKVariantUInt16 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_UINT8:
				{
					std::wstring val = this->getEditText(id);
					uint8_t dval = (uint8_t) PKStr::wstringToUInt32(val);
					PKVariantUInt8 v(dval);
					this->currentObject->set(id, &v);
				}
				break;

			case PK_VARIANT_DOUBLE:
				{
					std::wstring val = this->getEditText(id);
					double dval = PKStr::wstringToDouble(val);
					PKVariantDouble v(dval);
					this->currentObject->set(id, &v);
				}
				break;
				
			case PK_VARIANT_STRING:
				{
					std::wstring val = this->getEditText(id);
					PKVariantString str(PKStr::wStringToString(val));
					this->currentObject->set(id, &str);
				}
				break;

			case PK_VARIANT_WSTRING:
				{
					std::wstring val = this->getEditText(id);
					PKVariantWString str(val);
					this->currentObject->set(id, &str);
				}
				break;

			case PK_VARIANT_POINT:
				{
					std::wstring valx = this->getEditText(id + ":x");
					std::wstring valy = this->getEditText(id + ":y");
					PKVariantPoint pt(PKStr::wstringToInt32(valx),
									  PKStr::wstringToInt32(valy));
					this->currentObject->set(id, &pt);
				}
				break;

			case PK_VARIANT_RECT:
				{
					std::wstring valx = this->getEditText(id + ":x");
					std::wstring valy = this->getEditText(id + ":y");
					std::wstring valw = this->getEditText(id + ":w");
					std::wstring valh = this->getEditText(id + ":h");

					PKVariantRect rc(PKStr::wstringToInt32(valx),
									 PKStr::wstringToInt32(valy),
									 PKStr::wstringToUInt32(valw),
									 PKStr::wstringToUInt32(valh));
					this->currentObject->set(id, &rc);
				}
				break;

			case PK_VARIANT_COLOR:
				{
					std::wstring valr = this->getEditText(id + ":r");
					std::wstring valg = this->getEditText(id + ":g");
					std::wstring valb = this->getEditText(id + ":b");
					std::wstring vala = this->getEditText(id + ":a");

					PKVariantColor cl((uint8_t) PKStr::wstringToUInt32(valr),
									  (uint8_t) PKStr::wstringToUInt32(valg),
									  (uint8_t) PKStr::wstringToUInt32(valb),
									  (uint8_t) PKStr::wstringToUInt32(vala));
					this->currentObject->set(id, &cl);
				}
				break;

			case PK_VARIANT_STRINGLIST:
				{
					std::wstring val = this->getEditText(id);
					std::vector<std::string> strings;
					PKStr::explode(PKStr::wStringToString(val), ',', &strings);
					PKVariantStringList str(strings);
					this->currentObject->set(id, &str);
				}
				break;

			case PK_VARIANT_WSTRINGLIST:
				{
					std::wstring val = this->getEditText(id);
					std::vector<std::wstring> strings;
					PKStr::explode(val, L',', &strings);
					PKVariantWStringList str(strings);
					this->currentObject->set(id, &str);
				}
				break;
			    
			case PK_VARIANT_INT32RANGE:
				{
					std::wstring valmin = this->getEditText(id + ":min");
					std::wstring valmax = this->getEditText(id + ":max");
					PKVariantInt32Range rg(PKStr::wstringToInt32(valmin),
									       PKStr::wstringToInt32(valmax));
					this->currentObject->set(id, &rg);
				}
				break;

			case PK_VARIANT_INT64RANGE:
				{
					std::wstring valmin = this->getEditText(id + ":min");
					std::wstring valmax = this->getEditText(id + ":max");
					PKVariantInt64Range rg(PKStr::wstringToInt64(valmin),
									       PKStr::wstringToInt64(valmax));
					this->currentObject->set(id, &rg);
				}
				break;

			case PK_VARIANT_DOUBLERANGE:
				{
					std::wstring valmin = this->getEditText(id + ":min");
					std::wstring valmax = this->getEditText(id + ":max");
					PKVariantDoubleRange rg(PKStr::wstringToDouble(valmin),
									        PKStr::wstringToDouble(valmax));
					this->currentObject->set(id, &rg);
				}
				break;
			}
		}
	}

	if(this->previewWindow)
	{
		this->previewWindow->resizeToFit();
	}
}
