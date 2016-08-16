//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKi18n.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFLocale.h>
#endif

#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#endif

// Application support directory

std::wstring PKi18n::appSupportPath = L"./";

void PKi18n::setAppSupportPath(std::wstring path)
{
    PKi18n::appSupportPath = path;
}

std::wstring PKi18n::getAppSupportPath()
{
    return PKi18n::appSupportPath;    
}

//
// Util
//

int _UTF8toUTF16(const unsigned char *utf08, unsigned int length, unsigned short *utf16, unsigned int maxout, bool &hasBOM)
{
    unsigned int cur = 0;
    unsigned int v=0;
    unsigned int o=0;
    unsigned int r=0;
    unsigned int tail;

    // it's da BOM
    if(length >= 3 && (utf08[0] == 0xEF && utf08[1] == 0xBB && utf08[2] == 0xBF))
    {
        v = 3;
        hasBOM = true;
    }
    else
    {
        hasBOM = false;
    }

    while(v<length)
    {
        if(utf08[v] < 0x80)
        {
            cur = utf08[v];
            tail = 0;
        }
        else if(utf08[v] < 0xC0)
        {
            return -1;
        }
        else if(utf08[v] < 0xE0)
        {
            cur = utf08[v] & 0x1F;
            tail = 1;
        }
        else if(utf08[v] < 0xF0)
        {
            cur = utf08[v] & 0x0F;
            tail = 2;
        }
        else if(utf08[v] < 0xF8)
        {
            cur = utf08[v] & 0x07;
            tail = 3;
        }
        else
        {
            return -1;
        }

        ++v;

        for(r=0;r<tail;++r)
        {
            if(v >= length || ((utf08[v] & 0xC0) != 0x80))
                return -1;

            cur = (cur << 6) | (utf08[v] & 0x3F);

            ++v;
        }

        if(cur < 0x10000)
        {
            if(o >= maxout)
                return -1;

            utf16[o++] = cur;
        }
        else if(cur < 0x110000)
        {
            if(o+1 >= maxout)
                return -1;

            cur -= 0x10000;

            utf16[o++] = 0xD800 | (cur >> 10);
            utf16[o++] = 0xDC00 | (cur & 0x03FF);
        }
        else
        {
            return -1;
        }
    }

    return o;
}

int _UTF8ToWchart(const unsigned char *utf08, unsigned int length, wchar_t *wchart, unsigned int maxout, bool &hasBOM)
{
    unsigned short *buffer= new unsigned short[maxout];
    int result = _UTF8toUTF16(utf08, length, buffer,  maxout, hasBOM);

    for(int index = 0; index < result; ++ index)
    {
        wchart[index] = buffer[index];
    }

    delete [] buffer;

    return result;
}

// UTF-16 -> UTF-8
int _UTF16toUTF8(const unsigned short *utf16, unsigned int length, unsigned char *utf08, unsigned int maxout, bool useBOM)
{
    unsigned int cur = 0;
    unsigned int v=0;
    unsigned int o=0;
    signed int r=0;
    signed int bits = 0;

    // it's da BOM
    if(useBOM)
    {
        utf08[o++] = 0xEF;
        utf08[o++] = 0xBB;
        utf08[o++] = 0xBF;
    }

    while(v<length)
    {
        cur = utf16[v++];

        if( (cur & 0xFC00) == 0xD800)
        {
            if( (v < length) && ((utf16[v] & 0xFC00) == 0xDC00))
            {
                cur  = (cur & 0x03FF) << 10;
                cur |= utf16[v] & 0x03FF;
                cur += 0x10000;
            }
            else
            {
                return -1;
            }

            ++v;
        }

        if(o >= maxout)
            return -1;

        if(cur < 0x80)
        {
            utf08[o++] = cur;
            bits = 0;
        }
        else if(cur < 0x800)
        {
            utf08[o++] = (cur >> 6) | 0xC0;
            bits = 6;
        }
        else if(cur < 0x10000)
        {
            utf08[o++] = (cur >> 12) | 0xE0;
            bits = 12;
        }
        else
        {
            utf08[o++] = (cur >> 18) | 0xF0;
            bits = 18;
        }

        for(r=bits;r>0;r-=6)
        {
            if(o >= maxout)
                return -1;

            utf08[o++] = 0x80 | ((cur >> (r-6)) & 0x3F);
        }
    }

    return o;
}

