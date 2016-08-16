//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKPersistTool.h"
#include "PKStr.h"
#include "PKOS.h"
#include "PKPath.h"

#ifdef LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include "UTFConv.h"
#define MAX_LINE 32768
#endif

PKCritSec PKPersistTool::mutex;

//
// PKPersistTool
//

PKPersistTool::PKPersistTool(std::string appName)
{
#ifdef WIN32

	DWORD result = 0;

	this->key = NULL;

	LONG res = 0;

    std::wstring keyName = L"Software\\KetchupApps\\" + 
                           PKStr::stringToWString(appName);


	res = RegCreateKeyEx(HKEY_CURRENT_USER,
						 keyName.c_str(),
						 0, NULL, REG_OPTION_NON_VOLATILE, 
						 KEY_WRITE|KEY_READ, NULL,
						 &(this->key), &result);

    if(this->key == NULL)
    {
        if(PKOS::getWindowsVersion() == WIN_VISTA || 
           PKOS::getWindowsVersion() == WIN_7)
	    {
		    // On Vista/7 we might be running in
		    // low integrity processes so we must
		    // write preferences in a place in
		    // the registry that we are sure will
		    // always be granted availability...

		    keyName = L"Software\\AppDataLow\\Software\\KetchupApps\\" + 
                      PKStr::stringToWString(appName);

            res = RegCreateKeyEx(HKEY_CURRENT_USER,
						         keyName.c_str(),
						         0, NULL, REG_OPTION_NON_VOLATILE, 
						         KEY_WRITE|KEY_READ, NULL,
						         &(this->key), &result);
	    }
    }

#endif
    
#ifdef MACOSX
    
    this->appID = CFStringCreateWithCString(NULL, appName.c_str(), kCFStringEncodingASCII);
    
#endif

#ifdef LINUX

    char *home = getenv("HOME");

    if(home == NULL)
    {
        return;
    }

    std::string directory = home;
    directory += ("/." + appName);

    this->filename = directory + "/preferences";

    if(!PKPath::directoryExists(directory))
    {
        mkdir(directory.c_str(), 0x1ed);
    }   

#endif

}

PKPersistTool::~PKPersistTool()
{
#ifdef WIN32

	if(this->key != NULL)
	{
		RegCloseKey(this->key);
		this->key = NULL;
	}

#endif

#ifdef MACOSX
	CFRelease(this->appID);
#endif
}

void PKPersistTool::setBool(std::string name, bool value)
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	DWORD b = value ? 1 : 0;

	if(this->key != NULL)
	{
		std::wstring wname = PKStr::stringToWString(name);

		RegSetValueEx(this->key, 
					  wname.c_str(), 
					  0, REG_DWORD, (BYTE *) &b, 
					  sizeof(DWORD));
	}

#endif
   
#ifdef MACOSX
    
    CFStringRef key  = CFStringCreateWithCString(NULL, 
                                                 name.c_str(), 
                                                 kCFStringEncodingASCII);
    
    CFBooleanRef data = (value ? kCFBooleanTrue : kCFBooleanFalse);
        
    // Set up the preference.
    CFPreferencesSetValue(key, data,
                          this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
    
    // Write out the preference data.
    CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    
    
    CFRelease(key);
    CFRelease(data);
    
#endif

#ifdef LINUX

    // Read the preferences

    bool found = false;
    std::vector <std::string> lines;

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs != NULL)
    {
        char buffer[MAX_LINE];

        while(fgets(buffer, MAX_LINE, prefs) != NULL)   
        {
            std::string line = buffer;
            std::vector<std::string> items;

            line = line.substr(0, line.length()-1);

	    PKStr::explode(line, ':', &items);

            if(items.size() == 3)
            {
                if(items[0] == "bool" &&
                   items[1] == name)
                {
                    found = true;
                    std::string newline = "bool:" + name + ":" + 
                                          PKStr::boolToString(value);
                    lines.push_back(newline);
                }
                else
                {   
                    lines.push_back(line);
                }
            }
        }

        fclose(prefs);
    }

    if(!found)
    {
        // Add it at the end of the list

        std::string newline = "bool:" + name + ":" + 
                              PKStr::boolToString(value);
        lines.push_back(newline);
    }

    // Write the file back

    prefs = fopen(this->filename.c_str(), "w");

    if(prefs != NULL)
    {
        for(unsigned int i=0; i < lines.size(); i++)
        {
            fprintf(prefs, "%s\n", lines[i].c_str());       
        }

        fclose(prefs);
    }

