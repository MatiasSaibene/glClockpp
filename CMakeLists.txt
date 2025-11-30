cmake_minimum_required(VERSION 3.10)
project(glClockpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

set(DEST_DIR ${CMAKE_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})

#Include GLAD directories
include_directories(${CMAKE_SOURCE_DIR})

#Finds GLFW
find_package(PkgConfig REQUIRED)
find_package(SDL3 REQUIRED)
find_package(assimp REQUIRED)

# Add executable
add_executable(${PROJECT_NAME}
    main.cpp
    Shader.cpp
    stb_image.cpp
    glad/src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    glad/include
)

#Linkea GLFW y OpenGL
target_link_libraries(${PROJECT_NAME}
    SDL3::SDL3 
    SDL3_image 
    SDL3_ttf
    GL
    dl
    assimp::assimp
)