int _WchartToUTF8(const wchar_t *wchart, unsigned int length, unsigned char *utf08, unsigned int maxout, bool useBOM)
{
    unsigned short *buffer = new unsigned short[maxout + 1];

    for(unsigned int index = 0; index < length; ++ index)
    {
        buffer[index] = wchart[index];
    }

    int result = _UTF16toUTF8(buffer, length, utf08, maxout, useBOM);

    delete [] buffer;

    return result;
}


//
// PK_i18n() : Main i18n function
//             Performs string swapping
//             based on the current locale
//

std::wstring PK_i18n(std::wstring source)
{
    PKi18n *i18n = PKi18n::getInstance();

    if(source == L"")
    {
        return L"";
    }

    if(i18n->currentIndex == -1)
    {
        return source;
    }
    else
    {
        uint32_t count = i18n->tables[i18n->currentIndex]->originals.size();

        int32_t min = 0;
        int32_t max = count;

        bool lastRound  = false;
        int32_t current = count/2;

        do
        {
            int result = i18n->tables[i18n->currentIndex]->originals[current].compare(i18n->wStringToString(source));

            if(result == 0)
            {
                if(i18n->tables[i18n->currentIndex]->utf8)
                {
                    return i18n->utf8StringToWstring(i18n->tables[i18n->currentIndex]->translations[current]);
                }
                else // Assume iso-8859-1
                {
                    return i18n->stringToWString(i18n->tables[i18n->currentIndex]->translations[current]);
                }
            }
            else
            {
                if(lastRound)
                {
                    // Not found
                    break;
                }

                if(result > 0)
                {
                    max = current;
                    current = (max + min) /2;

                    if(current == min || current == max)
                    {
                        lastRound = true;
                    }
                }
                else
                {
                    min = current;
                    current = (max + min) /2;

                    if(current == min || current == max)
                    {
                        lastRound = true;
                    }
                }
            }
        }
        while(true);
    }

    return source;
}

//
// PK_noop_i18n() : Marks a strings for inclusion
//                  in the strings table but don't
//                  do anything on it at runtime
//

std::wstring PK_noop_i18n(std::wstring source)
{	
	return source;
}

//
// PK_i18n() : Secondray i18n function
//             Performs string swapping
//             based on a given locale
//

std::wstring PK_i18n(std::wstring source, std::string locale)
{
    PKi18n *i18n = PKi18n::getInstance();

    if(source == L"")
    {
        return L"";
    }

    // Find the index
    std::string currentLocale = i18n->getLocale();
    i18n->setLocale(locale);

    if(i18n->currentIndex == -1)
    {
        i18n->setLocale(currentLocale);
        return source;
    }
    else
    {
        uint32_t count = i18n->tables[i18n->currentIndex]->originals.size();

        int32_t min = 0;
        int32_t max = count;

        bool lastRound  = false;
        int32_t current = count/2;

        do
        {
            int result = i18n->tables[i18n->currentIndex]->originals[current].compare(i18n->wStringToString(source));

            if(result == 0)
            {
                if(i18n->tables[i18n->currentIndex]->utf8)
                {
                    std::wstring result = i18n->utf8StringToWstring(i18n->tables[i18n->currentIndex]->translations[current]);
                    i18n->setLocale(currentLocale);
                    return result;
                }
                else // Assume iso-8859-1
                {
                    std::wstring result = i18n->stringToWString(i18n->tables[i18n->currentIndex]->translations[current]);
                    i18n->setLocale(currentLocale);
                    return result;
                }
            }
            else
            {
                if(lastRound)
                {
                    // Not found
                    break;
                }

                if(result > 0)
                {
                    max = current;
                    current = (max + min) /2;

                    if(current == min || current == max)
                    {
                        lastRound = true;
                    }
                }
                else
                {
                    min = current;
                    current = (max + min) /2;

                    if(current == min || current == max)
                    {
                        lastRound = true;
                    }
                }
            }
        }
        while(true);
    }

    i18n->setLocale(currentLocale);
    return source;
}

//
// PKi18n class
// ------------
//

