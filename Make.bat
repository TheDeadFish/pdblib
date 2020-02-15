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
::@if not exist %PROGRAMS%\local\include\peFile ( 
::	mkdir %PROGRAMS%\local\include\peFile  )
::@copy /Y src\peFile\*.h %PROGRAMS%\local\include\peFile
::@copy /Y bin\libpeFile.a %PROGRAMS%\local\lib32
