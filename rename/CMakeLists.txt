# CMakeList.txt : CMake project for rename, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Add source to this project's executable.
#add_compile_options("/std:c++17")
add_executable (rename "parse.cpp")
#set_property(TARGET rename PROPERTY CXX_STANDARD 17)
set_property(TARGET rename PROPERTY CXX_STANDARD 17)
set_property(TARGET rename PROPERTY CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
