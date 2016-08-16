//
// main.cpp for uiBuilder
//

#include "PKDialog.h"
#include "PKStr.h"
#include "PKApp.h"
#include "PKMenu.h"
#include "PKToolbar.h"
#include "PKScreenTool.h"
#include "PKLabel.h"
#include "PKTreeView.h"
#include "PKPopupMenu.h"
#include "PKPropertiesWindow.h"
#include "PKSaveDialog.h"
#include "PKOpenDialog.h"
#include "PKFile.h"
#include "PKPath.h"
#include "PKAlertDialog.h"
#include "PKPersistTool.h"
#include "PKMusicUIFactory.h"

// Factories

#include "PKUIFactory.h"
#include "PKMusicUIFactory.h"

PKUIFactory      uiFactory;
PKMusicUIFactory musicUIFactory;
static PKApp *app;

// Misc

#include <algorithm>

//
// Variables
//

class UIBuilderMainWindow;
UIBuilderMainWindow *mainWindow;
PKMenu *mainMenu;
PKToolbar *mainToolbar;
PKDialog  *previewWindow;
std::wstring fileName;
std::wstring fullPath;
PKPopupMenu *itemMenu;
PKPopupMenu *addSubMenu;
PKPropertiesWindow *propWindow;
std::vector<std::string> controls;
int32_t lastId = 0;
std::vector<IPKObjectFactory*> theFactories;

#define ID_POPUP_DELETE    0x01
#define ID_POPUP_ADD       0x02
#define ID_POPUP_ADD_START 0x03

//
// Utils
//

void PKTreeViewXMLWalk(PKObject *child, void *context)
{
	PKTreeView *tree = (PKTreeView*) context;
	PKControl  *ctrl = dynamic_cast<PKControl*>(child);

	if(tree && child)
	{
		std::string id      = PKVALUE_STRING(child->get(PKControl::ID_STRING_PROPERTY));
		std::string classID = child->getClassName();

		std::wstring text = PKStr::stringToWString(classID) + L" - " + 
							PKStr::stringToWString(id);

		std::string parentID = "";

		PKObject *parent = child->getParent();

		if(parent)
		{
			PKControl *ctrl = dynamic_cast<PKControl*>(parent);

			if(ctrl)
			{
				parentID = PKVALUE_STRING(ctrl->get(PKControl::ID_STRING_PROPERTY));
			}
		}

		tree->addItem(text, id, parentID);
	}
}

bool PKtoXMLPropertyHook(PKObject *object, std::string propertyName)
{
	if(propertyName == "frame")
	{
		if(PKVALUE_INT32(object->get(PKControl::LAYOUTX_INT32_PROPERTY)) == PK_LAYOUT_FIXED ||
		   PKVALUE_INT32(object->get(PKControl::LAYOUTY_INT32_PROPERTY)) == PK_LAYOUT_FIXED)
		{
			return true;
		}

		return false;
	}

	if(propertyName == "type" &&
	  (object->getClassName() == "PKVLayout" ||
       object->getClassName() == "PKHLayout"))
	{
		return false;
	}

	return true;
}

//
// UIBuilderMainWindow
//

class UIBuilderMainWindow : public PKDialog, public PKPopupMenuCallback
{
public:

	UIBuilderMainWindow(std::wstring id, 
						PKWindowStyle style,
						PKWindowHandle parent,
						std::wstring icon)

	: PKDialog(id, style, parent, icon)
	{
	}

    void initUIElements()
    {
    }

	void newDocument()
	{
		if(previewWindow != NULL)
		{	
			// TODO: propose to close current document ?
			return;
		}

		// Create first XML

		std::string xml = "<PKVLayout id=\"ID_ROOT\" layoutx=\"2\" layouty=\"2\"></PKVLayout>";
		fileName = L"";
		fullPath = L"";

		// Create preview window from it

		previewWindow = new PKDialog(L"PREVIEW_WINDOW", 
									 PK_WINDOW_STYLE_DEFAULT,
									 NULL, L"ICON");

		PKVariantWString title(L"Preview - Untitled");
		previewWindow->set(PKWindow::CAPTION_WSTRING_PROPERTY, &title);

		previewWindow->loadFromXML(xml);

		previewWindow->resizeToFit();
		previewWindow->centerOnScreen();
		previewWindow->moveWindowTo(previewWindow->getWindowX() + 150,
									previewWindow->getWindowY());

		previewWindow->show();

		// Update UI elements

		PKLabel *label = dynamic_cast<PKLabel*>(this->findControl("ID_LABEL"));

		if(label)
		{
			label->setText(L"Current Window: Untitled");
			label->enable();
		}

		// Update treeview

		PKTreeView *tree = dynamic_cast<PKTreeView*>(this->findControl("ID_TREEVIEW"));

		if(tree)
		{
			PKObject *root = previewWindow->getChildAt(0);
			
			if(root)
			{
				tree->clearItems();
				root->walkChildren(PKTreeViewXMLWalk, (void*) tree);
				tree->expandAll();
                tree->setSelectedItem("ID_ROOT");
			}
		}

		// Create and show properties window
	
		propWindow = new PKPropertiesWindow();

		propWindow->setPreviewWindow(previewWindow);
        propWindow->setObject(previewWindow->getChildAt(0));

		propWindow->resizeToFit();
	
		if(propWindow->getWindowWidth() < 200 ||
		   propWindow->getWindowHeight() < 100)
		{
			propWindow->resizeWindowAreaTo(200, 350);
		}

		propWindow->moveWindowTo(mainWindow->getWindowX() + 
								 mainWindow->getWindowWidth() + 10, 
								 mainWindow->getWindowY());

		propWindow->show();
	}

