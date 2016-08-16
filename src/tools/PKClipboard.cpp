//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKClipboard.h"
#include "PKStr.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

#ifdef LINUX
#include <gtk/gtk.h>
#endif

PKClipboard::PKClipboard()
{
    // Nothing to do
}

PKClipboard::~PKClipboard()
{
    // Nothing to do
}

void PKClipboard::clearClipboard()
{
#ifdef WIN32
    if(OpenClipboard(NULL))
    {
        // Empty
        EmptyClipboard();

        // Cleanup
        CloseClipboard();
    }
#endif

#ifdef LINUX
    GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	
	if(clip)
	{
	    gtk_clipboard_clear(clip);
	}
#endif

#ifdef MACOSX
	
	OSStatus err = noErr;
	PasteboardRef theClipboard;
	
    err = PasteboardCreate(kPasteboardClipboard, &theClipboard);	
	
	if(err == noErr)
	{
		PasteboardClear(theClipboard);
		CFRelease(theClipboard);
	}
	
#endif
}

void PKClipboard::addText(std::wstring text)
{
    uint32_t size = text.size();

    if(size == 0)
    {
        return;
    }

#ifdef WIN32
    if(OpenClipboard(NULL))
    {
        // Empty first
        EmptyClipboard();

        // Allocate memory for the text

        HGLOBAL hClipboardData;
        hClipboardData = GlobalAlloc(GMEM_DDESHARE, 
                                     (size+1)*sizeof(wchar_t));

        if(hClipboardData == NULL)
        {
            CloseClipboard();
            return;
        }

        wchar_t * pchData;
        pchData = (wchar_t*) GlobalLock(hClipboardData);

        if(pchData == NULL)
        {
            GlobalFree(hClipboardData);
            CloseClipboard();
            return;
        }

        memcpy(pchData, text.c_str(), size*sizeof(wchar_t));
        pchData[size] = L'\0';

        GlobalUnlock(hClipboardData);

        // Set the data
        SetClipboardData(CF_UNICODETEXT, hClipboardData);

        // Cleanup
        CloseClipboard();
    }
#endif
	
#ifdef LINUX
    GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	
	if(clip)
	{
	    // Clear
	    gtk_clipboard_clear(clip);

        // Set text
        
        std::string utf8 = PKStr::wStringToUTF8string(text);
        gtk_clipboard_set_text(clip, utf8.c_str(), utf8.size());
	}
#endif

#ifdef MACOSX
	
	OSStatus            err = noErr;
	PasteboardRef       theClipboard;
	PasteboardSyncFlags syncFlags;
	CFDataRef           textData = NULL;	
	
    err = PasteboardCreate(kPasteboardClipboard, &theClipboard);	
	
	if(err == noErr)
	{
		// Clear first
		PasteboardClear(theClipboard);
		
		// Now add text
		
		syncFlags = PasteboardSynchronize( theClipboard );
		
		UniChar *unicode  = NULL;
		uint32_t sizeChar = 0;
		
		PKStr::wStringToUniChar(text, &sizeChar, &unicode);
		
		if(unicode != NULL)
		{
			textData = CFDataCreate(kCFAllocatorDefault,
									(UInt8*)unicode, 
									sizeChar*sizeof(UniChar));

			if(textData)
			{
				err = PasteboardPutItemFlavor(theClipboard, 
											  (PasteboardItemID) 1,
											  CFSTR("public.utf16-plain-text"),
											  textData, 0);
			
				CFRelease(textData);
			}
			
			delete [] unicode;
		}

		CFRelease(theClipboard);
	}
	

#endif	
}

bool PKClipboard::containsText()
{
#ifdef WIN32
    if(OpenClipboard(NULL))
    {
        BOOL hasText    = IsClipboardFormatAvailable(CF_TEXT);
        BOOL hasUnicode = IsClipboardFormatAvailable(CF_UNICODETEXT);
    
        CloseClipboard();

        return ((hasText == TRUE) || (hasUnicode == TRUE));
    }
#endif

#ifdef LINUX
    GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	
	if(clip)
	{
        // Get text
        gchar *text = gtk_clipboard_wait_for_text(clip);
        
        if(text)
    	{
            g_free(text);
            return TRUE;            
    	}
	}
	
	return FALSE;	
#endif

#ifdef MACOSX
	
	OSStatus            err = noErr;
	PasteboardRef       theClipboard;
	PasteboardSyncFlags syncFlags;
	
    err = PasteboardCreate(kPasteboardClipboard, &theClipboard);	
	
	if(err == noErr)
	{
		ItemCount itemCount;
		
		syncFlags = PasteboardSynchronize(theClipboard);
		
		err = PasteboardGetItemCount(theClipboard, &itemCount);
		
		if(err != noErr)
		{
			CFRelease(theClipboard);
			return false;
		}
		
		for(UInt32 itemIndex = 1; itemIndex <= itemCount; itemIndex++)
		{
			PasteboardItemID    itemID;
			CFArrayRef          flavorTypeArray;
			CFIndex             flavorCount;
			
			err = PasteboardGetItemIdentifier(theClipboard, itemIndex, &itemID);

			if(err != noErr)
			{
				continue;
			}
			
			err = PasteboardCopyItemFlavors(theClipboard, itemID, &flavorTypeArray);

			if(err != noErr)
			{
				continue;
			}
			
			flavorCount = CFArrayGetCount(flavorTypeArray);
			
			for(CFIndex flavorIndex = 0; flavorIndex < flavorCount; flavorIndex++)
			{
				CFStringRef             flavorType;
								
				flavorType = (CFStringRef) CFArrayGetValueAtIndex(flavorTypeArray,
                                                                  flavorIndex );
				
				if(UTTypeConformsTo(flavorType, CFSTR("public.utf16-plain-text")))
				{
					CFRelease(flavorTypeArray);
					CFRelease(theClipboard);
					return true;
				}
			}

			CFRelease (flavorTypeArray);
		}
		
		CFRelease(theClipboard);
	}
	
#endif

    return false;
}

