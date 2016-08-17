//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKStr.h"

#ifdef WIN32
#include <mbstring.h>
#include <Windows.h>
#endif

#include "UTFConv.h"

#if(defined(LINUX) || defined(MACOSX))
#include <wctype.h>
#include <stdlib.h>
#if(defined(MACOSX))
#include <CoreFoundation/CoreFoundation.h>
#endif
#endif

std::wstring PKStr::stringToWString(std::string  src)
{
    std::wstring result(src.size(), 0);

    for(unsigned int i=0; i<src.size(); i++)
    {
        result[i] = (short) (unsigned char) src[i];
    }

    return result;
}

std::string PKStr::wStringToString(std::wstring src)
{
    std::string result(src.size(), 0);

    for(unsigned int i=0; i<src.size(); i++)
    {
        result[i] = (char) src[i];
    }

    return result;
}

std::string PKStr::toLower(std::string str)
{
    const char *textBytes = str.c_str();

    std::string lower = "";

    for(unsigned int v=0;v<str.length();v++)
    {
        lower += tolower(textBytes[v]);
    }

    return lower;
}

std::wstring PKStr::toLower(std::wstring str)
{
    const wchar_t *textBytes = str.c_str();

    std::wstring lower = L"";

    for(unsigned int v=0;v<str.length();v++)
    {
        lower += towlower(textBytes[v]);
    }

    return lower;
}

std::string PKStr::toUpper(std::string str)
{
    const char *textBytes = str.c_str();

    std::string upp = "";

    for(unsigned int v=0;v<str.length();v++)
    {
        upp += toupper(textBytes[v]);
    }

    return upp;
}

std::wstring PKStr::toUpper(std::wstring str)
{
    const wchar_t *textBytes = str.c_str();

    std::wstring upp = L"";

    for(unsigned int v=0;v<str.length();v++)
    {
        upp += towupper(textBytes[v]);
    }

    return upp;
}

int PKStr::findFirstOf(std::wstring target,
                             std::wstring toFind)
{
    return target.find(toFind);
}

int PKStr::findFirstOf(std::string target,
                             std::string toFind)
{
    return target.find(toFind);
}

std::wstring PKStr::replaceChar(wchar_t find,
                                      wchar_t replace,
                                      std::wstring str)
{
    std::wstring result;
    std::wstring rep(1, replace);

    for(unsigned int i=0; i < str.size(); i++)
    {
        if(str[i] == find)
        {
            result.append(rep);
        }
        else
        {
            result.append(str.substr(i, 1));
        }
    }

    return result;
}

std::string PKStr::stripBeginingAndEndChar(std::string stringToStrip,
                                                 char stripChar)
{
    std::string strippedString = stringToStrip;

    while((strippedString.length() > 0) &&
          (strippedString[0] == stripChar))
    {
        strippedString = strippedString.erase(0, 1);
    }

    while((strippedString.length() > 0) &&
          (strippedString[strippedString.length() - 1] == stripChar))
    {
        strippedString = strippedString.erase(strippedString.length() - 1, 1);
    }


    return strippedString;
}

std::wstring PKStr::stripBeginingAndEndChar(std::wstring stringToStrip,
                                                  wchar_t stripChar)
{
    std::wstring strippedString = stringToStrip;

    while((strippedString.length() > 0) &&
          (strippedString[0] == stripChar))
    {
        strippedString = strippedString.erase(0, 1);
    }

    while((strippedString.length() > 0) &&
          (strippedString[strippedString.length() - 1] == stripChar))
    {
        strippedString = strippedString.erase(strippedString.length() - 1, 1);
    }


    return strippedString;
}

std::wstring PKStr::replaceString(std::wstring find,
                                  std::wstring replace,
                                  std::wstring str)
{
    std::wstring result = L"";

    while(true)
    {
        int pos = PKStr::findFirstOf(str, find);

        if(pos != std::string::npos)
        {
			std::wstring sub = str.substr(0, pos+find.size());
            result += sub.replace(pos, find.size(), replace);
       
			str = str.substr(pos+find.size());
		}
        else
        {
			result += str;
            break;
        }
    }

    return result;
}

std::string PKStr::replaceString(std::string find,
                                 std::string replace,
                                 std::string str)
{
    std::string result = "";

    while(true)
    {
        int pos = PKStr::findFirstOf(str, find);

        if(pos != std::string::npos)
        {
			std::string sub = str.substr(0, pos+find.size());
            result += sub.replace(pos, find.size(), replace);
       
			str = str.substr(pos+find.size());
		}
        else
        {
			result += str;
            break;
        }
    }

    return result;
}