PKi18n *PKi18n::_instance    = NULL;

//
// Static getter
//

PKi18n *PKi18n::getInstance()
{
    if(PKi18n::_instance == NULL)
    {
        PKi18n::_instance = new PKi18n();
    }

    return PKi18n::_instance;
}

//
// Helpers
//

std::string PKi18n::getSystemLocale()
{
#ifdef WIN32

    std::string language;
    std::string country;

    char lang[9];
    char ctry[9];

    if(GetLocaleInfoA(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT), 
                      LOCALE_SISO639LANGNAME, lang, 9) > 0)
    {
        language = lang;
    }
    
    if(GetLocaleInfoA(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT),
                      LOCALE_SISO3166CTRYNAME, ctry, 9) > 0)
    {
        country = ctry;
    }

    if(language != "" && country != "")
    {
        std::string locale = language + "_" + country;
        return locale;
    }

    return "en_US";

#endif

#ifdef MACOSX

	std::string loc = "en_US";	
	
	CFArrayRef langArray = (CFArrayRef) 
		CFPreferencesCopyAppValue(CFSTR("AppleLanguages"), 
								  kCFPreferencesCurrentApplication);
	
	if(langArray)
	{
		CFStringRef firstLanguage = NULL;
		firstLanguage = (CFStringRef) CFArrayGetValueAtIndex(langArray, 0);

		CFLocaleRef localeRef = CFLocaleCreate(kCFAllocatorDefault, firstLanguage);
		
		if(localeRef)
		{
			CFStringRef lang = (CFStringRef) CFLocaleGetValue(localeRef, kCFLocaleLanguageCode);
			CFStringRef ctry = (CFStringRef) CFLocaleGetValue(localeRef, kCFLocaleCountryCode);
			
			if(lang)
			{
				std::wstring wlang = PKi18n::cfStringToWString(lang);
				std::wstring wctry = wlang; 
			
				if(ctry)
				{
					wctry = PKi18n::cfStringToWString(ctry);
				}
			
				std::wstring wloc = wlang + L"_" + PKi18n::toUpper(wctry);
			
				loc = PKi18n::wStringToString(wloc);
			}
			
			CFRelease(localeRef);
		}
		
		CFRelease(langArray);			
	}	

	return loc;
	
#endif

#ifdef LINUX 

    char *locale = getenv("LANG");

    if(locale == NULL)
	{
		return "en_US";
	}

	int l = strlen(locale);

	if(l < 5)
	{
		return "en_US";
	}

	std::string loc = locale;

	return loc.substr(0,5);

#endif

	return "en_US";
}

//
// Members
//

PKi18n::PKi18n()
{
    this->currentIndex  = -1;
    this->currentLocale = "en_US";
}

PKi18n::~PKi18n()
{
    for(uint32_t i=0; i < this->tables.size(); i++)
    {
        if(this->tables[i])
        {
            delete this->tables[i];
        }
    }

    this->tables.clear();
}

std::string PKi18n::getLocale()
{
    return this->currentLocale;
}

bool PKi18n::setLocale(std::string locale)
{
    //
    // See if it's english
    //

    if(locale == "en_US")
    {
        this->currentIndex  = -1;
        this->currentLocale = locale;

        return true;
    }

    //
    // Let's try to find a perfect match
    //

    for(uint32_t i=0; i < this->tables.size(); i++)
    {
        if(this->tables[i]->locale == locale)
        {
            this->currentIndex  = i;
            this->currentLocale = locale;

            return true;
        }
    }

    //
    // If not, let's find a partial match
    //
    
    for(uint32_t i=0; i < this->tables.size(); i++)
    {
        if(this->tables[i]->locale.substr(0,2) == locale.substr(0, 2))
        {
            this->currentIndex  = i;
            this->currentLocale = locale;

            return true;
        }
    }

    // If still no match, give up

    return false;
}

bool PKi18n::addLocale(std::string locale)
{
#ifdef WIN32

    // Load resource in memory

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);
	HRSRC   res   = FindResourceA(hInst, locale.c_str(), locale.c_str());
	HGLOBAL bfr   = LoadResource(hInst, res);

	if(bfr != NULL)
	{
		int   s    = SizeofResource(hInst, res);
		void *data = LockResource(bfr);
    
        return this->addLocale(locale, data, s);
    }

