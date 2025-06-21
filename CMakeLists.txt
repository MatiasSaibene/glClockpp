cmake_minimum_required(VERSION 3.10)
project(glClockpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

set(SHADER_FILES
    model_shader.vs
    model_shader.fs
)

set(TEXTURE_FILES
    Clock_face.png
    blanco.png
    celeste.png
    gris.png
    negro.png
)

set(MODEL_FILES
    3DClock.obj
    3DClock.mtl
    Minutes_hand.obj
    Minutes_hand.mtl
    Hours_hand.obj
    Hours_hand.mtl
)

set(DEST_DIR ${CMAKE_BINARY_DIR})

# Copiar shaders
foreach(FILE ${SHADER_FILES})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${FILE} ${DEST_DIR}/${FILE} COPYONLY)
endforeach()

# Copiar texturas
foreach(FILE ${TEXTURE_FILES})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${FILE} ${DEST_DIR}/${FILE} COPYONLY)
endforeach()

# Copiar modelos
foreach(FILE ${MODEL_FILES})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${FILE} ${DEST_DIR}/${FILE} COPYONLY)
endforeach()

#Include GLAD directories
include_directories(${CMAKE_SOURCE_DIR})

#Finds GLFW
find_package(PkgConfig REQUIRED)
find_package(assimp REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

#Add executable
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
    ${GLFW_LIBRARIES}
    GL
    dl
    assimp::assimp
)

