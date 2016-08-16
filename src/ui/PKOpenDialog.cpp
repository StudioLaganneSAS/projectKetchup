//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKOpenDialog.h"

#include "PKStr.h"
#include "PKPath.h"

#ifdef MACOSX

void NavOpenDialogCallback(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void* callBackUserData) 
{
	PKOpenDialog* self = reinterpret_cast<PKOpenDialog*> (callBackUserData);
	
	switch (callBackSelector) 
	{
        case kNavCBUserAction:
		{
			NavReplyRecord reply;
			
			if(noErr == NavDialogGetReply(callBackParms->context, &reply)) 
			{				
				NavUserAction action = NavDialogGetUserAction(callBackParms->context);
				
				if(action == kNavUserActionChoose ||
				   action == kNavUserActionSaveAs)
				{
					NavReplyRecord record;
					
					OSStatus osError = NavDialogGetReply(callBackParms->context, &record);
					
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
							self->macDialogResult = PKStr::utf8StringToWstring(path);
						}
						
						NavDisposeReply(&record);
					}            
					
				}
				
				NavDisposeReply(&reply);
			}                       

			QuitAppModalLoopForWindow(self->parent);
		}
		break;

        case kNavCBTerminate:
		{
			QuitAppModalLoopForWindow(self->parent);
		}
		break;
		
		default:
			break;
	}
}


#endif

//
// PKSaveDialog
//

PKOpenDialog::PKOpenDialog()
{
#ifdef MACOSX
	this->macDialogResult = L"";
	this->parent = NULL;
#endif
}

PKOpenDialog::~PKOpenDialog()
{

}

void PKOpenDialog::setTitle(std::wstring title)
{
	this->title = title;
}

void PKOpenDialog::setInitialFolder(std::wstring folder)
{
    this->folder = folder;
}


void PKOpenDialog::setInitialFilename(std::wstring filename)
{
	this->filename = filename;
}

void PKOpenDialog::addFilterEntry(std::wstring extensions,
							      std::wstring description)

{
	this->exts.push_back(extensions);
	this->descs.push_back(description);
}

// Main Function