std::string PKStr::escapeSingleQuotes(std::string input)
{
	return PKStr::replaceString(input, "'", "\\'");
}

std::wstring PKStr::escapeSingleQuotes(std::wstring input)
{
	return PKStr::replaceString(input, L"'", L"\\'");
}

void PKStr::explode(std::wstring str,
                          wchar_t delimiter,
                          std::vector <std::wstring> *result)
{
    std::wstring s = str;

    int pos = s.find_first_of(delimiter);

    while(pos != std::wstring::npos)
    {
        if(pos != 0)
        {
            result->push_back(s.substr(0, pos));
        }

        s = s.substr(pos+1);
        pos = s.find_first_of(delimiter);
    }

    result->push_back(s);
}

void PKStr::explode(std::string str,
                          char delimiter,
                          std::vector <std::string> *result)
{
    std::string s = str;

    int pos = s.find_first_of(delimiter);

    while(pos != std::string::npos)
    {
        if(pos != 0)
        {
            result->push_back(s.substr(0, pos));
        }

        s = s.substr(pos+1);
        pos = s.find_first_of(delimiter);
    }

    result->push_back(s);
}

bool PKStr::isOnlyWhiteSpaceAndTabs(std::string input)
{
    for(unsigned int i=0; i < input.size(); i++)
    {
        if( (input[i] != ' ' ) &&
            (input[i] != '\t') &&
            (input[i] != '\r') &&
            (input[i] != '\n'))
        {
            return false;
        }
    }

    return true;
}

bool PKStr::isOnlyWhiteSpaceAndTabs(std::wstring input)
{
    for(unsigned int i=0; i < input.size(); i++)
    {
        if( (input[i] != L' ' ) &&
            (input[i] != L'\t') &&
            (input[i] != L'\r') &&
            (input[i] != L'\n'))
        {
            return false;
        }
    }

    return true;
}

std::string PKStr::boolToString(bool inVal)
{
    std::string response = "false";

    if(true == inVal)
    {
        response = "true";
    }

    return response;
}

bool PKStr::stringToBool(std::string inVal)
{
    bool outVal = false;

    inVal = PKStr::toLower(inVal);

    if(inVal == "true")
    {
        outVal = true;
    }
    else if(inVal == "false")
    {
        outVal = false;
    }

    return outVal;
}

std::string PKStr::uint32ToString(uint32_t inVal)
{
    std::string outVal;
    char buffer[64];

#ifdef WIN32
    sprintf_s(buffer, 64, "%u", inVal);
#else
    sprintf(buffer, "%u", inVal);
#endif

    outVal = buffer;
    return outVal;
}

uint32_t PKStr::stringToUInt32(std::string inVal)
{
    uint32_t outVal;
    outVal = atoi(inVal.c_str());
    return outVal;
}

std::string PKStr::int32ToString(int32_t inVal)
{
    std::string outVal;
    char buffer[64];

#ifdef WIN32
    sprintf_s(buffer, 64, "%d",inVal);
#else
    sprintf(buffer, "%d", inVal);
#endif

    outVal = buffer;
    return outVal;
}

int32_t PKStr::stringToInt32(std::string inVal)
{
    int32_t outVal;
    outVal = atoi(inVal.c_str());
    return outVal;
}

std::string PKStr::doubleToString(double inVal, int precision)
{
    std::string outVal;
    char buffer[64];

    if(precision == 1)
    {
#ifdef WIN32
    sprintf_s(buffer,64,"%.1f",inVal);
#else
    sprintf(buffer, "%.1f", inVal);
#endif
    }
    else if(precision == 2)
    {
#ifdef WIN32
    sprintf_s(buffer,64,"%.2f",inVal);
#else
    sprintf(buffer, "%.2f", inVal);
#endif
    }
    else if(precision == 3)
    {
#ifdef WIN32
    sprintf_s(buffer,64,"%.3f",inVal);
#else
    sprintf(buffer, "%.3f", inVal);
#endif
    }
    else
    {
#ifdef WIN32
    sprintf_s(buffer,64,"%f",inVal);
#else
    sprintf(buffer, "%f", inVal);
#endif
    }

    outVal = buffer;
    return outVal;
}

double PKStr::stringToDouble(std::string inVal)
{
    double outVal;
    outVal = atof(inVal.c_str());
    return outVal;
}

std::string PKStr::floatToString(float inVal)
{
    std::string outVal;
    char buffer[64];

#ifdef WIN32
    sprintf_s(buffer,64,"%f",inVal);
#else
    sprintf(buffer, "%f", inVal);
#endif

    outVal = buffer;
    return outVal;
}

