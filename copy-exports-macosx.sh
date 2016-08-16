#!/bin/bash

# Copy include files

cp src/core/*.h export/all/all/include/
cp src/dialogs/*.h export/all/all/include/
cp src/filterGraph/*.h export/all/all/include/
cp src/filters/*.h export/all/all/include/
cp src/music/*.h export/all/all/include/
cp src/musicUI/*.h export/all/all/include/
cp src/tools/*.h export/all/all/include/
cp src/ui/*.h export/all/all/include/
cp src/vector/*.h export/all/all/include/

# Copy librairies

cp build/Debug/*.a   export/macosx-i386/debug/lib/
cp build/Release/*.a export/macosx-i386/release/lib/

# Copy thirdparty libs

cp thirdparty/libpng/macosx-i386/debug/lib/*.a  export/macosx-i386/debug/lib
cp thirdparty/libjpeg/macosx-i386/debug/lib/*.a export/macosx-i386/debug/lib
cp thirdparty/zlib/macosx-i386/debug/lib/*.a    export/macosx-i386/debug/lib

cp thirdparty/libpng/macosx-i386/release/lib/*.a  export/macosx-i386/release/lib
cp thirdparty/libjpeg/macosx-i386/release/lib/*.a export/macosx-i386/release/lib
cp thirdparty/zlib/macosx-i386/release/lib/*.a    export/macosx-i386/release/lib

# Copy executables

cp build/Debug/pki18nlst   export/macosx-i386/debug/bin
cp build/Debug/pki18nstrip export/macosx-i386/debug/bin

zip -r export/macosx-i386/debug/bin/uiBuilder.zip build/Debug/uiBuilder.app/

cp build/Release/pki18nlst   export/macosx-i386/release/bin
cp build/Release/pki18nstrip export/macosx-i386/release/bin

zip -r export/macosx-i386/release/bin/uiBuilder.zip build/Release/uiBuilder.app/
