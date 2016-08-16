//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKToolbar.h"
#include "PKObjectLoader.h"
#include "PKUIFactory.h"
#include "PKStr.h"
#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKWindow.h"
#include "PKDialog.h"
#include "PKBlitter.h"
#include "PKBitmapOps.h"
#include "PKFont.h"
#include "PKOS.h"

#include "PKToolbarItem.h"
#include "PKToolbarSegmentedItem.h"
#include "PKToolbarSeparator.h"
#include "PKToolbarFixedSpace.h"
#include "PKToolbarFlexibleSpace.h"

//
// static
//

int PKToolbar::lastId = 20000;
int PKToolbar::PK_TOOLBAR_HEIGHT = 60;

#ifdef LINUX
#define abs(a) (a < 0 ? -a : a)
#endif

//
// PROPERTIES
//

std::string PKToolbar::ID_STRING_PROPERTY        = "id";
std::string PKToolbar::LIGHTCOLOR_COLOR_PROPERTY = "lightColor";
std::string PKToolbar::DARKCOLOR_COLOR_PROPERTY  = "darkColor";

//
// MACOSX Toolbar Delegate
//

#ifdef MACOSX

#include "PKMacSegmentedToolbarItem.h"

static OSStatus PKToolbarDelegate(EventHandlerCallRef inCallRef,
								  EventRef inEvent, 
								  void* inUserData )

{
    OSStatus            result = eventNotHandledErr;
    CFMutableArrayRef   array;
    CFStringRef         identifier;
	
	PKToolbar *tb = (PKToolbar *) inUserData;
	
	if(tb == NULL)
	{
		return result;
	}
	
    switch(GetEventKind(inEvent))
    {
        case kEventToolbarGetDefaultIdentifiers:
		{
            GetEventParameter( inEvent, kEventParamMutableArray,
							  typeCFMutableArrayRef, NULL,
							  sizeof( CFMutableArrayRef ), NULL, &array );

			// Walk through children and add to array
			
			uint32_t count = tb->getChildrenCount();
			
			for(uint32_t i=0; i < count; i++)
			{
				PKObject *child = tb->getChildAt(i);
				
				//
				// See which type of object it is
				//
				
				// Try standard item
				
				PKToolbarItem *item = dynamic_cast<PKToolbarItem *>(child);
				
				if(item != NULL)
				{
					PKVariant   *iid  = item->get(PKToolbarItem::ID_STRING_PROPERTY);
					std::string  itid = PKVALUE_STRING(iid);
					
					std::wstring witid  = PKStr::stringToWString(itid);
					CFStringRef  cfItId = PKStr::wStringToCFString(witid);
					
					CFArrayAppendValue(array, cfItId);
					CFRelease(cfItId);
				}
				
				// Try segmented item
				
				PKToolbarSegmentedItem *sitem = dynamic_cast<PKToolbarSegmentedItem *>(child);
				
				if(sitem != NULL)
				{
					PKVariant   *iid  = sitem->get(PKToolbarSegmentedItem::ID_STRING_PROPERTY);
					std::string  itid = PKVALUE_STRING(iid);
					
					std::wstring witid  = PKStr::stringToWString(itid);
					CFStringRef  cfItId = PKStr::wStringToCFString(witid);
					
					CFArrayAppendValue(array, cfItId);
					CFRelease(cfItId);
				}

				// Try Separator
				
				PKToolbarSeparator *sep = dynamic_cast<PKToolbarSeparator *>(child);
				
				if(sep != NULL)
				{
					CFArrayAppendValue(array, kHIToolbarSeparatorIdentifier);
				}

				// Try Fixed Space
				
				PKToolbarFixedSpace *ffs = dynamic_cast<PKToolbarFixedSpace *>(child);
				
				if(ffs != NULL)
				{
					CFArrayAppendValue(array, kHIToolbarSpaceIdentifier);
				}

				// Try Flexible Space
				
				PKToolbarFlexibleSpace *fs = dynamic_cast<PKToolbarFlexibleSpace *>(child);
				
				if(fs != NULL)
				{
					CFArrayAppendValue(array, kHIToolbarFlexibleSpaceIdentifier);
				}
			}
			
			
            result = noErr;
		}			
        break;
			
        case kEventToolbarCreateItemWithIdentifier:
		{
			HIToolbarItemRef        item = NULL;
			CFTypeRef               data = NULL;
			
			GetEventParameter(inEvent,
							  kEventParamToolbarItemIdentifier, typeCFStringRef,
							  NULL, sizeof( CFStringRef ), NULL, &identifier );
			
			GetEventParameter(inEvent,
							  kEventParamToolbarItemConfigData, typeCFTypeRef,
							  NULL, sizeof( CFTypeRef ), NULL, &data );
			
			
			uint32_t count = tb->getChildrenCount();
			
			for(uint32_t i=0; i < count; i++)
			{
				PKObject *child = tb->getChildAt(i);
				
				// Try standard item
				
				PKToolbarItem *pkitem = dynamic_cast<PKToolbarItem *>(child);
				
				if(pkitem != NULL)
				{
					PKVariant   *iid  = pkitem->get(PKToolbarItem::ID_STRING_PROPERTY);
					std::string  itid = PKVALUE_STRING(iid);
					
					std::wstring witid  = PKStr::stringToWString(itid);
					CFStringRef  cfItId = PKStr::wStringToCFString(witid);
					
					// Add a child item then
					
					if(CFStringCompare(cfItId, identifier, kCFCompareBackwards) == kCFCompareEqualTo)
					{
						OSStatus err = HIToolbarItemCreate(cfItId, kHIToolbarItemCantBeRemoved, &item);
					
						if(err == noErr)
						{
							// Set item properties
							
							PKToolbar::lastId++;
							
							HIToolbarItemSetCommandID(item, PKToolbar::lastId);
							pkitem->setId(PKToolbar::lastId);
						
							HIToolbarItemChangeAttributes(item, 
														  kHIToolbarItemCantBeRemoved,
														  kHIToolbarItemIsSeparator);
						
							PKVariant *t = pkitem->get(PKToolbarItem::TEXT_WSTRING_PROPERTY);
							std::wstring text = PKVALUE_WSTRING(t);
						
							CFStringRef cfLabelRef = PKStr::wStringToCFString(text);					
							HIToolbarItemSetLabel(item, cfLabelRef);
							CFRelease(cfLabelRef);
						
							PKVariant *h = pkitem->get(PKToolbarItem::HELP_WSTRING_PROPERTY);
							std::wstring help = PKVALUE_WSTRING(h);
						
							CFStringRef cfHelp = PKStr::wStringToCFString(help);					
							HIToolbarItemSetHelpText(item, cfHelp, NULL);
							CFRelease(cfHelp);
													
							PKVariant *img = pkitem->get(PKToolbarItem::ICON_WSTRING_PROPERTY);
							std::wstring imgURL = PKVALUE_WSTRING(img);
						
							PKImage *bitmap = NULL;
						
							PKImageURIType type;
							std::wstring   image;
						
							PKImage::processURI(imgURL, &type, &image);
						
							if(type == PKIMAGE_URI_RESOURCE)
							{
								bitmap = PKImageLoader::loadBitmapFromResource(image);
							}
						
							if(type == PKIMAGE_URI_FILENAME)
							{
								bitmap = PKImageLoader::loadBitmapFromFilename(image);
							}
						
							if(bitmap != NULL)
							{
								// Get a CGImageRef
							
								CGImageRef itemImage = bitmap->getCGImageRef();
							
								if(itemImage)
								{
									HIToolbarItemSetImage(item, itemImage);
								}
								
								tb->bitmaps.push_back(bitmap);
							}
							break;
						}
					}
				}
				
				// Try segmented item
				
				PKToolbarSegmentedItem *sitem = dynamic_cast<PKToolbarSegmentedItem *>(child);
				
				if(sitem != NULL)
				{
					PKVariant   *iid  = sitem->get(PKToolbarSegmentedItem::ID_STRING_PROPERTY);
					std::string  itid = PKVALUE_STRING(iid);
					
					std::wstring witid  = PKStr::stringToWString(itid);
					CFStringRef  cfItId = PKStr::wStringToCFString(witid);
					
					// Add a child item then
					
					if(CFStringCompare(cfItId, identifier, kCFCompareBackwards) == kCFCompareEqualTo)
					{
						sitem->loadImages();
					
						PKWindow *window = tb->getWindow();
						
						if(window)
						{
							PKDialog *dialog = dynamic_cast<PKDialog*>(window);
							
							if(dialog)
							{
								sitem->dialog = dialog;
							}
						}
						
						item = CreateSegmentedToolbarItem(sitem);
						break;
					}	
				}
					
			}

			if(item)
			{
				SetEventParameter( inEvent, kEventParamToolbarItem,
								  typeHIToolbarItemRef,
								  sizeof(HIToolbarItemRef ), &item );
				
				result = noErr;
			}
		}
        break;
    }
	
    return result;
}

