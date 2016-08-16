//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKChooseFolderDialog.h"

#include "PKPath.h"
#include "PKStr.h"

#ifdef WIN32
#include "shlobj.h"
#include "ole2.h"
#endif

//
// PKChooseFolderDialog
//

PKChooseFolderDialog::PKChooseFolderDialog()
{

}

PKChooseFolderDialog::~PKChooseFolderDialog()
{

}

void PKChooseFolderDialog::setTitle(std::wstring title)
{
	this->title = title;
}

void PKChooseFolderDialog::setInitialFolder(std::wstring folder)
{
	this->folder = folder;
}

std::wstring PKChooseFolderDialog::getInitialFolder()
{
	return this->folder;
}

// Main Function

#ifdef WIN32

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif

// Callback procedure to get the events from
// the dialog and se the initial folder...

int CALLBACK BrowseCallbackProc(HWND hwnd,
								UINT uMsg,
								LPARAM lParam,
								LPARAM lpData)
{
	PKChooseFolderDialog *dialog = (PKChooseFolderDialog *) lpData;

	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		{
			// Set the initial folder

			if(dialog != NULL)
			{
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, 
						   (LPARAM) (void *) dialog->getInitialFolder().c_str());
			}
		}
		break;

	case BFFM_VALIDATEFAILED:
		{
			return 1; // Keep the dialog up
		}
		break;

	default:
		break;
	}
	return 0;
}

#endif

std::wstring PKChooseFolderDialog::showAndGetFolder(PKWindowHandle parent)
{

	std::wstring result = this->folder;

#ifdef WIN32

	BROWSEINFO bfo;

	memset(&bfo, 0, sizeof(BROWSEINFO));

	wchar_t f[MAX_PATH];

	bfo.hwndOwner	   = parent;
	bfo.pszDisplayName = f;
	bfo.lpszTitle      = this->title.c_str();
	bfo.ulFlags        = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	bfo.lpfn           = BrowseCallbackProc;
	bfo.lParam         = (long) (LPVOID) this;

	OleInitialize(NULL);

	LPITEMIDLIST res = SHBrowseForFolder(&bfo);

	if(res != NULL)
	{
		// We have a folder

		wchar_t buffer[10*MAX_PATH];
		
		if(SHGetPathFromIDList(res, buffer))
		{
			result = buffer;
		}
		
		CoTaskMemFree(res);
	}

	OleUninitialize();

#endif

#ifdef LINUX

    std::string titleUTF8 = PKStr::wStringToUTF8string(this->title);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(titleUTF8.c_str(),
                                                    GTK_WINDOW(parent),
                                                    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				                                    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				                                    NULL);

    if(dialog != NULL)
	{
	    // Configure
		
	    if(this->folder != L"")
	    {
	        std::string fUTF8 = PKStr::wStringToUTF8string(this->folder);
    	    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), fUTF8.c_str());
        }	    
	
	    // Run
	
        if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
        {
            char *filename;
            filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

            if(filename != NULL)
            {
                std::string filestr = filename;
                result = PKStr::utf8StringToWstring(filestr);
                g_free (filename);
            }
        }

        gtk_widget_destroy (dialog);
	}
	
	return result;

#endif


#ifdef MACOSX
    
    OSStatus status; 
    NavDialogCreationOptions myDialogOptions; 
    status = NavGetDefaultDialogCreationOptions (&myDialogOptions);
    
    CFStringRef messageStr = NULL;
    CFStringRef titleStr   = NULL;
    
    std::wstring ttl = this->title;
	
    uint32_t ts = 0;
    UniChar *tp = NULL;
    
    PKStr::wStringToUniChar(ttl, &ts, &tp);

    if(tp != NULL)
    {
        titleStr = CFStringCreateWithCharacters(NULL, tp, ts);
        myDialogOptions.windowTitle  = titleStr;        
        delete [] tp;
    }

    uint32_t tts = 0;
    UniChar *ttp = NULL;
    
    PKStr::wStringToUniChar(this->title, &tts, &ttp);
    
    if(ttp != NULL)
    {
        messageStr = CFStringCreateWithCharacters(NULL, ttp, tts);
        myDialogOptions.message = messageStr;
        delete [] ttp;
    }

    myDialogOptions.optionFlags  = (kNavDefaultNavDlogOptions & !kNavAllowMultipleFiles) | kNavSelectDefaultLocation;
    
    NavDialogRef dialogRef;
    
    status = NavCreateChooseFolderDialog(&myDialogOptions, NULL, NULL, NULL, &dialogRef);
    
    status = NavDialogRun(dialogRef);
    
    if(status == noErr)
    {
        NavUserAction action = NavDialogGetUserAction(dialogRef);
        
        if(action == kNavUserActionChoose)
        {
            NavReplyRecord record;
            
            OSStatus osError = NavDialogGetReply(dialogRef, &record);
            
            if(osError == noErr && record.validRecord)
            {                
                
                AEKeyword          theKeyword;
                DescType           actualType;
                FSRef              fileRef;
                Size               actualSize;

                osError = AEGetNthPtr(&(record.selection), 1, typeFSRef,
                                      &theKeyword,
                                      &actualType,
                                      &fileRef, sizeof(fileRef),
                                      &actualSize);

                if(osError == noErr)
                {
                    UInt8 buffer[32768];
                    FSRefMakePath(&fileRef, buffer, 32768);
                    
                    std::string path = (const char *) buffer;
                    result = PKStr::utf8StringToWstring(path);
                }
            }            
            
            NavDisposeReply(&record);
        }
    }
    
    NavDialogDispose(dialogRef);
    
    if(messageStr != NULL) CFRelease(messageStr);
    if(titleStr   != NULL) CFRelease(titleStr);
    
#endif
    
	return result;
}
