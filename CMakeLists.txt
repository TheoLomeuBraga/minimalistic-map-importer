cmake_minimum_required(VERSION 2.8)
project(minimalistic_map_inporter)

#sdl
set(SDL_STATIC ON)
add_subdirectory(libs/SDL)
add_subdirectory(libs/SDL_image)

#exe
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
include_directories(demo include reference libs/SDL/include libs/SDL_image/include)
add_executable(demo shaders demo/demo.cpp)

find_package(OpenGL REQUIRED)
target_link_libraries(demo SDL3::SDL3-static ${OPENGL_LIBRARIES})
link_libraries(demo SDL3::SDL3-static)

#test reference
include_directories(reference)
add_executable(test_reference reference/test_reference.cpp reference/brush.cpp reference/csg.cpp reference/entity.cpp reference/face.cpp reference/map.cpp reference/poly.cpp reference/property.cpp reference/test_reference.cpp reference/texture.cpp reference/WAD3.cpp)
