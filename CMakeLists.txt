project(pdblib)
cmake_minimum_required(VERSION 3.16)


set( EXECUTABLE_OUTPUT_PATH ../bin)
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin)

include_directories(include src)
add_library(pdbLib src/msf.cc src/dbi.cc src/symb.cc 
	src/pdblib_.cc src/pdblib.cc
	src/dump.cc
)

link_libraries(pdbLib -lexshit -lstdshit)
add_executable(test src/test.cc)
add_executable(symb-mod src/app/symb-mod.cc)
add_executable(msf-dump src/app/msf-dump.cc)
