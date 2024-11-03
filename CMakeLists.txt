cmake_minimum_required(VERSION 2.8)
project(minimalistic_map_inporter)


#test reference
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
include_directories(reference)
add_executable(test_reference reference/test_reference.cpp reference/brush.cpp reference/csg.cpp reference/entity.cpp reference/face.cpp reference/map.cpp reference/poly.cpp reference/property.cpp reference/test_reference.cpp reference/texture.cpp reference/WAD3.cpp)