    void saveDocument()
    {
        if(previewWindow == NULL)
        {
            // No doc to save
            return;
        }

		if(propWindow)
		{
			propWindow->applyUpdates();
		}

		std::wstring result = L"";

        if(fileName == L"")
        {
            // Need to ask for destination first
			PKSaveDialog save;

			save.setTitle(L"Please choose a destination file:");
			save.setInitialFilename(L"newWindow.h");
			save.addFilterEntry(L"*.h", L"C++ Header Files");
			
			result = save.askForSaveFilename(mainWindow->getWindowHandle());

			if(result == L"")
			{
				// Cancelled, return
				return;
			}

			// Setup filename and filepath

			if(result.substr(result.size() - 2) != L".h")
			{
				result.append(L".h");
			}

			fileName = PKPath::getFileNameAndExtension(result);
			fullPath = result;
        }

        // Save it !!

		PKObject *root = previewWindow->getChildAt(0);
		
		if(root)
		{
			std::string xml = root->toXMLWithPropertyHook(PKtoXMLPropertyHook);

			// NOTE: this only works on WIN32 because of the '\r's

			xml = PKStr::replaceString("\"", "\\\"", xml);
			xml = PKStr::replaceString("\r\n", "\"\r\n\"", xml);
			xml = xml.substr(0, xml.size()-3);

			std::string name = PKStr::wStringToString(fileName.substr(0, fileName.size()-2));

			xml = "#ifndef " + name + "_XML_H\r\n" +
				  "#define " + name + "_XML_H\r\n\r\n" +
				  "//\r\n// "+ name + "XML\r\n//\r\n\r\nstd::string " + 
				  name + "XML = \r\n\"" + xml + ";\r\n\r\n" +
				  "#endif // " + name + "_XML_H\r\n";

			PKFile::saveStringToFile(xml, fullPath); 

			// Update UI

			PKLabel *label = dynamic_cast<PKLabel*>(this->findControl("ID_LABEL"));

			if(label)
			{
				label->setText(L"Current Window: " + fileName);
				label->enable();
			}

			PKVariantWString title(L"Preview - " + fileName);
			previewWindow->set(PKWindow::CAPTION_WSTRING_PROPERTY, &title);
		}
    }

    void closeDocument()
    {
		this->saveDocument();

		fileName = L"";
		fullPath = L"";

		if(previewWindow)
		{
			previewWindow->hide();
			previewWindow->release();
			previewWindow = NULL;
		}

		if(propWindow)
		{
			propWindow->hide();
			propWindow->release();
			propWindow = NULL;
		}

		// Update UI

		PKLabel *label = dynamic_cast<PKLabel*>(this->findControl("ID_LABEL"));

		if(label)
		{
			label->setText(L"Current Window: None");
			label->enable();
		}

		PKTreeView *tree = dynamic_cast<PKTreeView*>(this->findControl("ID_TREEVIEW"));

		if(tree)
		{
			tree->clearItems();
		}
	}

