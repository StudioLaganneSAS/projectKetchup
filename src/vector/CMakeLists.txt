
# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools"
					"${PROJECT_SOURCE_DIR}/thirdparty/libpng/all/all/include"
					"${PROJECT_SOURCE_DIR}/thirdparty/zlib/all/all/include"
					"${PROJECT_SOURCE_DIR}/thirdparty/libjpeg/all/all/include")

# Add vectorlibrary
add_library(vector 

PKGIFLoader.cpp
PKGIFLoader.h
PKImage.cpp
PKImage.h
PKImageLoader.cpp
PKImageLoader.h
PKJPGLoader.cpp
PKJPGLoader.h
PKPNGLoader.cpp
PKPNGLoader.h
PKVectorEngine.cpp
PKVectorEngine.h

)