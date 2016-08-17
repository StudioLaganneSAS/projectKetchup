//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKSTR_H
#define PKSTR_H

#include "PKTypes.h"
#include <string>
#include <vector>

#ifdef MACOSX
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef LINUX
#include <stdio.h>
#endif

class PKStr
{
public:

    // string <-> wstring

    static std::wstring stringToWString(std::string src);
    static std::string  wStringToString(std::wstring src);

    // Upper / Lower case conversions

    static std::string  toLower(std::string str);
    static std::wstring toLower(std::wstring str);

    static std::string  toUpper(std::string str);
    static std::wstring toUpper(std::wstring str);

    // Find
    // returns wstring::npos if not found

    static int findFirstOf(std::wstring target,
                           std::wstring toFind);

    // Find
    // returns string::npos if not found

    static int findFirstOf(std::string target,
                           std::string toFind);

    // Single Character replacement

    static std::wstring replaceChar(wchar_t find,
                                    wchar_t replace,
                                    std::wstring str);

    static std::string stripBeginingAndEndChar(std::string stringToStrip,
                                               char stripChar);

    static std::wstring stripBeginingAndEndChar(std::wstring stringToStrip,
                                                wchar_t stripChar);

    // Whole sub-string replacement

    static std::wstring replaceString(std::wstring find,
                                      std::wstring replace,
                                      std::wstring str);

    static std::string replaceString(std::string find,
                                     std::string replace,
                                     std::string str);


	static std::string escapeSingleQuotes(std::string input);
	static std::wstring escapeSingleQuotes(std::wstring input);

    // Matching

    static bool isOnlyWhiteSpaceAndTabs(std::string  input);
    static bool isOnlyWhiteSpaceAndTabs(std::wstring input);

    // String breakdown

    static void  explode(std::wstring str,
                         wchar_t delimiter,
                         std::vector <std::wstring> *result);

    static void  explode(std::string str,
                         char delimiter,
                         std::vector <std::string> *result);

    // Conversions

    static std::string boolToString(bool inVal);
    static bool stringToBool(std::string inVal);

    static std::string uint32ToString(uint32_t inVal);
    static uint32_t stringToUInt32(std::string inVal);

    static std::string int32ToString(int32_t inVal);
    static int32_t stringToInt32(std::string inVal);

    static std::string doubleToString(double inVal, int precision = 0);
    static double stringToDouble(std::string inVal);

    static std::string floatToString(float inVal);
    static float stringToFloat(std::string inVal);

    static std::string uint64ToString(uint64_t inVal);
    static uint64_t stringToUint64(std::string inVal);

    static std::string int64ToString(int64_t inVal);
    static int64_t stringToInt64(std::string inVal);

    static std::wstring boolToWstring(bool inVal);
    static bool wstringToBool(std::wstring inVal);

    static std::wstring uint32ToWstring(uint32_t inVal);
    static uint32_t wstringToUInt32(std::wstring inVal);

    static std::wstring int32ToWstring(int32_t inVal);
    static int32_t wstringToInt32(std::wstring inVal);

    static std::wstring doubleToWstring(double inVal, int precision = 0);
    static double wstringToDouble(std::wstring inVal);

    static std::wstring floatToWstring(float inVal);
    static float wstringToFloat(std::wstring inVal);

    static std::wstring uint64ToWstring(uint64_t inVal);
    static uint64_t wstringToUint64(std::wstring inVal);

    static std::wstring int64ToWstring(int64_t inVal);
    static int64_t wstringToInt64(std::wstring inVal);

    static std::string wStringToMultiByte(const std::wstring& w);
    static std::wstring multiByteToWString(const std::string& s);

    static std::string  wStringToUTF8string(const std::wstring& w);
    static std::wstring utf8StringToWstring(const std::string& u);

#ifdef MACOSX

    // Utility conversions

    static CFStringRef wStringToCFString(std::wstring wstr);
    static std::wstring cfStringToWString(CFStringRef cfStr);

    // Convert a wstring to unichars
    // on success ptr is not null and
    // must be freed by the caller

    static void wStringToUniChar(std::wstring wstr,
                                 uint32_t *size,
                                 UniChar **ptr);

    // Converts a unichar buffer to a wstring

    static std::wstring uniCharToWString(UniChar *chars,
                                         uint32_t ccount);

#endif
};

#endif // PKSTR_H