    void openDocument(std::wstring path)
    {
		if(previewWindow)
		{
			// TODO: offer to close current document
			
			return;
			//this->closeDocument();
		}

		// Fire up open dialog

        std::wstring result = path;

        if(result == L"")
        {
    		PKOpenDialog open;

	    	open.setTitle(L"Please choose a Header file to open:");
		    open.addFilterEntry(L"*.h", L"C++ Header Files");

		    result = open.askForOpenFilename(mainWindow->getWindowHandle());
        }

		if(result != L"")
		{
			fullPath = result;
			fileName = PKPath::getFileNameAndExtension(result);

			std::string contents = PKFile::readFile(result);

			int offset = contents.find_first_of("\"");
			contents   = contents.substr(offset+1);

			offset   = contents.find_last_of("\"");
			contents = contents.substr(0, offset);

			contents = PKStr::replaceString("\\\"", "[[ç]]", contents);
			contents = PKStr::replaceString("\"", "", contents);
			contents = PKStr::replaceString("[[ç]]", "\"", contents);

			// Create preview window from it

			previewWindow = new PKDialog(L"PREVIEW_WINDOW", 
										 PK_WINDOW_STYLE_DEFAULT,
										 NULL, L"ICON");

			PKVariantWString title(L"Preview - " + fileName);
			previewWindow->set(PKWindow::CAPTION_WSTRING_PROPERTY, &title);

			previewWindow->loadFromXML(contents);

			previewWindow->resizeToFit();
			previewWindow->centerOnScreen();
			previewWindow->moveWindowTo(previewWindow->getWindowX() + 150,
										previewWindow->getWindowY());

			previewWindow->show();

			// Update UI elements

			PKLabel *label = dynamic_cast<PKLabel*>(this->findControl("ID_LABEL"));

			if(label)
			{
				label->setText(L"Current Window: " + fileName);
				label->enable();
			}

			// Update treeview

			PKTreeView *tree = dynamic_cast<PKTreeView*>(this->findControl("ID_TREEVIEW"));

			if(tree)
			{
				PKObject *root = previewWindow->getChildAt(0);
				
				if(root)
				{
					tree->clearItems();
					root->walkChildren(PKTreeViewXMLWalk, (void*) tree);
					tree->expandAll();
					tree->setSelectedItem("ID_ROOT");
				}
			}

			// Create and show properties window
		
			propWindow = new PKPropertiesWindow();

			propWindow->setPreviewWindow(previewWindow);
			propWindow->setObject(previewWindow->getChildAt(0));

			propWindow->resizeToFit();
		
			if(propWindow->getWindowWidth() < 200 ||
			   propWindow->getWindowHeight() < 100)
			{
				propWindow->resizeWindowAreaTo(200, 350);
			}

			propWindow->moveWindowTo(mainWindow->getWindowX() + 
									 mainWindow->getWindowWidth() + 10, 
									 mainWindow->getWindowY());

			propWindow->show();
		}
    }

    void treeViewItemSelected(std::string id)
	{
		// Need to find the object and bind it
		// to the properties window, for edition

		PKControl *ctrl = previewWindow->findControl(id);

		if(ctrl && propWindow)
		{
			propWindow->hide();
			propWindow->setObject(ctrl);
			propWindow->show();
		}
	}

	void treeViewItemRightClicked(std::string id, int32_t x, int32_t y)
	{
        // Select the item

		PKTreeView *tree = dynamic_cast<PKTreeView*>(this->findControl("ID_TREEVIEW"));

		if(tree)
		{
            tree->setSelectedItem(id);
        }

		// Setup popup menu

		itemMenu->enableItem(ID_POPUP_DELETE, true);

		PKControl *ctrl = previewWindow->findControl(id);
		PKControl *root = dynamic_cast<PKControl*>(previewWindow->getChildAt(0));

		if(ctrl != NULL)
		{
			if(root == ctrl)
			{
				itemMenu->enableItem(ID_POPUP_DELETE, false);
			}
		}

		// Show popup menu
		itemMenu->popup(this->getWindowHandle());
	}