#endif
}

bool PKPersistTool::getInt32(std::string name, int32_t *value)	
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->key != NULL)
	{
		std::wstring wname = PKStr::stringToWString(name);

		DWORD type = REG_DWORD;
		DWORD data = 0;
		DWORD size = sizeof(DWORD);

		if(RegQueryValueEx(this->key, wname.c_str(),
						   NULL, &type, 
						   (BYTE *) &data, &size) == ERROR_SUCCESS)
		{
			if(type == REG_DWORD)
			{
				*value = data;
				return true;
			}
		}
	}

#endif
    
#ifdef MACOSX

	// Read latest preferences
    CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    

	// Create key
    CFStringRef key = CFStringCreateWithCString(NULL, 
                                                name.c_str(), 
                                                kCFStringEncodingASCII);    
    CFPropertyListRef data;
    
    // Read the preference.
    data = CFPreferencesCopyValue(key,
                                  this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
    
    CFRelease(key);
    
    if(data != NULL)
    {
        if(CFGetTypeID(data) == CFNumberGetTypeID())
        {
            CFNumberRef n = (CFNumberRef) data;
            
            CFNumberGetValue(n, kCFNumberIntType, value);
            
            CFRelease(data);    
            return true;
        }

        CFRelease(data);    
    }    
    
#endif

#ifdef LINUX

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs == NULL)
    {
        return false;
    }

    char buffer[MAX_LINE];
    bool found = false;

    while(fgets(buffer, MAX_LINE, prefs) != NULL)
    {
        std::string line = buffer;
        std::vector<std::string> items;

        line = line.substr(0, line.length()-1);

        PKStr::explode(line, ':', &items);

        if(items.size() == 3)
        {
            if(items[0] == "int" &&
               items[1] == name)
            {
                found = true;
                *value = PKStr::stringToInt32(items[2]);
            }   
        }
    }

    fclose(prefs);
    return found;

#endif

	return false;
}

void PKPersistTool::setInt32(std::string name, int32_t value)
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	DWORD b = value;

	if(this->key != NULL)
	{
		std::wstring wname = PKStr::stringToWString(name);

		RegSetValueEx(this->key, 
					  wname.c_str(), 
					  0, REG_DWORD, (BYTE *) &b, 
					  sizeof(DWORD));
	}

#endif
   
#ifdef MACOSX
    
    CFStringRef key  = CFStringCreateWithCString(NULL, 
                                                 name.c_str(), 
                                                 kCFStringEncodingASCII);
    
    CFNumberRef data = CFNumberCreate(NULL, kCFNumberIntType, &value);
        
    // Set up the preference.
    CFPreferencesSetValue(key, data,
                          this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
    
    // Write out the preference data.
    CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    
    
    CFRelease(key);
    CFRelease(data);
    
#endif

#ifdef LINUX

    // Read the preferences

    bool found = false;
    std::vector <std::string> lines;

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs != NULL)
    {
        char buffer[MAX_LINE];

        while(fgets(buffer, MAX_LINE, prefs) != NULL)   
        {
            std::string line = buffer;
            std::vector<std::string> items;

            line = line.substr(0, line.length()-1);

            PKStr::explode(line, ':', &items);

            if(items.size() == 3)
            {
                if(items[0] == "int" &&
                   items[1] == name)
                {
                    found = true;
                    std::string newline = "int:" + name + ":" + 
                                          PKStr::int32ToString(value);
                    lines.push_back(newline);
                }
                else
                {   
                    lines.push_back(line);
                }
            }
        }

        fclose(prefs);
    }

    if(!found)
    {
        // Add it at the end of the list

        std::string newline = "int:" + name + ":" + 
                              PKStr::int32ToString(value);
        lines.push_back(newline);
    }

    // Write the file back

    prefs = fopen(this->filename.c_str(), "w");

    if(prefs != NULL)
    {
        for(unsigned int i=0; i < lines.size(); i++)
        {
            fprintf(prefs, "%s\n", lines[i].c_str());       
        }

        fclose(prefs);
    }

