@call cmake_gcc DEBUG

:: install library
set DIR=%PROGRAMS%\local\include\pdblib
@mkdir %DIR%
@copy /Y src\pdblib.h %DIR%
@copy /Y src\msf.h %DIR%
@copy /Y include\cvinfo.h %DIR%
@copy /Y bin\libpdbLib.a %PROGRAMS%\local\lib32
