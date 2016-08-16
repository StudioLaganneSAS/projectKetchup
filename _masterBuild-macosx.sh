#!/bin/bash

# Call CMake first
./cleanup-project-macosx.sh
./generate-project-macosx.sh

# Now build the solution configurations
xcodebuild -project build/projectKetchup.xcodeproj/ -alltargets -configuration Debug clean
xcodebuild -project build/projectKetchup.xcodeproj/ -alltargets -configuration Debug build
xcodebuild -project build/projectKetchup.xcodeproj/ -alltargets -configuration Release clean
xcodebuild -project build/projectKetchup.xcodeproj/ -alltargets -configuration Release build

# Now build the installers
#call scripts\build-installer.bat Debug
#call scripts\build-installer.bat Release

# Now copy exports
./copy-exports-macosx.sh