#endif

#ifdef MACOSX

    CFBundleRef bundle;
    CFURLRef    moURL;
	
    std::string id = locale;
    
    bundle = CFBundleGetMainBundle();
	
    if(bundle != NULL)
    {
		CFRetain(bundle);
		
        const char *idStr = id.c_str();
        CFStringRef str = CFStringCreateWithCString(NULL, 
                                                    idStr, 
                                                    kCFStringEncodingASCII);
        
        moURL = CFBundleCopyResourceURL(bundle, str,
                                        CFSTR("mo"), NULL);
        
        CFRelease(str);
		
        if(moURL != NULL)
        {
            int   MAX_PATH = 32768;
            char *buffer   = new char[MAX_PATH];
			
            if(CFURLGetFileSystemRepresentation(moURL,
                                                true, 
												reinterpret_cast<UInt8 *> (buffer), 
                                                MAX_PATH))
            {
                FILE *file = fopen(buffer, "rb");
				
				if(file == NULL)
				{
					delete [] buffer;
					CFRelease(moURL);
					CFRelease(bundle);
                    return false;
				}
				
                delete [] buffer;
				
                fseek(file, 0, SEEK_END);
                off_t size = ftell(file);
                fseek(file, 0, SEEK_SET);
				
                char *contents = new char[size];
                
                if(contents == NULL)
                {
					fclose(file);
					CFRelease(moURL);
					CFRelease(bundle);
                    return false;
                }
                
                size_t r = fread(contents, 1, size, file);
				
                bool ok = this->addLocale(locale, contents, r);
                
                delete [] contents;
                fclose(file);
                
                if(!ok)
                {
					CFRelease(moURL);
					CFRelease(bundle);
                    return false;
                }
                
				CFRelease(moURL);
				CFRelease(bundle);
                return true;
            }
			
			CFRelease(moURL);
        }
		
		CFRelease(bundle);
    }
	
	return false;	
	
#endif

#ifdef LINUX
	
    std::string path = PKi18n::wStringToUTF8string(PKi18n::getAppSupportPath()) + "/" + locale + ".mo";
    
    FILE *file = fopen(path.c_str(), "rb");
	
	if(file == NULL)
	{
        return false;
	}
		
    fseek(file, 0, SEEK_END);
    off_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
	
    char *contents = new char[size];
    
    if(contents == NULL)
    {
		fclose(file);
        return false;
    }
    
    size_t r = fread(contents, 1, size, file);
	
    bool ok = this->addLocale(locale, contents, r);
    
    delete [] contents;
    fclose(file);
    
    if(!ok)
    {
        return false;
    }
    
    return true;

#endif

    return false;
}

uint32_t PKi18n::getNumberOfAvailableLocales()
{
    return this->tables.size() + 1;
}

std::string PKi18n::getAvailableLocaleAt(uint32_t index)
{
    if(index >= this->tables.size())
    {
        return "en_US";
    }

    return this->tables[index]->locale;
}

std::wstring PKi18n::getLocalizedNameForLocale(std::string locale)
{
    // TODO: add more

    if(locale == "en_US")
    {
        return PK_i18n(L"English (US)", locale);
    }

    if(locale == "fr_FR")
    {
        return PK_i18n(L"French (France)", locale);
    }

    return L"";
}