//
// Mac Menu event handler
//

static OSStatus MacToolbarCmdHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
{
    UInt32   eclass;
    
	PKToolbar *toolbar = (PKToolbar *) inRefcon;
	
	if(toolbar == NULL)
	{
		return eventNotHandledErr;
	}
	
    eclass = GetEventClass(inEvent);
    
    if(eclass == kEventClassCommand) 
    {
		switch(GetEventKind(inEvent))
		{
			case kEventProcessCommand:
			{
				HICommand     hiCommand;
				
				GetEventParameter(inEvent,
								  kEventParamDirectObject,
								  typeHICommand,NULL,
								  sizeof(HICommand),NULL,
								  &hiCommand);

				uint32_t commandID = hiCommand.commandID;
				
				// Find the item with this ID
				
				uint32_t count = toolbar->getChildrenCount();
				
				for(uint32_t i=0; i < count; i++)
				{
					PKObject *child = toolbar->getChildAt(i);
					PKToolbarItem *item = dynamic_cast<PKToolbarItem*>(child);
					
					if(item)
					{
						if(commandID == item->getId())
						{
							// That's the one, let's fire a
							// callback with its string id
							
							PKVariant   *iid  = item->get(PKToolbarItem::ID_STRING_PROPERTY);
							std::string  sid = PKVALUE_STRING(iid);
							
							PKWindow *window = toolbar->getWindow();
							
							if(window)
							{
								PKDialog *dialog = dynamic_cast<PKDialog*>(window);
								
								if(dialog)
								{
									dialog->toolbarItemClicked(toolbar, item, sid);
								}
							}
							
							return eventNotHandledErr;
						}
					}
				}				
			}
			break;
		}
	}		
	
	return eventNotHandledErr;
}

#endif

//
// PKToolbar
//

PKToolbar::PKToolbar()
{
    PKColor color1 = PKColorCreate(248, 248, 248, 255);
    PKColor color2 = PKColorCreate(218, 218, 218, 255);

#ifdef WIN32

	DWORD c1 = GetSysColor(COLOR_3DHIGHLIGHT);
	DWORD c2 = GetSysColor(COLOR_3DFACE);

	color1.r = GetRValue(c1);
	color1.g = GetGValue(c1);
	color1.b = GetBValue(c1);

	color2.r = GetRValue(c2)-20;
	color2.g = GetGValue(c2)-20;
	color2.b = GetBValue(c2)-20;

#endif // WIN32

	PKOBJECT_ADD_STRING_PROPERTY(id, "");
    PKOBJECT_ADD_COLOR_PROPERTY(lightColor, color1);
    PKOBJECT_ADD_COLOR_PROPERTY(darkColor,  color2);
	
	this->built = false;
	this->window = NULL;

#if(defined(WIN32))
    
    this->font = new PKFont();

    if(this->font != NULL)
    {
        this->font->setColor(0 , 0, 0);
    }

    this->shadowFont = new PKFont();

    if(this->shadowFont != NULL)
    {
        shadowFont->setColor(235, 235, 235);
    }

    this->overIndex = -1;
    this->downIndex = -1;

    this->overSubIndex = -1;
    this->downSubIndex = -1;

#endif
	
#ifdef MACOSX
	
	this->toolbar = NULL;
	this->handler = NULL;
	this->commandHandler = NULL;

	EventTypeSpec kCmdEvents[ ]= { {kEventClassCommand, kEventProcessCommand} };
	
	InstallApplicationEventHandler(MacToolbarCmdHandler, 
								   GetEventTypeCount(kCmdEvents), 
								   kCmdEvents, this, &this->commandHandler);

#endif
}

