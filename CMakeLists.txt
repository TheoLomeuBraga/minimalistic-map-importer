cmake_minimum_required(VERSION 2.8)
project(minimalistic_map_inporter)

#sdl
set(SDL_STATIC ON)
add_subdirectory(libs/SDL)
add_subdirectory(libs/SDL_image)

#exe
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
include_directories(demo include libs/SDL/include libs/SDL_image/include)
add_executable(demo shaders demo/main.cpp)

find_package(OpenGL REQUIRED)
target_link_libraries(demo SDL3::SDL3-static ${OPENGL_LIBRARIES})
link_libraries(demo SDL3::SDL3-static)