bool PKi18n::addLocale(std::string locale, void *buffer, uint32_t size)
{
    if(buffer == NULL || size < 28)
    {
        return false;
    }

    uint32_t *header = (uint32_t *) buffer;

    uint32_t magic = header[0];

    uint32_t numstr = header[2];
    uint32_t orioff = header[3];
    uint32_t trsoff = header[4];

    if(magic != 0x950412de) // Not a .mo file
    {
        return false;
    }

    pkLocaleTable *newTable = new pkLocaleTable;

    if(newTable == NULL)
    {
        return false;
    }

    newTable->locale = locale;
    newTable->utf8   = false; 

    for(uint32_t i=0; i < numstr; i++)
    {
        char *strBuffer  = (char *) buffer;

        if(orioff+8*i+4 >= size ||
           trsoff+8*i+4 >= size)
        {
            delete newTable;
            return false;
        }

        uint32_t oLength = *((uint32_t *)&strBuffer[orioff+8*i]);
        uint32_t oOffset = *((uint32_t *)&strBuffer[orioff+8*i+4]);

        uint32_t tLength = *((uint32_t *)&strBuffer[trsoff+8*i]);
        uint32_t tOffset = *((uint32_t *)&strBuffer[trsoff+8*i+4]);

        std::string o = std::string(&strBuffer[oOffset], oLength);
        std::string t = std::string(&strBuffer[tOffset], tLength);

        if(o == "")
        {
            // Skip the first empty entry
            // since we don't need it for swap
            // but look at the charset info

            if((strstr(t.c_str(), "charset=utf-8")) ||
               (strstr(t.c_str(), "charset=UTF-8")))
            {
                newTable->utf8 = true;
            }

            continue;
        }

        newTable->originals.push_back(o);
        newTable->translations.push_back(t);
    }

    this->tables.push_back(newTable);

    return true;
}

std::wstring PKi18n::stringToWString(std::string  src)
{
    std::wstring result(src.size(), 0);

    for(unsigned int i=0; i<src.size(); i++)
    {
        result[i] = (short) (unsigned char) src[i];
    }

    return result;
}

std::string PKi18n::wStringToString(std::wstring src)
{
    std::string result(src.size(), 0);

    for(unsigned int i=0; i<src.size(); i++)
    {
        result[i] = (char) src[i];
    }

    return result;
}

std::string PKi18n::wStringToUTF8string(const std::wstring& w)
{
    bool useBOM = false;
    int maxSize = (w.size() * 3) + 1; // have 1 extra for NULL
	char *utf8 = new char[maxSize];

    int newSize = _WchartToUTF8(w.c_str(), w.size(), (unsigned char *) utf8, maxSize -1, useBOM);

    // end the string properly
    if (newSize >= 0 && newSize < maxSize)
    {
        utf8[newSize] = '\0';
    }

    std::string uStringValue(utf8);

    // clean up
    if (utf8 != NULL)
    {
        delete [] utf8;
    }

    return uStringValue;
}

std::wstring PKi18n::utf8StringToWstring(const std::string& u)
{
    bool hasBOM = false;
    int maxSize = u.size() + 1; // add 1 extra for NULL
    wchar_t *wbuffer = new wchar_t[maxSize];

    int newSize = _UTF8ToWchart((const unsigned char *)u.c_str(), u.size(), wbuffer, maxSize - 1, hasBOM);

    if (newSize >= 0 && newSize < maxSize)
    {
        wbuffer[newSize] = L'\0';
    }

    std::wstring wstringValue(wbuffer);

    // clean up
    if (wbuffer != NULL)
    {
        delete [] wbuffer;
    }

    return wstringValue;
}

std::wstring PKi18n::toUpper(std::wstring str)
{
    const wchar_t *textBytes = str.c_str();
	
    std::wstring upp = L"";
	
    for(unsigned int v=0;v<str.length();v++)
    {
        upp += towupper(textBytes[v]);
    }
	
    return upp;
}


#ifdef MACOSX

std::wstring PKi18n::cfStringToWString(CFStringRef cfStr)
{
    std::wstring result = L"";
	
    CFIndex size = CFStringGetLength(cfStr);
    UniChar *buffer = new UniChar[size];
	
    if(buffer == NULL)
    {
        return result;
    }
	
    CFRange range;
	
    range.location = 0;
    range.length   = size;
	
    CFStringGetCharacters(cfStr, range, buffer);
	
    result = PKi18n::uniCharToWString(buffer, size);
    delete [] buffer;
	
    return result;
}

std::wstring PKi18n::uniCharToWString(UniChar *chars,
									  uint32_t size)
{
    if(chars == NULL || size == 0)
    {
        return L"";
    }
	
    wchar_t *ptr = new wchar_t[size];
	
    if(ptr == NULL)
    {
        return L"";
    }
	
    for(uint32_t i=0; i < size; i++)
    {
        ptr[i] = (wchar_t) chars[i];
    }

    std::wstring newString(ptr, size);
    delete [] ptr;

    return newString;
}


#endif

