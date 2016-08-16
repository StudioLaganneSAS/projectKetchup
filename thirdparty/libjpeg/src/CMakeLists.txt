# Require CMake version 2.5 or above
cmake_minimum_required (VERSION 2.6)

# The name of this project
project (jpeg)

if(WIN32)
add_definitions(-DWIN32 -DX86 -DUNICODE -D_UNICODE)
endif(WIN32)

if(APPLE)

add_definitions(-DMACOSX -DX86)

SET (SDKVER "10.5")
SET (SDKROOT "/Developer/SDKs/MacOSX10.5.sdk")
SET (CMAKE_OSX_SYSROOT "${SDKROOT}")
SET (CMAKE_OSX_ARCHITECTURES "i386" "ppc")

include_directories("/Developer/Headers/FlatCarbon")

endif(APPLE)

if(UNIX AND NOT APPLE)
add_definitions(-DLINUX -DX86)
endif(UNIX AND NOT APPLE)

# Setup static CRT linking for MSVC
# so that we don't depend on CRT DLLs
if(MSVC)
# We statically link to reduce dependancies
foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MD")
    if(${flag_var} MATCHES "/MDd")
        string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MDd")
endforeach(flag_var)
endif(MSVC)

# Setup output directories for binary products
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR})

# Add include 
include_directories("${PROJECT_SOURCE_DIR}")

# Add jpeg library
add_library(jpeg 

ansi2knr.c
cderror.h
cdjpeg.c
cdjpeg.h
cjpeg.c
ckconfig.c
djpeg.c
jcapimin.c
jcapistd.c
jccoefct.c
jccolor.c
jcdctmgr.c
jchuff.c
jchuff.h
jcinit.c
jcmainct.c
jcmarker.c
jcmaster.c
jcomapi.c
jconfig.h
jcparam.c
jcphuff.c
jcprepct.c
jcsample.c
jctrans.c
jdapimin.c
jdapistd.c
jdatadst.c
jdatasrc.c
jdcoefct.c
jdcolor.c
jdct.h
jddctmgr.c
jdhuff.c
jdhuff.h
jdinput.c
jdmainct.c
jdmarker.c
jdmaster.c
jdmerge.c
jdphuff.c
jdpostct.c
jdsample.c
jdtrans.c
jerror.c
jerror.h
jfdctflt.c
jfdctfst.c
jfdctint.c
jidctflt.c
jidctfst.c
jidctint.c
jidctred.c
jinclude.h
jmemansi.c
jmemmac.c
jmemmgr.c
jmemname.c
jmemnobs.c
jmemsys.h
jmorecfg.h
jpegint.h
jpeglib.h
jpegtran.c
jquant1.c
jquant2.c
jutils.c
jversion.h
rdbmp.c
rdcolmap.c
rdgif.c
rdjpgcom.c
rdppm.c
rdrle.c
rdswitch.c
rdtarga.c
transupp.c
transupp.h
wrbmp.c
wrgif.c
wrjpgcom.c
wrppm.c
wrrle.c
wrtarga.c

)
