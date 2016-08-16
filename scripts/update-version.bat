rem Compute the version for the library
rem -----------------------------------

setLocal EnableDelayedExpansion

set /P PK_MAJOR=<MAJOR_VERSION.txt
set /P PK_MINOR=<MINOR_VERSION.txt
set /A PK_MINOR=!PK_MINOR!+1
echo.%PK_MINOR%>MINOR_VERSION.txt

rem Output the version to the header in src/
rem ----------------------------------------

echo.#define PK_VERSION "%PK_MAJOR%.%PK_MINOR%">src/PKVersion.h