	// This is for the popup menu
	void menuCallback(unsigned int id)
	{
		if(id == ID_POPUP_DELETE)
		{
			// Get handle to tree view

			PKTreeView *tree = dynamic_cast<PKTreeView*>(this->findControl("ID_TREEVIEW"));

			if(tree)
			{
				// Get selection from treeview
				std::string id = tree->getSelectedItemId();

				if(id != "")
				{
					PKControl *control = previewWindow->findControl(id);

					if(control)
					{
						PKControl *parent = dynamic_cast<PKControl*>(control->getParent());

						if(parent)
						{
							parent->removeChild(control);
							control->hide();
							control->release();

							propWindow->setObject(NULL);
							propWindow->resizeWindowAreaTo(200, 350);

							PKObject *root = previewWindow->getChildAt(0);
							
							if(root)
							{
								tree->clearItems();
								root->walkChildren(PKTreeViewXMLWalk, (void*) tree);
								tree->expandAll();
							}

							previewWindow->resizeToFit();
						}
					}
				}
			}
		}

		if(id >= ID_POPUP_ADD_START)
		{
			// Add an item from the right factory

			uint32_t index = (id - ID_POPUP_ADD_START);
			std::string strID = controls[index];
			strID = "PK" + strID;

			PKControl *object = NULL;

			for(uint32_t i=0; i < theFactories.size(); i++)
			{
				object = (PKControl *) theFactories[i]->createObject(strID);

				if(object != NULL)
				{
					break;
				}
			}

			if(object)
			{
				// Get parent object

				PKTreeView *tree = dynamic_cast<PKTreeView*>(this->findControl("ID_TREEVIEW"));

				if(tree)
				{
					// Get selection from treeview
					std::string sid = tree->getSelectedItemId();

					if(sid != "")
					{
						PKControl *control = previewWindow->findControl(sid);

						if(control)
						{
							// Add control to window
							control->addChild(object);

							PKVariantString oid("ID_OBJ_" + PKStr::uint32ToString(lastId++));
							object->set(PKControl::ID_STRING_PROPERTY, &oid);

							object->build();
							object->initialize(previewWindow);
							previewWindow->resizeToFit();

							// Refresh tree view

							PKObject *root = previewWindow->getChildAt(0);
							
							if(root)
							{
								tree->clearItems();
								root->walkChildren(PKTreeViewXMLWalk, (void*) tree);
								tree->expandAll();

								tree->setSelectedItem(PKVALUE_STRING(&oid));
							}
						}
					}
				}
			}
		}
	}

	void toolbarItemClicked(PKToolbar *fromToolbar, PKToolbarItem *item, std::string id)
	{
		if(id == "TOOLBAR_ID_NEW")
		{
			this->newDocument();
		}

		if(id == "TOOLBAR_ID_SAVE")
		{
            this->saveDocument();
		}

        if(id == "TOOLBAR_ID_OPEN")
        {
            this->openDocument(L"");
        }

        if(id == "TOOLBAR_ID_SAVE_AND_RELOAD")
        {

            this->saveDocument();
            std::wstring path = fullPath;
            this->closeDocument();
            this->openDocument(path);
        }
	}

	void menuItemClicked(PKMenu *fromMenu, PKMenuItem *item, std::string id)
	{
		if(id == "MENU_ID_NEW")
		{
			this->newDocument();
		}

        if(id == "MENU_ID_CLOSE")
        {
            this->closeDocument();
        }

        if(id == "MENU_ID_OPEN")
        {
            this->openDocument(L"");
        }

        if(id == "MENU_ID_SAVE")
        {
            this->saveDocument();
        }

		if(id == "MENU_ID_QUIT")
		{
			// TODO: Offer to save or not
			this->closeDocument();
			this->hide();
	        app->quit();
		}
	}

    void menuCancelled()
    {
    }

	void windowClosed()
	{
		// TODO: Offer to save or not
		this->closeDocument();
		this->hide();
        app->quit();
	}
	
private:
};

//
// PKApp delegates
//

