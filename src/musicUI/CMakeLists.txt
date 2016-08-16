
# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools"
					"${PROJECT_SOURCE_DIR}/src/vector"
					"${PROJECT_SOURCE_DIR}/src/ui"
					"${PROJECT_SOURCE_DIR}/src/dialogs"
					"${PROJECT_SOURCE_DIR}/src/music")

# Add musicUI library
add_library(musicUI

PKChordDiagramUI.cpp
PKChordDiagramUI.h
PKChordDiagramVectorUI.cpp
PKChordDiagramVectorUI.h
PKChordLibraryDialog.cpp
PKChordLibraryDialog.h
PKChordLibraryDialog_XML.h
PKMusicUIFactory.cpp
PKMusicUIFactory.h

)
