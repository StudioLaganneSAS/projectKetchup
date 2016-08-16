set _MASTER_BUILD=YES

rem Update the version
call scripts/update-version.bat

rem Call CMake first
call cleanup-project-win32-vs2010.cmd
call generate-project-win32-vs2010.cmd

rem Now build the solution configurations
devenv "build\projectKetchup.sln" /Rebuild "Debug|Win32"
devenv "build\projectKetchup.sln" /Rebuild "Release|Win32"

rem Now copy exports
call copy-exports-win32.cmd

pause