int32_t PKApp::appStartupDelegate(std::wstring commandLine)
{
	// 
	// Setup factories
	//

	theFactories.push_back(&uiFactory);
	theFactories.push_back(&musicUIFactory);

	//
	// Read settings
	//

	PKPersistTool p("uiBuilder");
	p.getInt32("lastId", &lastId);

    //
	// Create the Window
	//
	
	mainWindow = new UIBuilderMainWindow(L"UIBUILDER_MAIN_WINDOW", 
	    								 PK_WINDOW_STYLE_DEFAULT | 
										 PK_WINDOW_STYLE_NO_MAX,
										 NULL, L"ICON");

	PKVariantWStringI18N caption(PK_i18n(L"ProjectKetchup uiBuilder"));
	mainWindow->set(PKWindow::CAPTION_WSTRING_PROPERTY, &caption);

	// Load the window

    std::string xml =   "<PKVLayout layoutx=\"2\" layouty=\"2\" spacing=\"5\" margin=\"5\">"
						"  <PKLabel id=\"ID_LABEL\" text=\"Current Window: None\" enabled=\"false\" />"
						"  <PKTreeView id=\"ID_TREEVIEW\" layoutx=\"2\" layouty=\"2\"/>"
                        "</PKVLayout>";
	
	mainWindow->loadFromXML(xml);
    mainWindow->initUIElements();

	// Add menu

    std::string menuXML = "<PKMenu>"
                          "  <PKMenuItem text=\"PKI18N:File\" id=\"MENU_ID_FILE\">"
                          "    <PKMenu>"
                          "      <PKMenuItem text=\"PKI18N:New Dialog...\" shortcut=\"Ctrl+N\" id=\"MENU_ID_NEW\" />"
                          "      <PKSeparator/>"
                          "      <PKMenuItem text=\"PKI18N:Open Dialog...\" shortcut=\"Ctrl+O\" id=\"MENU_ID_OPEN\" />"
                          "      <PKMenuItem text=\"PKI18N:Close Dialog\" id=\"MENU_ID_CLOSE\" />"
                          "      <PKSeparator/>"
						  "      <PKMenuItem text=\"PKI18N:Save Dialog\" shortcut=\"Ctrl+S\" id=\"MENU_ID_SAVE\" />"
                          "      <PKSeparator/>"
						  "      <PKMenuItem text=\"PKI18N:Quit\" shortcut=\"Alt+F4\" id=\"MENU_ID_QUIT\" />"
                          "    </PKMenu>"
                          "  </PKMenuItem>"
                          "  <PKMenuItem text=\"PKI18N:Help\" right=\"true\" id=\"MENU_ID_HELP\">"
                          "    <PKMenu>"
                          "      <PKMenuItem text=\"PKI18N:About...\" id=\"MENU_ID_ABOUT\" />"
                          "    </PKMenu>"
                          "  </PKMenuItem>"
                          "</PKMenu>";

    mainMenu = PKMenu::loadMenuFromXML(menuXML);
	app->installRootMenu(mainWindow, mainMenu);

	// Add toolbar

	std::string toolbarXML = "<PKToolbar id=\"com.projectKetchup.uiBuilder\">"
							 "  <PKToolbarItem id=\"TOOLBAR_ID_NEW\" text=\"PKI18N:New\" icon=\"res|TB_NEW\" />"
						     "  <PKToolbarSeparator />"
							 "  <PKToolbarItem id=\"TOOLBAR_ID_OPEN\" text=\"PKI18N:Open\" icon=\"res|TB_OPEN\" />"
							 "  <PKToolbarItem id=\"TOOLBAR_ID_SAVE\" text=\"PKI18N:Save\" icon=\"res|TB_SAVE\" />"
						     "  <PKToolbarFlexibleSpace />"
							 "  <PKToolbarItem id=\"TOOLBAR_ID_SAVE_AND_RELOAD\" text=\"PKI18N:Save & Reload\" icon=\"res|TB_ABOUT\" />"
							 "</PKToolbar>";

	PKToolbar::PK_TOOLBAR_HEIGHT = 48;
	
	mainToolbar = PKToolbar::loadToolbarFromXML(toolbarXML);
	mainWindow->setToolbar(mainToolbar);

	// Create popup menus

	addSubMenu = new PKPopupMenu();

	for(uint32_t i=0; i < theFactories.size(); i++)
	{
		std::vector<std::string> newObjects = theFactories[i]->getAvailableObjectsList();

		for(uint32_t n=0; n < newObjects.size(); n++)
		{
			newObjects[n] = newObjects[n].substr(2);
		}

		// Sort the array

		std::sort(newObjects.begin(), newObjects.end());

		// Add it

		for(uint32_t n=0; n < newObjects.size(); n++)
		{
			controls.push_back(newObjects[n]);
		}

		if(i != theFactories.size() - 1)
		{
			controls.push_back("-");
		}
	}

	// Remove "PK" from object names


	// Create the menu

	for(uint32_t i=0; i < controls.size(); i++)
	{
		if(controls[i] != "-")
		{
			addSubMenu->insertItem(ID_POPUP_ADD_START+i,
								   PKStr::stringToWString(controls[i]));
		}
		else
		{
			addSubMenu->insertSeparator();
		}
	}

	itemMenu = new PKPopupMenu();
	itemMenu->setMenuCallback(mainWindow);

	itemMenu->insertItem(ID_POPUP_DELETE, L"Delete");
	itemMenu->insertSeparator();
	itemMenu->insertSubmenu(ID_POPUP_ADD, L"Add...", addSubMenu);

	// Show our window

	PKScreenTool sTool;
	std::vector<PKRect> screens = sTool.getScreens();

	if(screens.size())
	{
		mainWindow->resizeClientAreaTo(280, screens[0].h - 100);
		mainWindow->moveWindowTo(screens[0].x + 5, screens[0].y + 5);
	}

	xml = "";
	fileName = L"";
	fullPath = L"";
	previewWindow = NULL;
	propWindow = NULL;

	mainWindow->show();

    return 0; 
}

int32_t PKApp::appShutdownDelegate()
{
    mainWindow->release();

	PKPersistTool p("uiBuilder");
	p.setInt32("lastId", lastId);

    return 0;
}