PKToolbar::~PKToolbar()
{
#if(defined(WIN32))

    this->deleteLayout();

    if(this->font != NULL)
    {
        delete this->font;
    }

    if(this->shadowFont != NULL)
    {
        delete this->shadowFont;
    }

#endif

#ifdef MACOSX
	
	if(this->handler != NULL)
	{
		RemoveEventHandler(this->handler);
	}	
	
	if(this->commandHandler)
	{
		RemoveEventHandler(this->commandHandler);
	}
	
	if(this->toolbar != NULL)
	{
		CFRelease(this->toolbar);
	}
	
	for(unsigned int i=0; i<this->bitmaps.size(); i++)
	{
		delete this->bitmaps[i];
	}
	
	this->bitmaps.clear();
	
#endif

	for(unsigned int i=0; i < this->getChildrenCount(); i++)
	{
		PKObject *child = this->getChildAt(i);

		if(child)
		{
			child->release();
		}
	}
}

// Helper to load from XML
	
PKToolbar *PKToolbar::loadToolbarFromXML(std::string xml)
{
	PKUIFactory factory;
	PKObjectLoader loader;
	
	loader.addObjectFactory(&factory);
	
	PKObject *toolbar = loader.loadFromXML(xml);
	
    if(toolbar)
    {
        PKToolbar *tb = dynamic_cast<PKToolbar*>(toolbar);
		
        if(tb)
        {
            return tb;
        }
    }
	
    return NULL;	
}
	
// Builds the controls
	
void PKToolbar::build()
{
	if(this->built)
	{
		return;
	}
	
#ifdef MACOSX
	
	// Create the toolbar
	
	PKVariant   *i = this->get(PKToolbar::ID_STRING_PROPERTY);
	std::string id = PKVALUE_STRING(i);
	
	std::wstring wid  = PKStr::stringToWString(id);
	CFStringRef  cfId = PKStr::wStringToCFString(wid);
	
	OSStatus err = HIToolbarCreate(cfId, kHIToolbarNoAttributes, &this->toolbar);
		
	if(err == noErr)
	{
		// Install Event Handler
		
		static const EventTypeSpec kToolbarEvents[] = 
		{
			{ kEventClassToolbar, kEventToolbarGetDefaultIdentifiers },
			{ kEventClassToolbar, kEventToolbarCreateItemWithIdentifier },
		};
		
		InstallEventHandler(HIObjectGetEventTarget((HIToolbarRef)this->toolbar),							
							PKToolbarDelegate, GetEventTypeCount( kToolbarEvents ),							
							kToolbarEvents, this, &this->handler);		

        this->built = true;
	}
	
	CFRelease(cfId);
	
#endif

#ifdef LINUX

    this->widget = gtk_toolbar_new();

    if(this->widget != NULL)
    {
        // Setup
        
        gtk_toolbar_set_style(GTK_TOOLBAR(this->widget), GTK_TOOLBAR_BOTH);
    
        // Create the contents
        
		uint32_t count = this->getChildrenCount();
		
		for(uint32_t i=0; i < count; i++)
		{
			PKObject *child = this->getChildAt(i);
			
			//
			// See which type of object it is
			//
			
			// Try standard item
			
			PKToolbarItem *item = dynamic_cast<PKToolbarItem *>(child);
			
			if(item != NULL)
			{
			    // Get the id

                PKVariant *i   = item->get(PKToolbarItem::ID_STRING_PROPERTY);
                std::string id = PKVALUE_STRING(i);

			    // Get the text
			
			    PKVariant *t = item->get(PKToolbarItem::TEXT_WSTRING_PROPERTY);
			    std::wstring text = PKVALUE_WSTRING(t);
			    std::string  textUTF8 = PKStr::wStringToUTF8string(text);
			    
			    PKVariant *h = item->get(PKToolbarItem::HELP_WSTRING_PROPERTY);
			    std::wstring help = PKVALUE_WSTRING(h);
			    std::string  helpUTF8 = PKStr::wStringToUTF8string(help);

			    // Get the image
			    
				PKVariant *img = item->get(PKToolbarItem::ICON_WSTRING_PROPERTY);
				std::wstring imgURL = PKVALUE_WSTRING(img);
			
				PKImage *bitmap = NULL;
			
				PKImageURIType type;
				std::wstring   image;
			
				PKImage::processURI(imgURL, &type, &image);
			
				if(type == PKIMAGE_URI_RESOURCE)
				{
					bitmap = PKImageLoader::loadBitmapFromResource(image);
				}
			
				if(type == PKIMAGE_URI_FILENAME)
				{
					bitmap = PKImageLoader::loadBitmapFromFilename(image);
				}
				
				if(bitmap)
				{
                    GtkWidget *image = gtk_image_new();
                    
	                // Invert R & B
	                uint32_t width  = bitmap->getWidth();
	                uint32_t height = bitmap->getWidth();
	                uint8_t *pdata  = (uint8_t *) bitmap->getPixels();
	
	                for(unsigned int j=0; j < height; j++)
	                {
		                for(unsigned int i=0; i < width; i++)
		                {
		                    uint8_t temp = pdata[0];
		                     
			                pdata[0] = pdata[2];
     		                pdata[2] = temp;

			                pdata +=4;
		                }
	                }

                    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*) bitmap->getPixels(),
                                                            GDK_COLORSPACE_RGB,
                                                            TRUE, 8, width, height, 
                                                            width*4, 
                                                            NULL, NULL);

                    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
                    gtk_widget_show(image);
                    				
    			    GtkToolItem *button = gtk_tool_button_new(image, textUTF8.c_str());

                    if(helpUTF8 != "")
                    {
                        gtk_tool_item_set_tooltip_markup(button, helpUTF8.c_str());
                    }
                    
                    gtk_widget_show(GTK_WIDGET(button));
                    
                    // Track
                    
                    this->ids.push_back(id);
                    this->buttons.push_back((GtkToolButton *) button);
                    
                    // Connect
                    
                    g_signal_connect(G_OBJECT(button), "clicked", 
                                     G_CALLBACK(PKToolbar::gtk_toolbar_button_clicked), this);
                    
                    // Insert
                    
    			    gtk_toolbar_insert(GTK_TOOLBAR(this->widget),
                                       GTK_TOOL_ITEM(button), -1);    			    
				}			    
			    
			}
			
			// Try Separator
			
			PKToolbarSeparator *sep = dynamic_cast<PKToolbarSeparator *>(child);
			
			if(sep != NULL)
			{
			    GtkToolItem *item = gtk_separator_tool_item_new();
			    
			    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(item), TRUE);
			    gtk_tool_item_set_expand(item, FALSE);
                gtk_widget_show(GTK_WIDGET(item));

			    gtk_toolbar_insert(GTK_TOOLBAR(this->widget),
                                   GTK_TOOL_ITEM(item), -1);    			    
			}

			// Try Flexible Space
			
			PKToolbarFlexibleSpace *fs = dynamic_cast<PKToolbarFlexibleSpace *>(child);
			
			if(fs != NULL)
			{
			    GtkToolItem *item = gtk_separator_tool_item_new();
			    
			    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(item), FALSE);
			    gtk_tool_item_set_expand(item, TRUE);
                gtk_widget_show(GTK_WIDGET(item));

			    gtk_toolbar_insert(GTK_TOOLBAR(this->widget),
                                   GTK_TOOL_ITEM(item), -1);    			    
			}
		}
    }