#endif
}

bool PKPersistTool::getBool(std::string name, bool *value)	
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->key != NULL)
	{
		std::wstring wname = PKStr::stringToWString(name);

		DWORD type = REG_DWORD;
		DWORD data = 0;
		DWORD size = sizeof(DWORD);

		if(RegQueryValueEx(this->key, wname.c_str(),
						   NULL, &type, 
						   (BYTE *) &data, &size) == ERROR_SUCCESS)
		{
			if(type == REG_DWORD)
			{
				*value = (data == 0 ? false : true);
				return true;
			}
		}
	}

#endif
    
#ifdef MACOSX

	// Read latest preferences
    CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    

	// Create key
    CFStringRef key = CFStringCreateWithCString(NULL, 
                                                name.c_str(), 
                                                kCFStringEncodingASCII);    
    CFPropertyListRef data;
    
    // Read the preference.
    data = CFPreferencesCopyValue(key,
                                  this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
    
    CFRelease(key);
    
    if(data != NULL)
    {
        if(CFGetTypeID(data) == CFBooleanGetTypeID())
        {
            CFBooleanRef b = (CFBooleanRef) data;
            
            *value = CFBooleanGetValue(b);
            
            CFRelease(data);    
            return true;
        }

        CFRelease(data);    
    }    
    
#endif

#ifdef LINUX

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs == NULL)
    {
        return false;
    }

    char buffer[MAX_LINE];
    bool found = false;

    while(fgets(buffer, MAX_LINE, prefs) != NULL)
    {
        std::string line = buffer;
        std::vector<std::string> items;

        line = line.substr(0, line.length()-1);

        PKStr::explode(line, ':', &items);

        if(items.size() == 3)
        {
            if(items[0] == "bool" &&
               items[1] == name)
            {
                found = true;
                *value = PKStr::stringToBool(items[2]);
            }   
        }
    }

    fclose(prefs);
    return found;

#endif

	return false;
}

void PKPersistTool::setString(std::string name, std::string value)
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->key != NULL)
	{
		std::wstring wname  = PKStr::stringToWString(name);
        std::wstring wvalue = PKStr::stringToWString(value);
 
		RegSetValueEx(this->key, 
					  wname.c_str(), 
					  0, REG_SZ, (BYTE *) wvalue.c_str(), 
					  sizeof(unsigned short)*(wvalue.size() + 1));
	}
	
#endif

