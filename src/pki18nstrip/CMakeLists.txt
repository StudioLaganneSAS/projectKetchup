
# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools")
					
# Add pki18nlst exe
add_executable(pki18nstrip main.cpp)

# Add dependencies
target_link_libraries(pki18nstrip core tools)