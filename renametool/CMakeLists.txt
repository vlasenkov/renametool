cmake_minimum_required (VERSION 3.10)

add_executable (renametool "main.cpp" "DateTime.cpp" "JpgParser.cpp" "Mp4Parser.cpp")

set_property(TARGET renametool PROPERTY CXX_STANDARD 17)
set_property(TARGET renametool PROPERTY CXX_STANDARD_REQUIRED ON)
