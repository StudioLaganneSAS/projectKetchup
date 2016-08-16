#!/bin/bash

cd build
cmake -G "Xcode" ../
cd ..

# Hack to fix CMake problem with nasm files
sed -i tmp 's/lastKnownFileType = sourcecode; name = "PKX86.nasm";/lastKnownFileType = sourcecode.nasm; name = "PKX86.nasm";/g' "build/projectKetchup.xcodeproj/project.pbxproj"
sed -i tmp 's/lastKnownFileType = sourcecode; name = "PKBitmapOpsX86.nasm";/lastKnownFileType = sourcecode.nasm; name = "PKBitmapOpsX86.nasm";/g' "build/projectKetchup.xcodeproj/project.pbxproj"