#ifdef MACOSX
        
    CFStringRef key  = CFStringCreateWithCString(NULL, 
                                                 name.c_str(), 
                                                 kCFStringEncodingASCII);
    
    uint32_t size;
    UniChar *ptr;
    
	std::wstring wvalue = PKStr::stringToWString(value);
	
	PKStr::wStringToUniChar(wvalue, &size, &ptr);
    
    if(ptr != NULL)
    {
        CFStringRef data = CFStringCreateWithCharacters(NULL,
                                                        ptr, 
                                                        size);
        // Set up the preference.
        CFPreferencesSetValue(key, data,
                              this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
        
        // Write out the preference data.
        CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    
        
        delete [] ptr;
        CFRelease(data);
    }
        
    CFRelease(key);
    
#endif

#ifdef LINUX

    // Read the preferences

    bool found = false;
    std::vector <std::string> lines;

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs != NULL)
    {
        char buffer[MAX_LINE];

        while(fgets(buffer, MAX_LINE, prefs) != NULL)   
        {
            std::string line = buffer;
            std::vector<std::string> items;

            line = line.substr(0, line.length()-1);

            PKStr::explode(line, ':', &items);

            if(items.size() == 3)
            {
                if(items[0] == "wstring" &&
                   items[1] == name)
                {
                    found = true;

                    unsigned int maxout = value.length()*5;
                    unsigned char *out  = new unsigned char[maxout];    

                    if(out != NULL)
                    {
                        std::string newline = "wstring:" + name + ":" + value;
                        lines.push_back(newline);

                        delete [] out;
                    }
                }
                else
                {   
                    lines.push_back(line);
                }
            }
        }

        fclose(prefs);
    }

    if(!found)
    {
        // Add it at the end of the list

        unsigned int maxout = value.length()*5;
        unsigned char *out  = new unsigned char[maxout];    

        if(out != NULL)
        {
            std::string newline = "wstring:" + name + ":" + value;
            lines.push_back(newline);

            delete [] out;
        }
    }

    // Write the file back

    prefs = fopen(this->filename.c_str(), "w");

    if(prefs != NULL)
    {
        for(unsigned int i=0; i < lines.size(); i++)
        {
            fprintf(prefs, "%s\n", lines[i].c_str());       
        }

        fclose(prefs);
    }

#endif
}

bool PKPersistTool::getString(std::string name, std::string &value)
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->key != NULL)
	{
		std::wstring wname = PKStr::stringToWString(name);

		DWORD type = REG_SZ;
		BYTE  data[2 * 32768];
		DWORD size = 2 * 32768;

		HRESULT hr;

		if((hr = RegQueryValueEx(this->key, wname.c_str(),
						   NULL, &type, 
						   (BYTE *) data, &size)) == ERROR_SUCCESS)
		{
			if(type == REG_SZ)
			{
                std::wstring wvalue = std::wstring((wchar_t *) data);
                value = PKStr::wStringToString(wvalue);

				return true;
			}
		}
	}

#endif
    
#ifdef MACOSX
        
	// Read latest preferences
    CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    

	// Create key
    CFStringRef key = CFStringCreateWithCString(NULL, 
                                                name.c_str(), 
                                                kCFStringEncodingASCII);    
    CFPropertyListRef data;
    
    // Read the preference.
    data = CFPreferencesCopyValue(key,
                                  this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
    
    CFRelease(key);
    
    if(data != NULL)
    {
        if(CFGetTypeID(data) == CFStringGetTypeID())
        {            
            CFStringRef str = (CFStringRef) data;

            int length = CFStringGetLength(str);
            
            UniChar *buffer = new UniChar[length+1];

            if(buffer != NULL)
            {
                memset(buffer, 0, sizeof(UniChar)*(length+1));
                
                CFStringGetCString(str, (char *) buffer, 
                                   sizeof(UniChar)*(length+1), 
                                   kCFStringEncodingUnicode);
                                
				std::wstring wvalue = PKStr::uniCharToWString(buffer, length);
				value = PKStr::wStringToString(wvalue);
                
                delete [] buffer;
                CFRelease(str);    
                
                return true;
            }
        }

        CFRelease(data);    
    }
    
#endif

#ifdef LINUX

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs == NULL)
    {
        return false;
    }

    char buffer[MAX_LINE];
    bool found = false;

    while(fgets(buffer, MAX_LINE, prefs) != NULL)
    {
        std::string line = buffer;
        std::vector<std::string> items;

        line = line.substr(0, line.length()-1);

        PKStr::explode(line, ':', &items);

        if(items.size() == 3)
        {
            if(items[0] == "wstring" &&
               items[1] == name)
            {
                std::string utf8str = items[2];                
                found = true;
                    
                value = utf8str;
            }   
        }
    }

    fclose(prefs);
    return found;