std::wstring PKClipboard::getText()
{
    std::wstring result = L"";

#ifdef WIN32
    if(OpenClipboard(NULL))
    {
        // Try unicode first

        if(IsClipboardFormatAvailable(CF_UNICODETEXT))
        {
            // Get global memory

            HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
            wchar_t *pchData = (wchar_t*) GlobalLock(hClipboardData);

            if(pchData == NULL)
            {
                CloseClipboard();
                return L"";
            }

            // Assign
            result = pchData;

            // Unlock the global memory.
            GlobalUnlock(hClipboardData);
        }
        else
        {
            if(IsClipboardFormatAvailable(CF_TEXT))
            {
                // Get global memory

                HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
                char *pchData = (char*) GlobalLock(hClipboardData);

                if(pchData == NULL)
                {
                    CloseClipboard();
                    return L"";
                }

                // Assign
                std::string str = pchData;
                result = PKStr::stringToWString(str);

                // Unlock the global memory.
                GlobalUnlock(hClipboardData);

            }
        }
        
        CloseClipboard();
    }
#endif

#ifdef LINUX
    GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	
	if(clip)
	{
        // Get text
        gchar *text = gtk_clipboard_wait_for_text(clip);
        
        if(text)
    	{
    	    std::wstring result = PKStr::utf8StringToWstring(text);
            g_free(text);
            return result;
    	}
	}
	
	return L"";
#endif

#ifdef MACOSX
	
	OSStatus            err = noErr;
	PasteboardRef       theClipboard;
	PasteboardSyncFlags syncFlags;
	
	bool found = false;
	
    err = PasteboardCreate(kPasteboardClipboard, &theClipboard);	
	
	if(err == noErr)
	{
		ItemCount itemCount;
		
		syncFlags = PasteboardSynchronize(theClipboard);
		err = PasteboardGetItemCount(theClipboard, &itemCount);
		
		if(err != noErr)
		{
			CFRelease(theClipboard);
			return false;
		}
		
		for(UInt32 itemIndex = 1; itemIndex <= itemCount; itemIndex++)
		{
			PasteboardItemID    itemID;
			CFArrayRef          flavorTypeArray;
			CFIndex             flavorCount;
			
			err = PasteboardGetItemIdentifier(theClipboard, itemIndex, &itemID);
			
			if(err != noErr)
			{
				continue;
			}
			
			err = PasteboardCopyItemFlavors(theClipboard, itemID, &flavorTypeArray);
			
			if(err != noErr)
			{
				continue;
			}
			
			flavorCount = CFArrayGetCount(flavorTypeArray);
			
			for(CFIndex flavorIndex = 0; flavorIndex < flavorCount; flavorIndex++)
			{
				CFStringRef             flavorType;
				CFDataRef               flavorData;
				CFIndex                 flavorDataSize;
				
				flavorType = (CFStringRef) CFArrayGetValueAtIndex(flavorTypeArray,
                                                                  flavorIndex);
				
				if(UTTypeConformsTo(flavorType, CFSTR("public.utf16-plain-text")))
				{
					err = PasteboardCopyItemFlavorData(theClipboard, itemID,
													   flavorType, &flavorData);
					
					if(err != noErr)
					{
						continue;
					}
					
					flavorDataSize = CFDataGetLength(flavorData);
					const UInt8 *byte = CFDataGetBytePtr(flavorData);

					uint32_t size = flavorDataSize/sizeof(UniChar);
					UniChar *textPtr = new UniChar[size+1];
					
					if(textPtr == NULL)
					{
						CFRelease(flavorData);
						continue;
					}
					
					memcpy(textPtr, byte, flavorDataSize);
					textPtr[size] = 0;

					result = PKStr::uniCharToWString(textPtr, size);
					
					delete [] textPtr;
					CFRelease(flavorData);
					
					found = true;
					break;
				}
			}
			
			CFRelease(flavorTypeArray);
			
			if(found)
			{
				break;
			}
		}
		
		CFRelease(theClipboard);
	}	
	
#endif

    return result;
}