float PKStr::stringToFloat(std::string inVal)
{
    float outVal;

#ifdef WIN32
    int status = sscanf_s(inVal.c_str(),"%f",&outVal);
#else
    int status = sscanf(inVal.c_str(),"%f",&outVal);
#endif

    if(status == EOF)
    {
        return 0;
    }

    return outVal;
}

std::string PKStr::uint64ToString(uint64_t inVal)
{
    std::string outVal;
    char buffer[64];

#ifdef WIN32
    sprintf_s(buffer, 64, "%I64u", inVal);
#else
    sprintf(buffer, "%llu", inVal);
#endif

    outVal = buffer;
    return outVal;
}

uint64_t PKStr::stringToUint64(std::string inVal)
{
    uint64_t outVal;
#ifdef WIN32
    sscanf_s(inVal.c_str(), "%I64u", &outVal);
#else
    sscanf(inVal.c_str(), "%llu", &outVal);
#endif
    return outVal;
}

std::string PKStr::int64ToString(int64_t inVal)
{
    std::string outVal;
    char buffer[64];

#ifdef WIN32
    sprintf_s(buffer, 64, "%I64d", inVal);
#else
    sprintf(buffer, "%lld", inVal);
#endif

    outVal = buffer;
    return outVal;
}

int64_t PKStr::stringToInt64(std::string inVal)
{
    int64_t outVal;
#ifdef WIN32
    sscanf_s(inVal.c_str(), "%I64d", &outVal);
#else
    sscanf(inVal.c_str(), "%lld", &outVal);
#endif
    return outVal;
}

std::wstring PKStr::boolToWstring(bool inVal)
{
    std::string theString = PKStr::boolToString(inVal);
    return PKStr::stringToWString(theString);
}

bool PKStr::wstringToBool(std::wstring inVal)
{
    std::string theString = PKStr::wStringToString(inVal);
    return PKStr::stringToBool(theString);
}

std::wstring PKStr::uint32ToWstring(uint32_t inVal)
{
    std::string theString = PKStr::uint32ToString(inVal);
    return PKStr::stringToWString(theString);
}

uint32_t PKStr::wstringToUInt32(std::wstring inVal)
{
    std::string theString = PKStr::wStringToString(inVal);
    return PKStr::stringToUInt32(theString);
}

std::wstring PKStr::int32ToWstring(int32_t inVal)
{
    std::string theString = PKStr::int32ToString(inVal);
    return PKStr::stringToWString(theString);
}

int32_t PKStr::wstringToInt32(std::wstring inVal)
{
    std::string theString = PKStr::wStringToString(inVal);
    return PKStr::stringToInt32(theString);
}

std::wstring PKStr::doubleToWstring(double inVal, int precision)
{
    std::string theString = PKStr::doubleToString(inVal, precision);
    return PKStr::stringToWString(theString);
}

double PKStr::wstringToDouble(std::wstring inVal)
{
    std::string theString = PKStr::wStringToString(inVal);
    return PKStr::stringToDouble(theString);
}

std::wstring PKStr::floatToWstring(float inVal)
{
    std::string theString = PKStr::floatToString(inVal);
    return PKStr::stringToWString(theString);
}

float PKStr::wstringToFloat(std::wstring inVal)
{
    std::string theString = PKStr::wStringToString(inVal);
    return PKStr::stringToFloat(theString);
}

std::wstring PKStr::uint64ToWstring(uint64_t inVal)
{
    std::string theString = PKStr::uint64ToString(inVal);
    return PKStr::stringToWString(theString);
}

uint64_t PKStr::wstringToUint64(std::wstring inVal)
{
    std::string theString = PKStr::wStringToString(inVal);
    return PKStr::stringToUint64(theString);
}

std::wstring PKStr::int64ToWstring(int64_t inVal)
{
    std::string theString = PKStr::int64ToString(inVal);
    return PKStr::stringToWString(theString);
}

int64_t PKStr::wstringToInt64(std::wstring inVal)
{
    std::string theString = PKStr::wStringToString(inVal);
    return PKStr::stringToInt64(theString);
}