std::wstring PKOpenDialog::askForOpenFilename(PKWindowHandle parent)
{

	std::wstring result = L"";

#ifdef WIN32

	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner   = parent;
	
	// Build extension filter list

	int count = 0;

	for(unsigned int i=0; i < this->exts.size(); i++)
	{
		count += (this->descs[i].size() + 1);	
		count += (this->exts[i].size()  + 1);	
	}

	wchar_t *filter = new wchar_t[count + 1];

	if(filter == NULL) return L"";

	int offset = 0;

	for(unsigned int n=0; n < this->exts.size(); n++)
	{
		int w = this->descs[n].size();
		memcpy(filter + offset, this->descs[n].c_str(), sizeof(wchar_t)*w);
		offset += w;
		filter[offset] = L'\0';
		offset++;

		w = this->exts[n].size();
		memcpy(filter + offset, this->exts[n].c_str(), sizeof(wchar_t)*w);
		offset += w;
		filter[offset] = L'\0';
		offset++;
	}

	filter[offset] = L'\0';

	ofn.lpstrFilter = filter;

	// Build the output filename buffer

	const unsigned int size = 32768;
	wchar_t file[size];

	int fsize = pk_min(this->filename.size(), size);

	memset(file, 0, sizeof(wchar_t)*size);
	memcpy(file, this->filename.c_str(), 
		   sizeof(wchar_t)*fsize);

	ofn.lpstrFile = file;
	ofn.nMaxFile  = size;

    // Set the initial folder

    wchar_t dir[size];

    int dsize = pk_min(this->folder.size(), size);

	memset(dir, 0, sizeof(wchar_t)*size);
	memcpy(dir, this->folder.c_str(), 
		   sizeof(wchar_t)*dsize);

    ofn.lpstrInitialDir = dir;

	// And the rest

	ofn.lpstrTitle = (LPCWSTR) (unsigned short *) this->title.c_str();
	ofn.Flags = OFN_FILEMUSTEXIST;

	if(GetOpenFileName(&ofn))
	{
		
		delete [] filter;
		result = ofn.lpstrFile;

		return result;
	}
	else
	{
		// No selection

		delete [] filter;
		return L"";
	}

#endif

#ifdef LINUX

    std::string titleUTF8 = PKStr::wStringToUTF8string(this->title);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(titleUTF8.c_str(),
                                                    GTK_WINDOW(parent),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				                                    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				                                    NULL);

    if(dialog != NULL)
	{
	    // Configure
	
	    if(this->filename != L"")
	    {
	        std::string fUTF8 = PKStr::wStringToUTF8string(this->filename);
    	    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), fUTF8.c_str());
        }	    
	
	    if(this->folder != L"")
	    {
	        std::string fUTF8 = PKStr::wStringToUTF8string(this->folder);
    	    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), fUTF8.c_str());
        }	    
	
	    for(unsigned int n=0; n < this->exts.size(); n++)
	    {
	        GtkFileFilter *filter = gtk_file_filter_new();
	        
	        std::string nameStr = PKStr::wStringToUTF8string(this->descs[n]);
	        gtk_file_filter_set_name(GTK_FILE_FILTER(filter), nameStr.c_str());

	        std::string extStr = PKStr::wStringToUTF8string(this->exts[n]);
	        std::vector<std::string> exploded; 
	        PKStr::explode(extStr, L';', &exploded);
	        
	        for(unsigned int i=0; i < exploded.size(); i++)
	        {
    	        gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), exploded[i].c_str());
    	    }
    	    
    	    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
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
    
    CFStringRef messageStr  = NULL;
    CFStringRef filenameStr = NULL;
    
    // Extract filename and folder
    
    std::wstring file = this->filename;
    std::wstring dir  = this->folder;
    
    uint32_t siz = 0;
    UniChar *ptr = NULL;
    
    ptr = NULL;
	PKStr::wStringToUniChar(this->title, &siz, &ptr);
    
    if(ptr != NULL)
    {
        messageStr = CFStringCreateWithCharacters(NULL, ptr, siz);
		myDialogOptions.windowTitle = messageStr;
        delete [] ptr;
    }        
  
    ptr = NULL;
	PKStr::wStringToUniChar(file, &siz, &ptr);
    
    if(ptr != NULL)
    {
        filenameStr = CFStringCreateWithCharacters(NULL, ptr, siz);
        myDialogOptions.saveFileName = filenameStr;
        delete [] ptr;
    }
    
    myDialogOptions.optionFlags = (kNavDefaultNavDlogOptions & !kNavAllowMultipleFiles) | kNavSelectDefaultLocation;
    myDialogOptions.modality    = (kWindowModalityWindowModal); // make it a sheet
    
    NavDialogRef dialogRef;
    
	NavTypeListHandle myOpenResHandle;
	Handle myHandle = NULL;
	OSType theApplicationSignature = 'PKCF';
	OSType typ[4];
	int theNumTypes = 0;
	
	myHandle = NewHandle(sizeof(NavTypeList) + (theNumTypes * sizeof(OSType)));
	
	if (myHandle != NULL) 
	{
		myOpenResHandle  = (NavTypeListHandle) myHandle;
		
		(*myOpenResHandle)->componentSignature = theApplicationSignature;
		(*myOpenResHandle)->osTypeCount = theNumTypes;
		
		BlockMoveData(typ, (*myOpenResHandle)->osType, theNumTypes * sizeof(OSType));
	}
	
	if (myOpenResHandle != NULL) HLock((Handle)myOpenResHandle);
	
	WindowRef window = GetWindowFromPort(parent);
	myDialogOptions.parentWindow = window;
    
	status = NavCreateChooseFileDialog(&myDialogOptions, myOpenResHandle, NavOpenDialogCallback, NULL, NULL, this, &dialogRef);
            
	this->parent = window;
	this->macDialogResult = L"";
	
    status = NavDialogRun(dialogRef);
    
    if(status == noErr)
    {
		// Wait for dialog to complete
		RunAppModalLoopForWindow(window);
		
		// Record result
		result = this->macDialogResult;	
    }
    
    NavDialogDispose(dialogRef);

	if(filenameStr != nil)
	{
		CFRelease(filenameStr);
	}
	
    CFRelease(messageStr);
    
#endif
    
	return result;
}