#endif


	return false;
}

void PKPersistTool::setWString(std::string name, std::wstring value)
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->key != NULL)
	{
		std::wstring wname = PKStr::stringToWString(name);

		RegSetValueEx(this->key, 
					  wname.c_str(), 
					  0, REG_SZ, (BYTE *) value.c_str(), 
					  sizeof(unsigned short)*(value.size() + 1));
	}
	
#endif

#ifdef MACOSX
        
    CFStringRef key  = CFStringCreateWithCString(NULL, 
                                                 name.c_str(), 
                                                 kCFStringEncodingASCII);
    
    uint32_t size;
    UniChar *ptr;
    
    PKStr::wStringToUniChar(value, &size, &ptr);
    
    if(ptr != NULL)
    {
        CFStringRef data = CFStringCreateWithCharacters(NULL,
                                                        ptr, 
                                                        size);
        // Set up the preference.
        CFPreferencesSetValue(key, data,
                              this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
        
        // Write out the preference data.
        CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    
        
        delete [] ptr;
        CFRelease(data);
    }
        
    CFRelease(key);
    
#endif

#ifdef LINUX

    // Read the preferences

    bool found = false;
    std::vector <std::string> lines;

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs != NULL)
    {
        char buffer[MAX_LINE];

        while(fgets(buffer, MAX_LINE, prefs) != NULL)   
        {
            std::string line = buffer;
            std::vector<std::string> items;

            line = line.substr(0, line.length()-1);

            PKStr::explode(line, ':', &items);

            if(items.size() == 3)
            {
                if(items[0] == "wstring" &&
                   items[1] == name)
                {
                    found = true;

                    unsigned int maxout = value.length()*5;
                    unsigned char *out  = new unsigned char[maxout];    

                    if(out != NULL)
                    {
                        WchartToUTF8(value.c_str(), value.length(), out, maxout, true);
                        std::string svalue = (const char *) out;

                        std::string newline = "wstring:" + name + ":" + svalue;
                        lines.push_back(newline);

                        delete [] out;
                    }
                }
                else
                {   
                    lines.push_back(line);
                }
            }
        }

        fclose(prefs);
    }

    if(!found)
    {
        // Add it at the end of the list

        unsigned int maxout = value.length()*5;
        unsigned char *out  = new unsigned char[maxout];    

        if(out != NULL)
        {
            int outsize = WchartToUTF8(value.c_str(), value.length(), out, maxout, false);

            if(outsize != -1)
            {
                std::string svalue = std::string((const char *) out, outsize);

                std::string newline = "wstring:" + name + ":" + svalue;
                lines.push_back(newline);
            }

            delete [] out;
        }
    }

    // Write the file back

    prefs = fopen(this->filename.c_str(), "w");

    if(prefs != NULL)
    {
        for(unsigned int i=0; i < lines.size(); i++)
        {
            fprintf(prefs, "%s\n", lines[i].c_str());       
        }

        fclose(prefs);
    }

#endif
}

bool PKPersistTool::getWString(std::string name, std::wstring &value)
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->key != NULL)
	{
		std::wstring wname = PKStr::stringToWString(name);

		DWORD type = REG_SZ;
		BYTE  data[2 * 32768];
		DWORD size = 2 * 32768;

		HRESULT hr;

		if((hr = RegQueryValueEx(this->key, wname.c_str(),
						   NULL, &type, 
						   (BYTE *) data, &size)) == ERROR_SUCCESS)
		{
			if(type == REG_SZ)
			{
				value = std::wstring((wchar_t *) data);
				return true;
			}
		}
	}

#endif
    