#endif

#if(defined(WIN32))
	
    if(this->font == NULL)
    {
        return;
    }
	
    PKDoubleBufferedControl::build();
	
#endif
}

void PKToolbar::initialize(PKDialog *dialog)
{
#if(defined(WIN32))
	PKDoubleBufferedControl::initialize(dialog);
    this->createLayout();	
#endif

#if(defined(LINUX))	
	PKControl::initialize(dialog);
#endif
}

void PKToolbar::createLayout()
{
    if(this->built)
    {
        return;
    }
	
#if(defined(WIN32))
	
    // Setup the layout, images, labels
    // and fixed/flexible space for drawing

    //
    // Calculate width of flexible spaces
    //

    uint32_t size  = 0;
    uint32_t nums  = 0;
    uint32_t count = this->getChildrenCount();

    for(uint32_t i=0; i < count; i++)
    {
        PKObject *child = this->getChildAt(i);

        PKToolbarItem *item = dynamic_cast<PKToolbarItem*>(child);

        if(item)
        {
            // Add image to our vector

			PKVariant *img = item->get(PKToolbarItem::ICON_WSTRING_PROPERTY);
			std::wstring imgURL = PKVALUE_WSTRING(img);
		
			PKImage *bitmap = this->loadBitmap(imgURL);
		
			if(bitmap != NULL)
			{
                PKDoubleBufferedControl::premultiplyAlpha(bitmap);
                this->images.push_back(bitmap);

                //
                // Create over and down states
                //

                PKImage *over = new PKImage();

                if(over)
                {
                    over->create(bitmap->getWidth(), bitmap->getHeight());
                    over->copy(bitmap);

                    // Now alter it

                    uint8_t *pixels = (uint8_t *) over->getPixels();

                    for(uint32_t j = 0; j < bitmap->getHeight(); j++)
                    {
                        for(uint32_t i = 0; i < bitmap->getWidth(); i++)
                        {
                            uint32_t r = *pixels;
                            uint32_t g = *(pixels+1);
                            uint32_t b = *(pixels+2);
                            uint32_t a = *(pixels+3);

                            r = pk_min(0xFF, r+25);
                            g = pk_min(0xFF, g+25);
                            b = pk_min(0xFF, b+25);

                            if(r > a) r = a;
                            if(g > a) g = a;
                            if(b > a) b = a;

                            *pixels     = (uint8_t) r;
                            *(pixels+1) = (uint8_t) g;
                            *(pixels+2) = (uint8_t) b;

                            pixels += 4;
                        }
                    }

                    this->over_images.push_back(over);
                }

                PKImage *down = new PKImage();

                if(down)
                {
                    down->create(bitmap->getWidth(), bitmap->getHeight());
                    down->copy(bitmap);

                    // Now alter it

                    uint8_t *pixels = (uint8_t *) down->getPixels();

                    for(uint32_t j = 0; j < bitmap->getHeight(); j++)
                    {
                        for(uint32_t i = 0; i < bitmap->getWidth(); i++)
                        {
                            int32_t r = *pixels;
                            int32_t g = *(pixels+1);
                            int32_t b = *(pixels+2);

                            r = pk_max(0x00, r-25);
                            g = pk_max(0x00, g-25);
                            b = pk_max(0x00, b-25);

                            *pixels     = (uint8_t) r;
                            *(pixels+1) = (uint8_t) g;
                            *(pixels+2) = (uint8_t) b;

                            pixels += 4;
                        }
                    }

                    this->down_images.push_back(down);
                }
            }

            // Add label to our vector

            PKVariant *t = item->get(PKToolbarItem::TEXT_WSTRING_PROPERTY);
            std::wstring text = PKVALUE_WSTRING(t);

            size += pk_max(32, this->font->getTextWidth(text));
			size += 15;

            this->labels.push_back(text);
            this->segments.push_back(NULL);
        }
        
        PKToolbarSegmentedItem *sitem = dynamic_cast<PKToolbarSegmentedItem*>(child);

        if(sitem)
        {
			PKVariant *img = sitem->get(PKToolbarSegmentedItem::NORMAL_IMAGE_WSTRING_PROPERTY);
			std::wstring imgURL = PKVALUE_WSTRING(img);
		
			PKImage *bitmap = this->loadBitmap(imgURL);
		
			if(bitmap != NULL)
			{
				// Load all images

                PKDoubleBufferedControl::premultiplyAlpha(bitmap);
                this->images.push_back(bitmap);
			
				std::wstring downURL = PKVALUE_WSTRING(sitem->get(PKToolbarSegmentedItem::DOWN_IMAGE_WSTRING_PROPERTY));
				PKImage *down_bitmap = this->loadBitmap(downURL);
                PKDoubleBufferedControl::premultiplyAlpha(down_bitmap);
				this->down_images.push_back(down_bitmap);

				std::wstring hoverURL = PKVALUE_WSTRING(sitem->get(PKToolbarSegmentedItem::HOVER_IMAGE_WSTRING_PROPERTY));
				PKImage *hover_bitmap = this->loadBitmap(hoverURL);
                PKDoubleBufferedControl::premultiplyAlpha(hover_bitmap);
				this->over_images.push_back(hover_bitmap);

				PKVariant *t = sitem->get(PKToolbarSegmentedItem::TEXT_WSTRING_PROPERTY);
				std::wstring text = PKVALUE_WSTRING(t);

				size += pk_max(bitmap->getWidth(), this->font->getTextWidth(text));
				size += 15;

				this->labels.push_back(text);
                this->segments.push_back(sitem);
			}
		}

        PKToolbarFlexibleSpace *space = dynamic_cast<PKToolbarFlexibleSpace *>(child);

        if(space != NULL)
        {
            nums += 1;
        }

        PKToolbarSeparator *sep = dynamic_cast<PKToolbarSeparator*>(child);

		if(sep != NULL)
        {
            size += 15;
        }
    }

    // Now we can calculate the space of
    // one flexible space item, and go through
    // the loop one more time to place items

    int32_t spaceSize = 0;

    uint32_t width = (this->getW() - 10);

    if(nums > 0)
    {
        spaceSize = (width - size) / nums;
    }

    if(spaceSize < 0)
    {
        spaceSize = 0;
    }

    count = this->getChildrenCount();

    int32_t offset = 10;
    uint32_t index  = 0;
    uint32_t th     = this->getH();

    for(uint32_t i=0; i < count; i++)
    {
        PKObject *child = this->getChildAt(i);

        PKToolbarItem *item = dynamic_cast<PKToolbarItem*>(child);

        if(item)
        {
            PKVariant *t = item->get(PKToolbarItem::TEXT_WSTRING_PROPERTY);
            std::wstring text = PKVALUE_WSTRING(t);

            PKVariant *tt = item->get(PKToolbarItem::HELP_WSTRING_PROPERTY);
            std::wstring tooltip = PKVALUE_WSTRING(tt);

            // Get info

            int32_t lsize = this->font->getTextWidth(text);
            int32_t lh    = this->font->getCharHeight();

            // Place image

            PKPoint off;
            off.x = 0;
            off.y = 0;

            int32_t iw = 0;
            int32_t ih = 0;

            if(this->images.size() > index)
            {
                iw = this->images[index]->getWidth();
                ih = this->images[index]->getHeight();

				if(lsize > iw)
				{
	                off.x = offset + (lsize - iw)/2;
				}
				else
				{
	                off.x = offset;
				}
		        
				off.y = (th - (ih + lh + 5))/2;
			
                this->positions.push_back(off);
            }

            if(off.y == 0)
            {
                off.y = 5;
            }

            // Place label

            PKPoint pt;

			if(lsize > iw)
			{
	            pt.x = offset;
			}
			else
			{
	            pt.x = offset + (iw - lsize)/2;
			}

            pt.y = th - off.y - lh - 3;
            
            this->lpositions.push_back(pt);
            
            index++;
            offset += (pk_max(32,lsize)+15);

            // Compute item rectangles

            PKRect rect;
            rect.x = pk_min(off.x, pt.x);
            rect.w = pk_max(lsize, iw);
            rect.y = off.y;
            rect.h = ih + 3 + lh;

            this->itemAreas.push_back(rect);

            // And add tooltips

            this->addTooltip(rect, tooltip);
        }

		PKToolbarSegmentedItem *sitem = dynamic_cast<PKToolbarSegmentedItem*>(child);

        if(sitem)
        {
            PKVariant *t = sitem->get(PKToolbarSegmentedItem::TEXT_WSTRING_PROPERTY);
            std::wstring text = PKVALUE_WSTRING(t);

            PKVariant *tt = sitem->get(PKToolbarSegmentedItem::HELP_WSTRINGLIST_PROPERTY);
            std::vector<std::wstring> tooltips = PKVALUE_WSTRINGLIST(tt);

            PKVariant *of = sitem->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY);
            std::vector<std::string> offsets = PKVALUE_STRINGLIST(of);

            // Get info

            int32_t lsize = this->font->getTextWidth(text);
            int32_t lh    = this->font->getCharHeight();

            // Place image

            PKPoint off;
            off.x = 0;
            off.y = 0;

            int32_t iw = 0;
            int32_t ih = 0;

            if(this->images.size() > index)
            {
                iw = this->images[index]->getWidth();
                ih = this->images[index]->getHeight();

				if(lsize > iw)
				{
	                off.x = offset + (lsize - iw)/2;
				}
				else
				{
	                off.x = offset;
				}
		        
				off.y = (th - (ih + lh + 5))/2;
			
                this->positions.push_back(off);
            }

            if(off.y == 0)
            {
                off.y = 5;
            }

            // Place label

            PKPoint pt;

			if(lsize > iw)
			{
	            pt.x = offset;
			}
			else
			{
	            pt.x = offset + (iw - lsize)/2;
			}

            pt.y = th - off.y - lh - 3;
            
            this->lpositions.push_back(pt);
            
            index++;
            offset += (pk_max(iw,lsize)+15);

            // Compute item rectangles

            PKRect rect;
            rect.x = pk_min(off.x, pt.x);
            rect.w = pk_max(lsize, iw);
            rect.y = off.y;
            rect.h = ih + 3 + lh;

            this->itemAreas.push_back(rect);

            // And add tooltips

            if(tooltips.size() + 1 == offsets.size())
            {
                for(uint32_t n=0; n<tooltips.size(); n++)
                {
                    int off1 = PKStr::stringToInt32(offsets[n]);
                    int off2 = PKStr::stringToInt32(offsets[n+1]);

                    PKRect tt_rect;

                    tt_rect.x = off.x + off1;
                    tt_rect.w = off2 - off1;
                    tt_rect.y = off.y;
                    tt_rect.h = ih;

                    this->addTooltip(tt_rect, tooltips[n]);
                }
            }
		}

        PKToolbarFlexibleSpace *space = dynamic_cast<PKToolbarFlexibleSpace*>(child);

        if(space)
        {
            offset += spaceSize;
        }

        PKToolbarSeparator *sep = dynamic_cast<PKToolbarSeparator*>(child);

        if(sep)
        {
            this->spositions.push_back(offset+1);
            offset += 15;
        }
    }

