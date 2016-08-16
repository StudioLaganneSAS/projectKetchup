
# Add tools include directory as PKI18n needs PKStr (TODO: fix this)
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
		            "${PROJECT_SOURCE_DIR}/src/tools")

# Add core library
add_library(core IPKObjectFactory.h PKi18n.cpp PKi18n.h PKObject.cpp PKObject.h PKObjectLoader.cpp PKObjectLoader.h 
			PKProperty.cpp PKProperty.h PKPropertyBag.cpp PKPropertyBag.h PKPropertyCallback.h PKTypes.cpp PKTypes.h
			PKVariant.cpp PKVariant.h)
