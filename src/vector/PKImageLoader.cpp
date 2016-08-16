//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKImageLoader.h"
#include "PKStr.h"
#include "PKPath.h"

PKCritSec PKImageLoader::mutex;

//
// loadBitmap
//

PKImage *PKImageLoader::loadBitmapFromResource(std::wstring bitmapId)
{
	PKCritLock lock(&PKImageLoader::mutex);

#ifdef LINUX

	PKImage *bitmap = new PKImage();

	if(bitmap == NULL)
	{
		// out of memory
		return NULL;
	}

    std::wstring path = PKPath::getAppSupportPath() + 
                        PKPath::getPathSeparator() +
                        bitmapId + L".png";
                       
    PKImage *image = PKImageLoader::loadBitmapFromFilename(path);    

    if(image == NULL)
	{
        path = PKPath::getAppSupportPath() + 
               PKPath::getPathSeparator() +
               PKStr::toUpper(bitmapId) + L".png";	    
	
        image = PKImageLoader::loadBitmapFromFilename(path);    

        if(image == NULL)
    	{
            path = PKPath::getAppSupportPath() + 
                   PKPath::getPathSeparator() +
                   PKStr::toLower(bitmapId) + L".png";	    
	
            image = PKImageLoader::loadBitmapFromFilename(path);    
    	}    
	}    

    return image;

#endif // LINUX


#ifdef WIN32

	PKImage *bitmap = new PKImage();

	if(bitmap == NULL)
	{
		// out of memory
		return NULL;
	}

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery( &dummy, &mbi, sizeof(mbi) );

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);
	HRSRC   res   = FindResource(hInst, bitmapId.c_str(), bitmapId.c_str());
	HGLOBAL bmp   = LoadResource(hInst, res);

	if(bmp != NULL)
	{
		int   s    = SizeofResource(hInst, res);
		void *data = LockResource(bmp);
	
		if(bitmap->load((char *)data, s))
		{
			FreeResource(bmp);
			return bitmap;
		}
		else
		{
			FreeResource(bmp);
			delete bitmap;
			return NULL;
		}
	}
	else
	{
		delete bitmap;
		return NULL;
	}

#endif
    
#ifdef MACOSX

    CFBundleRef bundle;
	
    CFURLRef    bitmapURL;
    CFURLRef    lowBitmapURL;
	CFURLRef    finalURL;

    std::string id  = PKStr::wStringToString(bitmapId);
	std::string lid = PKStr::toLower(id);
    
    bundle = CFBundleGetMainBundle();

    if(bundle != NULL)
    {
		CFRetain(bundle);
		
        const char *idStr = id.c_str();
        CFStringRef str = CFStringCreateWithCString(NULL, 
                                                    idStr, 
                                                    kCFStringEncodingASCII);
        
        bitmapURL = CFBundleCopyResourceURL(bundle, str,
                                            CFSTR("png"), NULL);
        
        CFRelease(str);
    
        const char *lidStr = lid.c_str();
        CFStringRef lstr = CFStringCreateWithCString(NULL, 
                                                    lidStr, 
                                                    kCFStringEncodingASCII);
        
        lowBitmapURL = CFBundleCopyResourceURL(bundle, lstr,
                                               CFSTR("png"), NULL);
        
        CFRelease(lstr);
		
		finalURL = bitmapURL;
		
		if(finalURL == NULL)
		{
			finalURL = lowBitmapURL;
		}
		else
		{
			if(lowBitmapURL != NULL)
			{
				CFRelease(lowBitmapURL);
			}
		}
		
        if(finalURL != NULL)
        {
            int   MAX_PATH = 32768;
            char *buffer   = new char[MAX_PATH];
    
            if(CFURLGetFileSystemRepresentation(finalURL,
                                                true, reinterpret_cast<UInt8 *> (buffer), 
                                                MAX_PATH))
            {
                PKImage *bitmap = new PKImage();
                
                if(bitmap == NULL)
                {
                    // out of memory

					CFRelease(finalURL);
					CFRelease(bundle);
                    delete [] buffer;
                    return NULL;
                }

                FILE *file = fopen(buffer, "rb");
                        
				if(file == NULL)
				{
					delete [] buffer;
					CFRelease(finalURL);
					CFRelease(bundle);
					delete bitmap;
					return NULL;
				}
						
                delete [] buffer;

                fseek(file, 0, SEEK_END);
                off_t size = ftell(file);
                fseek(file, 0, SEEK_SET);

                char *contents = new char[size];
                
                if(contents == NULL)
                {
					fclose(file);
					CFRelease(finalURL);
					CFRelease(bundle);
                    delete bitmap;
                    return NULL;
                }
                
                fread(contents, 1, size, file);
        
                bool ok = bitmap->load(contents, size);
                
                delete [] contents;
                fclose(file);
                
                if(!ok)
                {
					CFRelease(finalURL);
					CFRelease(bundle);
                    delete bitmap;                
                    return NULL;
                }
                
				CFRelease(finalURL);
				CFRelease(bundle);
                return bitmap;
            }
			
			CFRelease(finalURL);
        }
		
		CFRelease(bundle);
    }
    
#endif

	return NULL;
}

PKImage *PKImageLoader::loadBitmapFromFilename(std::wstring filename)
{
	PKImage *image = new PKImage();

	if(!image->load(filename))
	{
		delete image;
		return NULL;
	}

	return image;
}
