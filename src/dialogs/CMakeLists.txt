
# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools"
					"${PROJECT_SOURCE_DIR}/src/vector"
					"${PROJECT_SOURCE_DIR}/src/ui")

# Add core library
add_library(dialogs

PKAboutDialog.cpp
PKAboutDialog.h
PKAboutDialog_XML.h
PKNumberInputDialog.cpp
PKNumberInputDialog.h
PKNumberInputDialogXML.h
PKStdDialog.cpp
PKStdDialog.h
PKStdDialogXML.h
PKTextInputDialog.cpp
PKTextInputDialog.h
PKTextInputDialogXML.h

)