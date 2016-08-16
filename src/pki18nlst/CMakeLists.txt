
# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools")
					
# Add pki18nlst exe
add_executable(pki18nlst main.cpp)

# Add frameworks for Mac
IF(APPLE)
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(CORE_FOUNDATION_LIBRARY CoreFoundation)
   MARK_AS_ADVANCED (CARBON_LIBRARY
                     CORE_FOUNDATION_LIBRARY)
   SET(EXTRA_LIBS ${CARBON_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
ENDIF (APPLE)

# Add dependencies
target_link_libraries(pki18nlst core tools ${EXTRA_LIBS})
