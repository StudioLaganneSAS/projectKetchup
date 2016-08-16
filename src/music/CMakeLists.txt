

# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools"
					"${PROJECT_SOURCE_DIR}/src/vector")

# Add music library
add_library(music

PKChord.cpp
PKChord.h
PKChordDiagram.cpp
PKChordDiagram.h
PKChordFinder.cpp
PKChordFinder.h
PKGuitarNeck.cpp
PKGuitarNeck.h
PKNeck.cpp
PKNeck.h
PKNote.cpp
PKNote.h
PKNotes.h
PKString.cpp
PKString.h
PKTuning.cpp
PKTuning.h

)