#endif // WIN32

    this->built = true;
}

void PKToolbar::deleteLayout()
{
#if(defined(WIN32))

    this->built = false;

    // Clean Up

    this->clearTooltips();

    for(uint32_t i=0; i < this->images.size(); i++)
    {
        delete this->images[i];
    }
    this->images.clear();

    for(uint32_t i=0; i < this->over_images.size(); i++)
    {
        delete this->over_images[i];
    }
    this->over_images.clear();

    for(uint32_t i=0; i < this->down_images.size(); i++)
    {
        delete this->down_images[i];
    }
    this->down_images.clear();

    this->positions.clear();
    this->labels.clear();
    this->lpositions.clear();
    this->spositions.clear();
    this->itemAreas.clear();

#endif
}

void PKToolbar::resize(uint32_t w, 
					   uint32_t h)
{
#if(defined(WIN32))

    PKDoubleBufferedControl::resize(w, h);

	// Update our layouting

	if(!this->built)
	{
		return;
	}

	this->deleteLayout();
	this->createLayout();

#endif
}

void *PKToolbar::getToolbarHandle()
{
#ifdef MACOSX
	return (void *) this->toolbar;
#endif	
	
	return NULL;
}

//
// WIN32 draw
//

#if(defined(WIN32))

void PKToolbar::draw32bpp(uint8_t *buffer)
{
    // Draw gradient background

    PKVariant *c1  = this->get(PKToolbar::LIGHTCOLOR_COLOR_PROPERTY);
    PKColor color1 = PKVALUE_COLOR(c1);

    PKVariant *c2  = this->get(PKToolbar::DARKCOLOR_COLOR_PROPERTY);
    PKColor color2 = PKVALUE_COLOR(c2);

    int w = this->getW();
    
    uint32_t width = this->getW();

#if(defined(LINUX))
    
    GtkAllocation all;
    gtk_widget_get_allocation(this->widget, &all);
    w = all.width;

#endif
    
    int h = this->getH();

    // Fill the buffer

    PKBitmapOps::gradientFill(buffer, w, h, PK_ORIENTATION_VERTICAL, color2, color1);

    // Now draw images

    uint32_t count = this->images.size();

    for(uint32_t i=0; i < count; i++)
    {
        if(this->segments[i])
        {
            // Segmented control

            int32_t selection = PKVALUE_INT32(this->segments[i]->get(PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY));
            std::vector<std::string> offsets = PKVALUE_STRINGLIST(this->segments[i]->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY));

            int x = this->positions[i].x;
            int y = this->positions[i].y;

            for(uint32_t n=0; n<offsets.size()-1; n++)
            {
                int32_t offset1 = PKStr::stringToInt32(offsets[n]);
                int32_t offset2 = PKStr::stringToInt32(offsets[n+1]);

                if(n == selection)
                {
                    // Draw down
                    PKBitmapOps::alphaBlit(buffer, 255, x+offset1, y, w, h, this->down_images[i], offset1, 0, offset2 - offset1);
                }
                else
                {
                    // Draw down, normal or over

                    if(this->downIndex == i && this->downSubIndex == n) // down
                    {
                        PKBitmapOps::alphaBlit(buffer, 255, x+offset1, y, w, h, this->down_images[i], offset1, 0, offset2 - offset1);
                    }
                    else
                    {
                        if(this->overIndex == i && this->overSubIndex == n) // hover
                        {
                            PKBitmapOps::alphaBlit(buffer, 255, x+offset1, y, w, h, this->over_images[i], offset1, 0, offset2 - offset1);
                        }
                        else // normal
                        {
                            PKBitmapOps::alphaBlit(buffer, 255, x+offset1, y, w, h, this->images[i], offset1, 0, offset2 - offset1);
                        }
                    }
                }
            }
        }
        else
        {
            // Standard button

            if(i == this->downIndex)
            {
                PKImage *image = this->down_images[i];

                if(image)
                {
                    int x = this->positions[i].x;
                    int y = this->positions[i].y;

                    PKBitmapOps::alphaBlit(buffer, 255, x, y, w, h, image);
                }
            }
            else
            {
                if(i == this->overIndex && this->downIndex == -1)
                {
                    PKImage *image = this->over_images[i];

                    if(image)
                    {
                        int x = this->positions[i].x;
                        int y = this->positions[i].y;

                        PKBitmapOps::alphaBlit(buffer, 255, x, y, w, h, image);
                    }
                }
                else
                {
                    PKImage *image = this->images[i];

                    if(image)
                    {
                        int x = this->positions[i].x;
                        int y = this->positions[i].y;

                        PKBitmapOps::alphaBlit(buffer, 255, x, y, w, h, image);
                    }
                }
            }
        }
    }   

    // And draw labels

    uint32_t lcount = this->labels.size();

    for(uint32_t l=0; l < lcount; l++)
    {
        int x = this->lpositions[l].x;
        int y = this->lpositions[l].y;

        if(this->shadowFont != NULL)
        {
#ifdef WIN32
			if(PKOS::getWindowsVersion() != WIN_VISTA &&
			   PKOS::getWindowsVersion() != WIN_7)
			{
	            this->shadowFont->drawText(this->labels[l],
    		                           	   buffer, x, y+1, w, h);
        	}
#endif
		}

        this->font->drawText(this->labels[l],
                             buffer, x, y, w, h);
    }   

    // Finally, draw separators

    uint32_t scount = this->spositions.size();

    for(uint32_t s=0; s < scount; s++)
    {
        int x = this->spositions[s];

        uint32_t color = ((0xFF << 24) | ((unsigned int) color1.r/2 << 16) | 
					     ((unsigned int) color1.g/2 << 8) | (unsigned int) color1.b/2);

        
        for(int32_t k=0; k < h; k++)
        {
            if((k >= 6) && (k < (h-6)) && ((k % 3) == 0))
            {
                uint32_t *pixels = (uint32_t*) buffer + k*w;
                pixels[x] = color;
            }
        }
    }
}

