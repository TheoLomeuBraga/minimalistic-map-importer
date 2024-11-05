cmake_minimum_required(VERSION 2.8)
project(minimalistic_map_inporter)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#lib
include_directories(demo src include/mmapi)
add_library(mmapi src/brush.cpp src/entity.cpp src/face.cpp src/map.cpp src/poly.cpp src/property.cpp)

#test reference
include_directories(include)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
add_executable(test_reference demo/test_reference.cpp)
target_link_libraries(test_reference mmapi)
