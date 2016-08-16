
# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools"
					"${PROJECT_SOURCE_DIR}/thirdparty/sqlite3/all/all/include")

				
# Add tools library
add_library(tools 

PKBase64.cpp
PKBase64.h
PKBrowserTool.cpp
PKBrowserTool.h
PKByteReader.cpp
PKByteReader.h
PKClipboard.cpp
PKClipboard.h
PKCpu.cpp
PKCpu.h
PKCrc.cpp
PKCrc.h
PKCritSec.cpp
PKCritSec.h
PKCursorTools.cpp
PKCursorTools.h
PKDatabase.cpp
PKDatabase.h
PKDownload.cpp
PKDownload.h
PKDownloader.cpp
PKDownloader.h
PKFile.cpp
PKFile.h
PKLogger.cpp
PKLogger.h
PKMemBuffer.cpp
PKMemBuffer.h
PKMutex.cpp
PKMutex.h
PKOS.cpp
PKOS.h
PKPath.cpp
PKPath.h
PKPersistTool.cpp
PKPersistTool.h
PKSafeEvents.cpp
PKSafeEvents.h
PKScratchBuffer.cpp
PKScratchBuffer.h
PKScreenTool.cpp
PKScreenTool.h
PKSettings.cpp
PKSettings.h
PKStr.cpp
PKStr.h
PKThread.cpp
PKThread.h
PKTime.cpp
PKTime.h
PKTimer.cpp
PKTimer.h
PKUndoManager.cpp
PKUndoManager.h
PKUndoableCommand.cpp
PKUndoableCommand.h
PKX86.h
UTFConv.cpp
UTFConv.h

)