#ifdef MACOSX
        
	// Read latest preferences
    CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    

	// Create key
    CFStringRef key = CFStringCreateWithCString(NULL, 
                                                name.c_str(), 
                                                kCFStringEncodingASCII);    
    CFPropertyListRef data;
    
    // Read the preference.
    data = CFPreferencesCopyValue(key,
                                  this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
    
    CFRelease(key);
    
    if(data != NULL)
    {
        if(CFGetTypeID(data) == CFStringGetTypeID())
        {            
            CFStringRef str = (CFStringRef) data;

            int length = CFStringGetLength(str);
            
            UniChar *buffer = new UniChar[length+1];

            if(buffer != NULL)
            {
                memset(buffer, 0, sizeof(UniChar)*(length+1));
                
                CFStringGetCString(str, (char *) buffer, 
                                   sizeof(UniChar)*(length+1), 
                                   kCFStringEncodingUnicode);
                                
                value = PKStr::uniCharToWString(buffer, length);
                
                delete [] buffer;
                CFRelease(str);    
                
                return true;
            }
        }

        CFRelease(data);    
    }
    
#endif

#ifdef LINUX

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs == NULL)
    {
        return false;
    }

    char buffer[MAX_LINE];
    bool found = false;

    while(fgets(buffer, MAX_LINE, prefs) != NULL)
    {
        std::string line = buffer;
        std::vector<std::string> items;

        line = line.substr(0, line.length()-1);

        PKStr::explode(line, ':', &items);

        if(items.size() == 3)
        {
            if(items[0] == "wstring" &&
               items[1] == name)
            {
                std::string utf8str = items[2];
                
                wchar_t *out = new wchar_t[utf8str.length()];

                if(out != NULL)
                {
                    found = true;
                    
                    bool hasBOM;
                    int outlength = UTF8ToWchart((const unsigned char *) utf8str.c_str(), 
                                                        utf8str.length(), 
                                                        out, 
                                                        utf8str.length(), hasBOM);

                    if(outlength != -1)
                    {
                        value = std::wstring(out, outlength);
                    }

                    delete [] out;
                }
            }   
        }
    }

    fclose(prefs);
    return found;

#endif


	return false;
}

void PKPersistTool::removeValue(std::string name)
{
	PKCritLock lock(&this->mutex);

#ifdef WIN32

	if(this->key != NULL)
	{
		std::wstring keyName = PKStr::stringToWString(name);
		RegDeleteValue(this->key, keyName.c_str());
	}

#endif


#ifdef MACOSX

    CFStringRef key  = CFStringCreateWithCString(NULL, 
                                                 name.c_str(), 
                                                 kCFStringEncodingASCII);
    
    // Remove the preference.
    CFPreferencesSetValue(key, NULL, // NULL removes it
                          this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);
    
    // Write out the preference data.
    CFPreferencesSynchronize(this->appID,
						  kCFPreferencesCurrentUser,
						  kCFPreferencesCurrentHost);    
    
    CFRelease(key);


#endif

#ifdef LINUX

    // Read the preferences

    bool found = false;
    std::vector <std::string> lines;

    FILE *prefs = fopen(this->filename.c_str(), "r");

    if(prefs != NULL)
    {
        char buffer[MAX_LINE];

        while(fgets(buffer, MAX_LINE, prefs) != NULL)   
        {
            std::string line = buffer;
            std::vector<std::string> items;

            line = line.substr(0, line.length()-1);

            PKStr::explode(line, ':', &items);

            if(items.size() == 3)
            {
                if(items[1] == name)
                {
                    found = true;
                }
                else
                {   
                    lines.push_back(line);
                }
            }
        }

        fclose(prefs);
    }

    if(!found)
    {
		return;
    }

    // Write the file back

    prefs = fopen(this->filename.c_str(), "w");

    if(prefs != NULL)
    {
        for(unsigned int i=0; i < lines.size(); i++)
        {
            fprintf(prefs, "%s\n", lines[i].c_str());       
        }

        fclose(prefs);
    }


#endif

}