void PKToolbar::mouseDown(PKButtonType button,
			              int x,
			              int y,
			              unsigned int mod)
{
    for(uint32_t n=0; n < this->itemAreas.size(); n++)
    {
        PKRect item = this->itemAreas[n];

        if((x >= item.x) && (x <= (int32_t)(item.x + item.w)) &&
           (y >= item.y) && (y <= (int32_t)(item.y + item.h)))
        {
            // It's a hit !

            this->downIndex = n;

            if(this->segments[n])
            {
                // We need to look deeper
                std::vector<std::string> offsets = PKVALUE_STRINGLIST(this->segments[n]->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY));

                this->downSubIndex = -1;

                for(int32_t k=0; k<offsets.size()-1; k++)
                {
                    int off1 = PKStr::stringToInt32(offsets[k]);
                    int off2 = PKStr::stringToInt32(offsets[k+1]);

                    if((x >= item.x + (item.w - this->images[n]->getWidth())/2 + off1) && 
                       (x <= item.x + (item.w - this->images[n]->getWidth())/2 + off2) &&
                       (y >= this->positions[n].y && y < (this->positions[n].y + this->images[n]->getHeight())))
                    {
                        this->downSubIndex = k;
                        break;
                    }
                }
            }

            this->invalidate();
            return;
        }
    }
   
    // If we get there we 
    // are over no item

    if(this->downIndex != -1)
    {
        this->downIndex = -1;
        this->invalidate();
        return;
    }
}

