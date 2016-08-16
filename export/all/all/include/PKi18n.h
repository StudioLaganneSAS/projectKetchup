//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PK_I18N_H
#define PK_I18N_H

#include "PKTypes.h"
#include "PKVariant.h"

#include <string>
#include <vector>

#ifdef LINUX
#include <wchar.h>
#include <wctype.h>
#endif

#ifdef MACOSX
#include <CoreFoundation/CoreFoundation.h>
#endif

//
// PK_i18n() : Main i18n function
//             Performs string swapping
//             based on the current locale
//

std::wstring PK_i18n(std::wstring source);

//
// PK_noop_i18n() : Marks a strings for inclusion
//                  in the strings table but don't
//                  do anything on it at runtime
//

std::wstring PK_noop_i18n(std::wstring source);

//
// PK_i18n() : Secondray i18n function
//             Performs string swapping
//             based on a given locale
//

std::wstring PK_i18n(std::wstring source, std::string locale);

//
// Internal structures
//

typedef struct _pkLocaleTable {

    std::string              locale;
    std::vector<std::string> originals;
    std::vector<std::string> translations;
    bool                     utf8;

} pkLocaleTable;

//
// PKi18n singleton class
// ----------------------
//
// Warning this class is not re-entrant
// Use only from the main thread of any app
//

class PKi18n 
{
public:

    //
    // Static Instance Creator (Singleton)
    //

    static PKi18n *getInstance();

    //
    // Static Helpers
    //

    //
    // getSystemLocale()
    // -----------------
    //
    // Return the current system locale for the current
    // user for UI presentation elements according to the
    // combination of ISO language and country codes, i.e;
    // for example fr_FR for French/France, etc...
    //

    static std::string getSystemLocale();

    //
    // Member functions
    // ----------------
    //
    // Used to add a language file or resource
    // and to select the current locale string
    //

    //
    // setLocale() : Expects "fr_FR", or "en_US", etc
    //

    std::string getLocale();
    bool        setLocale(std::string locale);

    //
    // addLocale()
    // -----------
    //
    // Adds a translation file in the .mo file format
    // from a resource file
    //

    bool addLocale(std::string locale); // Expects a resource named the same as locale, i.e. "fr_FR"
                                        // on windows and a file resource on mac named 'fr_FR.mo'...

    uint32_t    getNumberOfAvailableLocales();
    std::string getAvailableLocaleAt(uint32_t index);

    std::wstring getLocalizedNameForLocale(std::string locale);

protected:

    PKi18n();
    ~PKi18n();

protected:

    static PKi18n *_instance;

    //
    // Private .mo file parser
    // 

    bool addLocale(std::string locale, void *buffer, uint32_t size);

public:

    int32_t               currentIndex;
    std::string           currentLocale;
    std::vector<pkLocaleTable *> tables;

    //
    // Members
    //

    static std::wstring stringToWString(std::string  src);
    static std::string  wStringToString(std::wstring src);
    static std::string  wStringToUTF8string(const std::wstring& w);
    static std::wstring utf8StringToWstring(const std::string& u);
    static std::wstring toUpper(std::wstring str);
	
#ifdef MACOSX
	
	static std::wstring uniCharToWString(UniChar *chars,
			 						     uint32_t size);
	
	static std::wstring cfStringToWString(CFStringRef cfStr);
#endif
	
    static void         setAppSupportPath(std::wstring path);
    static std::wstring getAppSupportPath();

private:

    static std::wstring appSupportPath;
};

#endif // PK_I18N_H
