
# Add include directories 

include_directories("${PROJECT_SOURCE_DIR}/src/core"
					"${PROJECT_SOURCE_DIR}/src/tools" 
					"${PROJECT_SOURCE_DIR}/src/ui" 
					"${PROJECT_SOURCE_DIR}/src/music" 
					"${PROJECT_SOURCE_DIR}/src/musicUI" 
					"${PROJECT_SOURCE_DIR}/src/vector" 
		            "${PROJECT_SOURCE_DIR}/src/dialogs")

# Setup WIN32 resources 

if(WIN32)
set(EXTRA_RESOURCES uiBuilder.rc)
endif(WIN32)					
					
# Setup MACOSX resources 

if(APPLE)
set(EXTRA_RESOURCES TB_SAVE.png TB_ABOUT.png TB_NEW.png TB_OPEN.png)
set_source_files_properties(${EXTRA_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif(APPLE)					

# Setup WIN32 link specifics					
					
if(MSVC)

link_directories("${PROJECT_SOURCE_DIR}/thirdparty/libpng/win32-vs2008/${CMAKE_CFG_INTDIR}/lib")
link_directories("${PROJECT_SOURCE_DIR}/thirdparty/libjpeg/win32-vs2008/${CMAKE_CFG_INTDIR}/lib")
link_directories("${PROJECT_SOURCE_DIR}/thirdparty/zlib/win32-vs2008/${CMAKE_CFG_INTDIR}/lib")
link_directories("${PROJECT_SOURCE_DIR}/thirdparty/sqlite3/win32-vs2008/${CMAKE_CFG_INTDIR}/lib")

link_libraries(

Gdiplus.lib
Wininet.lib
Urlmon.lib
Winmm.lib
Comctl32.lib
Msimg32.lib
libpng.lib
libjpeg.lib
zlib.lib
sqlite.lib

)

endif(MSVC)
					
# Add main executable

add_executable(uiBuilder WIN32 MACOSX_BUNDLE

PKPropertiesWindow.cpp
PKPropertiesWindow.h
main.cpp

${EXTRA_RESOURCES}

)

# Add libraries & frameworks for Mac
IF(APPLE)

   # TODO: use debug or release subduer based on configuration (HOW?)
   
   set(PNG_LIB "${PROJECT_SOURCE_DIR}/thirdparty/libpng/macosx-i386/release/lib/libpng.a")
   set(JPG_LIB "${PROJECT_SOURCE_DIR}/thirdparty/libjpeg/macosx-i386/release/lib/libjpeg.a")
   set(Z_LIB   "${PROJECT_SOURCE_DIR}/thirdparty/zlib/macosx-i386/release/lib/libzlib.a")

   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(CORE_FOUNDATION_LIBRARY CoreFoundation)
   FIND_LIBRARY(CORE_SERVICES_LIBRARY CoreServices)
   FIND_LIBRARY(QUICKTIME_LIBRARY QuickTime)
   FIND_LIBRARY(SQLITE_LIBRARY sqlite3)

   MARK_AS_ADVANCED (CARBON_LIBRARY
		     QUICKTIME_LIBRARY
                     CORE_FOUNDATION_LIBRARY
                     CORE_SERVICES_LIBRARY)

   SET(EXTRA_LIBS ${CARBON_LIBRARY} ${CORE_FOUNDATION_LIBRARY} ${CORE_SERVICES_LIBRARY} ${QUICKTIME_LIBRARY} ${SQLITE_LIBRARY} ${PNG_LIB} ${JPG_LIB} ${Z_LIB})

ENDIF (APPLE)

# Setup dependencies
target_link_libraries(uiBuilder core tools ui dialogs vector music musicUI ${EXTRA_LIBS})

if(MSVC)

# Also setup manifest additions (common controls, XP-style)

set_target_properties(uiBuilder
		PROPERTIES
		LINK_FLAGS "/MANIFESTDEPENDENCY:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' publicKeyToken='6595b64144ccf1df' language='*' processorArchitecture='*'\""
)

endif(MSVC)					