void PKToolbar::mouseUp(PKButtonType button,
			            int x,
			            int y,
			            unsigned int mod)
{
    // Check if we are above an item

    for(uint32_t n=0; n < this->itemAreas.size(); n++)
    {
        PKRect item = this->itemAreas[n];

        if((x >= item.x) && (x <= (int32_t)(item.x + item.w)) &&
           (y >= item.y) && (y <= (int32_t)(item.y + item.h)))
        {
            if(n == this->downIndex)
            {
                if(this->segments[n])
                {
                    // It's on a segmented control
                    // We need to look deeper
                    std::vector<std::string> offsets = PKVALUE_STRINGLIST(this->segments[n]->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY));

                    for(int32_t k=0; k<offsets.size()-1; k++)
                    {
                        int off1 = PKStr::stringToInt32(offsets[k]);
                        int off2 = PKStr::stringToInt32(offsets[k+1]);

                        if((x >= item.x + (item.w - this->images[n]->getWidth())/2 + off1) && 
                           (x <= item.x + (item.w - this->images[n]->getWidth())/2 + off2) &&
                           (y >= this->positions[n].y && y < (this->positions[n].y + this->images[n]->getHeight())))
                        {
                            if(k == this->downSubIndex)
                            {
                                // It's a hit!

                                PKWindow *window = this->getWindow();

                                if(window)
                                {
                                    PKDialog *dialog = dynamic_cast<PKDialog*>(window);

                                    if(dialog)
                                    {
                                        PKVariant *i   = this->segments[n]->get(PKToolbarSegmentedItem::ID_STRING_PROPERTY);
                                        std::string id = PKVALUE_STRING(i);

                                        int32_t selection = PKVALUE_INT32(this->segments[n]->get(PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY));

                                        if(k != selection)
                                        {
                                            PKVariantInt32 newSel(k);
                                            this->segments[n]->set(PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY, &newSel);

                                            dialog->toolbarSegmentedItemSwitched(this, this->segments[n], id, k);

                                            if(this->downIndex != -1)
                                            {
                                                this->downIndex = -1;
                                                this->downSubIndex = -1;
                                                this->invalidate();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }
                else
                {
                    // It's a hit on a normal button

                    PKWindow *window = this->getWindow();

                    if(window)
                    {
                        PKDialog *dialog = dynamic_cast<PKDialog*>(window);

                        if(dialog)
                        {
                            int count = 0;

                            for(uint32_t c=0; c < this->getChildrenCount(); c++)
                            {
                                PKToolbarItem *item = dynamic_cast<PKToolbarItem*>(this->getChildAt(c));
                                PKToolbarSegmentedItem *sitem = dynamic_cast<PKToolbarSegmentedItem*>(this->getChildAt(c));

                                if(item || sitem)
                                {
                                    if(count == n) // That's the one
                                    {
                                        PKVariant *i   = item->get(PKToolbarItem::ID_STRING_PROPERTY);
                                        std::string id = PKVALUE_STRING(i);

                                        dialog->toolbarItemClicked(this, item, id);

                                        if(this->downIndex != -1)
                                        {
                                            this->downIndex = -1;
                                            this->downSubIndex = -1;
                                            this->invalidate();
                                        }

                                        return;
                                    }

                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Upate ourselves

    if(this->downIndex != -1)
    {
        this->downIndex = -1;
        this->downSubIndex = -1;
        this->invalidate();
    }
}

void PKToolbar::mouseDblClick(PKButtonType button,
				              int x,
				              int y,
				              unsigned int mod)
{
    // Yeah
    this->mouseDown(button, x, y, mod);
}

void PKToolbar::mouseMove(int x,
			              int y,
			              unsigned int mod)
{
    // Check if we are over an item

    for(uint32_t n=0; n < this->itemAreas.size(); n++)
    {
        PKRect item = this->itemAreas[n];

        if((x >= item.x) && (x <= (int32_t)(item.x + item.w)) &&
           (y >= item.y) && (y <= (int32_t)(item.y + item.h)))
        {
            // It's a hit !

            this->overIndex = n;

            if(this->segments[n])
            {
                // We need to look deeper
                std::vector<std::string> offsets = PKVALUE_STRINGLIST(this->segments[n]->get(PKToolbarSegmentedItem::OFFSETS_STRINGLIST_PROPERTY));

                this->overSubIndex = -1;

                for(int32_t k=0; k<offsets.size()-1; k++)
                {
                    int off1 = PKStr::stringToInt32(offsets[k]);
                    int off2 = PKStr::stringToInt32(offsets[k+1]);

                    if((x >= item.x + (item.w - this->images[n]->getWidth())/2 + off1) && 
                       (x <= item.x + (item.w - this->images[n]->getWidth())/2 + off2) &&
                       (y >= this->positions[n].y && y < (this->positions[n].y + this->images[n]->getHeight())))
                    {
                        this->overSubIndex = k;
                        break;
                    }
                }
            }

            this->invalidate();
            return;
        }
    }
   
    // If we get there we 
    // are over no item

    if(this->overIndex != -1)
    {
        this->overIndex = -1;
        this->overSubIndex = -1;
        this->invalidate();
        return;
    }
}

void PKToolbar::mouseLeave()
{
    // If we get there we 
    // are over no item

    if(this->overIndex != -1)
    {
        this->overIndex = -1;
        this->overSubIndex = -1;
        this->invalidate();
        return;
    }
}

void PKToolbar::invalidate(bool recurse)
{
    PKDoubleBufferedControl::invalidate(recurse);
}

#endif

#ifndef MACOSX

uint32_t PKToolbar::getMinimumWidth()
{
    return 0;
}

uint32_t PKToolbar::getMinimumHeight()
{
#ifdef WIN32

    if(this->images.size() > 0 && this->font)
    {
        return (15 + this->images[0]->getHeight() + this->font->getCharHeight());
    }

#endif

    return PKToolbar::PK_TOOLBAR_HEIGHT;
}

#endif

#ifdef LINUX

void PKToolbar::gtk_toolbar_button_clicked(GtkToolButton *toolbutton,
                                           gpointer       user_data)
{
    PKToolbar *toolbar = (PKToolbar *) user_data;
    
    if(toolbar != NULL)
    {
        toolbar->linuxButtonClicked(toolbutton);
    }
}                                         

void PKToolbar::linuxButtonClicked(GtkToolButton *toolbutton)
{
    std::string id = "";

    for(uint32_t i=0; i<this->buttons.size(); i++)
    {
        if(this->buttons[i] == toolbutton)
        {
            id = this->ids[i];    
            break;
        }   
    }
    
    if(id != "")
    {
        uint32_t count = this->getChildrenCount();

        for(uint32_t i=0; i < count; i++)
        {
            PKObject *child = this->getChildAt(i);

            PKToolbarItem *item = dynamic_cast<PKToolbarItem*>(child);

            if(item)
            {
                PKVariant  *ii  = item->get(PKToolbarItem::ID_STRING_PROPERTY);
                std::string iid = PKVALUE_STRING(ii);
                
                if(id == iid)
                {
                    if(this->dialog)
                    {
                        this->dialog->toolbarItemClicked(this, item, iid);
                    }
                }
            }
        }        
    }
}                                         

#endif

//
// i18n
//

void PKToolbarChildrenWalkI18N(PKObject *child, void *context)
{
    if(child != NULL)
    {
        PKPropertyBag *props = child->getProperties();
		
        if(props != NULL)
        {
            uint32_t count = props->getNumberOfProperties();
			
            for(uint32_t i=0; i < count; i++)
            {
                PKProperty *p = props->getPropertyAt(i);
				
                if(p != NULL)
                {
                    PKVariant *value = p->getValue();
					
                    if(value->_type == PK_VARIANT_WSTRING)
                    {
                        PKVariantWStringI18N *i18n = dynamic_cast<PKVariantWStringI18N *>(value);
						
                        if(i18n)
                        {
                            PKVariantWStringI18N *newVal = new PKVariantWStringI18N(i18n->_original);
                            newVal->_val = PK_i18n(i18n->_original);
                            p->setValue(newVal);
                            delete newVal;
                        }
                    }
                }
            }
        }
    }
}

void PKToolbar::updateUIStrings()
{
	// Update our children
	
	this->walkChildren(PKToolbarChildrenWalkI18N, NULL);
	
#if(defined(WIN32))

    // And rebuild ourselves in order
    // to reflect the new strings

    this->deleteLayout();
    this->createLayout();

#endif

#ifdef LINUX

    for(uint32_t i=0; i<this->buttons.size(); i++)
	{
	    GtkToolButton *button = this->buttons[i];
	    std::string    id     = this->ids[i];
	    
	    for(uint32_t n=0; n<this->getChildrenCount(); n++)
    	{
			PKObject *child = this->getChildAt(n);			
			PKToolbarItem *item = dynamic_cast<PKToolbarItem *>(child);
			
			if(item != NULL)
			{
			    // Get the id

                PKVariant  *ii  = item->get(PKToolbarItem::ID_STRING_PROPERTY);
                std::string iid = PKVALUE_STRING(ii);

                if(iid == id)
			    {
			        // Get the text
			
			        PKVariant *t = item->get(PKToolbarItem::TEXT_WSTRING_PROPERTY);
			        std::wstring text = PKVALUE_WSTRING(t);
			        std::string  textUTF8 = PKStr::wStringToUTF8string(text);
			        
			        PKVariant *h = item->get(PKToolbarItem::HELP_WSTRING_PROPERTY);
			        std::wstring help = PKVALUE_WSTRING(h);
			        std::string  helpUTF8 = PKStr::wStringToUTF8string(help);

                    if(helpUTF8 != "")
                    {
                        gtk_tool_item_set_tooltip_markup(GTK_TOOL_ITEM(button), helpUTF8.c_str());
                    }                    
                    
                    gtk_tool_button_set_label(button, textUTF8.c_str());

                    break;
            	}	    
        	}	    
    	}	    
	}	    

#endif

#ifdef MACOSX
	
	if(this->handler != NULL)
	{
		RemoveEventHandler(this->handler);
		this->handler = NULL;
	}	

	if(this->toolbar != NULL)
	{
		CFRelease(this->toolbar);
		this->toolbar = NULL;
	}

	this->built = false;
	this->build();
	
#endif
}

PKWindow *PKToolbar::getWindow()
{
	return this->window;
}

void PKToolbar::setWindow(PKWindow *window)
{
	this->window = window;
}

void PKToolbar::setSegmentedItemSelection(std::string id, int32_t sel)
{
    for(uint32_t i=0; i<this->getChildrenCount(); i++)
    {
        PKToolbarSegmentedItem *item = dynamic_cast<PKToolbarSegmentedItem *>(this->getChildAt(i));

        if(item)
        {
            std::string item_id = PKVALUE_STRING(item->get(PKToolbarSegmentedItem::ID_STRING_PROPERTY));

            if(item_id == id)
            {
                PKVariantInt32 new_sel(sel);
                item->set(PKToolbarSegmentedItem::SELECTION_INT32_PROPERTY, &new_sel);
#ifdef WIN32
                this->invalidate();
#endif
                break;
            }
        }
    }
}

PKImage *PKToolbar::loadBitmap(std::wstring imgURL)
{
    PKImage *bitmap = NULL;
	
	PKImageURIType type;
	std::wstring   image;
	
	PKImage::processURI(imgURL, &type, &image);

	if(type == PKIMAGE_URI_RESOURCE)
	{
    	bitmap = PKImageLoader::loadBitmapFromResource(image);
	}

	if(type == PKIMAGE_URI_FILENAME)
	{
    	bitmap = PKImageLoader::loadBitmapFromFilename(image);
	}

    return bitmap;
}
