@call cmake_gcc

:: install library
@copy /Y src\pdblib.h %PROGRAMS%\local\include
@copy /Y bin\libpdbLib.a %PROGRAMS%\local\lib32
