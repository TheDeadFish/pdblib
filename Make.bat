@setlocal
@call egcc.bat
@if not exist build32 mkdir build32
@set CFLAGS=%CFLAGS2%
@set CXXFLAGS=%CCFLAGS2%
@pushd build32
@%CMAKE%  ..\.
@mingw32-make
@popd

:: install library
@copy /Y src\pdblib.h %PROGRAMS%\local\include
@copy /Y bin\libpdbLib.a %PROGRAMS%\local\lib32
