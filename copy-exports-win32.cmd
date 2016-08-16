
rem Copy includes

xcopy src\*.h             export\all\all\include\ /Y 
xcopy src\core\*.h        export\all\all\include\ /Y 
xcopy src\dialogs\*.h     export\all\all\include\ /Y 
xcopy src\filterGraph\*.h export\all\all\include\ /Y 
xcopy src\filters\*.h     export\all\all\include\ /Y 
xcopy src\music\*.h       export\all\all\include\ /Y 
xcopy src\musicUI\*.h     export\all\all\include\ /Y 
xcopy src\tools\*.h       export\all\all\include\ /Y 
xcopy src\ui\*.h          export\all\all\include\ /Y 
xcopy src\vector\*.h      export\all\all\include\ /Y 

rem Copy Debug libs

xcopy build\Debug\core.lib         export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\dialogs.lib      export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\filterGraph.lib  export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\filters.lib      export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\music.lib        export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\musicUI.lib      export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\tools.lib        export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\ui.lib           export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\vector.lib       export\win32-vs2008\debug\lib\   /Y 

rem Copy Debug info

xcopy build\Debug\core.pdb         export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\dialogs.pdb      export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\filterGraph.pdb  export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\filters.pdb      export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\music.pdb        export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\musicUI.pdb      export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\tools.pdb        export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\ui.pdb           export\win32-vs2008\debug\lib\   /Y 
xcopy build\Debug\vector.pdb       export\win32-vs2008\debug\lib\   /Y 

rem Copy Release libs

xcopy build\Release\core.lib         export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\dialogs.lib      export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\filterGraph.lib  export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\filters.lib      export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\music.lib        export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\musicUI.lib      export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\tools.lib        export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\ui.lib           export\win32-vs2008\release\lib\   /Y 
xcopy build\Release\vector.lib       export\win32-vs2008\release\lib\   /Y 

rem Copy Debug third-party dependencies

xcopy thirdparty\libpng\win32-vs2008\debug\lib\libpng.lib   export\win32-vs2008\debug\lib\   /Y 
xcopy thirdparty\libjpeg\win32-vs2008\debug\lib\libjpeg.lib export\win32-vs2008\debug\lib\   /Y 
xcopy thirdparty\zlib\win32-vs2008\debug\lib\zlib.lib       export\win32-vs2008\debug\lib\   /Y 
xcopy thirdparty\sqlite3\win32-vs2008\debug\lib\sqlite.lib   export\win32-vs2008\debug\lib\   /Y 

rem Copy Release third-party dependencies

xcopy thirdparty\libpng\win32-vs2008\release\lib\libpng.lib   export\win32-vs2008\release\lib\   /Y 
xcopy thirdparty\libjpeg\win32-vs2008\release\lib\libjpeg.lib export\win32-vs2008\release\lib\   /Y 
xcopy thirdparty\zlib\win32-vs2008\release\lib\zlib.lib       export\win32-vs2008\release\lib\   /Y 
xcopy thirdparty\sqlite3\win32-vs2008\release\lib\sqlite.lib   export\win32-vs2008\release\lib\   /Y 

rem Copy executables

xcopy build\Debug\pki18nlst.exe         export\win32-vs2008\debug\bin\   /Y
xcopy build\Debug\pki18nstrip.exe       export\win32-vs2008\debug\bin\   /Y
xcopy build\Debug\uiBuilder.exe         export\win32-vs2008\debug\bin\   /Y

xcopy build\Release\pki18nlst.exe       export\win32-vs2008\release\bin\ /Y
xcopy build\Release\pki18nstrip.exe     export\win32-vs2008\release\bin\ /Y
xcopy build\Release\uiBuilder.exe       export\win32-vs2008\release\bin\ /Y
 
rem Use pki18nlst to generate a strings file and export it

call build\Debug\pki18nlst.exe -i i18n\i18nFolderList.txt -o build\projectKetchup_en_US.pot
xcopy build\projectKetchup_en_US.pot         export\all\all\i18n\   /Y

if not defined _MASTER_BUILD pause
