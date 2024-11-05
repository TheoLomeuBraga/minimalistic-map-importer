cmake_minimum_required(VERSION 2.8)
project(minimalistic_map_inporter)


#test reference
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
include_directories(demo src)
add_executable(test_reference demo/test_reference.cpp src/brush.cpp src/entity.cpp src/face.cpp src/map.cpp src/poly.cpp src/property.cpp)
