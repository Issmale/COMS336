cmake_minimum_required(VERSION 3.10)
project(COMS336_Raytracer)

set(CMAKE_CXX_STANDARD 17)

if(APPLE)
    set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    include_directories("/opt/homebrew/opt/libomp/include")
endif()

find_package(OpenMP)

add_executable(raytracer 
    src/main.cpp
    src/stb_image.cpp
    src/obj_loader.cpp
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(raytracer OpenMP::OpenMP_CXX)
    message("OpenMP enabled")
endif()