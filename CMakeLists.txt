project(pdblib)
cmake_minimum_required(VERSION 3.16)


set( EXECUTABLE_OUTPUT_PATH ../bin)
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin)

add_library(pdbLib src/msf.cc src/dbi.cc src/symb.cc 
	src/pdblib_.cc src/pdblib.cc)

link_libraries(pdbLib -lstdshit)
add_executable(test src/test.cc)
add_executable(symb-mod src/app/symb-mod.cc)
