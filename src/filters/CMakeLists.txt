

# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools"
					"${PROJECT_SOURCE_DIR}/src/vector"
					"${PROJECT_SOURCE_DIR}/src/filterGraph"
					"${PROJECT_SOURCE_DIR}/thirdparty/libjpeg/all/all/include")

# Add filters library
add_library(filters

PKFBmpReaderFilter.cpp
PKFBmpReaderFilter.h
PKFFileSinkFilter.cpp
PKFFileSinkFilter.h
PKFFileSourceFilter.cpp
PKFFileSourceFilter.h
PKFFiltersFactory.cpp
PKFFiltersFactory.h
PKFJpegReaderFilter.cpp
PKFJpegReaderFilter.h
PKFJpegWriterFilter.cpp
PKFJpegWriterFilter.h

)