std::string PKStr::wStringToMultiByte(const std::wstring& w)
{
    const wchar_t* wstr = w.c_str();
    int mbMultiplier = 4; // hard  code to 4 -- we don't know from the 1st char about the wole string
    const int wsize = w.size() * mbMultiplier + 1;

    // we need to figure out if what locale it is -- mostly for japanese
    std::string defaultLanguage = "English";

#ifdef WIN32
    LANGID langID = GetSystemDefaultLangID();
    if (langID == 0x0411)
    {
        defaultLanguage = "Japanese";
    }
    else if (langID >= 0x040c && langID <= 0x140c) // french
    {
        defaultLanguage = "French";
    }
    else if (langID >= 0x0407 && langID <= 0x1407) // german
    {
        defaultLanguage = "German";
    }
#elif MACOSX
    CFPropertyListRef list;
    CFStringRef string;

    list = CFPreferencesCopyAppValue(CFSTR("AppleLanguages"), kCFPreferencesCurrentApplication);

    if (list != nil)
    {
        string = (CFStringRef)CFArrayGetValueAtIndex((CFArrayRef)list, 0);

        if (CFStringCompare(string, CFSTR("ja"), 0) == 0)
        {
            defaultLanguage = "Japanese";
        }
        else if (CFStringCompare(string, CFSTR("fr"), 0) == 0)
        {
             defaultLanguage = "French";
        }
        else if (CFStringCompare(string, CFSTR("de"), 0) == 0)
        {
            defaultLanguage = "German";
        }

        CFRelease(list);
    }
#endif

    setlocale(LC_ALL, defaultLanguage.c_str());
    char *buffer = new char[wsize];

#ifdef WIN32
    size_t count;
    wcstombs_s(&count, buffer, wsize, wstr, wsize);
#else
    size_t count = wcstombs(buffer, wstr, wsize);
#endif

    if (count == -1)
    {
        return std::string();
    }

    if (count < w.size()) // the string could get much larger -- and that's ok
    {
        return std::string();
    }

    if (!buffer)
    {
        return std::string();
    }

    std::string result(buffer, count);

    delete [] buffer;

    return result;
}

std::wstring PKStr::multiByteToWString(const std::string& s)
{
    const char* str = s.c_str();
    const int size = s.size() + 1;

    wchar_t *buffer = new wchar_t[size];

#ifdef WIN32
    size_t count;
    mbstowcs_s(&count, buffer, size, str, size);
#else
    size_t count = mbstowcs(buffer, str, size);
#endif

    if (count == -1)
    {
        return std::wstring();
    }

    //if (count != s.size()) this won't work for non-standard chars
    //{
    //    return std::wstring();
    //}

    if (!buffer)
    {
        return std::wstring();
    }

    std::wstring result(buffer, count);

    delete [] buffer;

    return result;
}

std::string PKStr::wStringToUTF8string(const std::wstring& w)
{
    bool useBOM = false;
    int maxSize = (w.size() * 3) + 1; // have 1 extra for NULL
	char *utf8 = new char[maxSize];

    int newSize = WchartToUTF8(w.c_str(), w.size(), (unsigned char *) utf8, maxSize -1, useBOM);

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

std::wstring PKStr::utf8StringToWstring(const std::string& u)
{
    bool hasBOM = false;
    int maxSize = u.size() + 1; // add 1 extra for NULL
    wchar_t *wbuffer = new wchar_t[maxSize];

    int newSize = UTF8ToWchart((const unsigned char *)u.c_str(), u.size(), wbuffer, maxSize - 1, hasBOM);

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


#ifdef MACOSX

CFStringRef PKStr::wStringToCFString(std::wstring wstr)
{
    CFStringRef result = NULL;

    uint32_t size = 0;
    UniChar *ptr  = NULL;

    PKStr::wStringToUniChar(wstr, &size, &ptr);

    if(ptr != NULL)
    {
        result = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                              ptr, size);

        delete [] ptr;
    }
    
    if (!result)
        result = CFSTR("");

    return result;
}

std::wstring PKStr::cfStringToWString(CFStringRef cfStr)
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

    result = PKStr::uniCharToWString(buffer, size);
    delete [] buffer;

    return result;
}

void PKStr::wStringToUniChar(std::wstring wstr,
                                   uint32_t *size,
                                   UniChar **ptr)
{
    if(size == NULL || ptr == NULL)
    {
        return;
    }

    if(wstr == L"")
    {
        *ptr = NULL;
        *size = 0;

        return;
    }

    int s = wstr.size();

    *ptr = new UniChar[s];

    if(*ptr == NULL)
    {
        *size = 0;
        return;
    }

    for(int i=0; i<s; i++)
    {
        (*ptr)[i] = (UniChar) wstr[i];
    }

    *size = s;
}

std::wstring PKStr::uniCharToWString(UniChar *chars,
                                           uint32_t ccount)
{
    if(chars == NULL || ccount == 0)
    {
        return L"";
    }

    wchar_t *ptr = new wchar_t[ccount];

    if(ptr == NULL)
    {
        return L"";
    }

    for(uint32_t i=0; i < ccount; i++)
    {
        ptr[i] = (wchar_t) chars[i];
    }

    std::wstring newString(ptr, ccount);
    delete [] ptr;

    return newString;
}


#endif
