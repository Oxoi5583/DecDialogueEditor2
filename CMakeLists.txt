cmake_minimum_required(VERSION 3.13)
project(DecDialogueEditor LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE SOURCES src/*.c** src/*/*.c* src_extern/*.c** src_extern/*/*.c*)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)

include_directories(
    ${PROJECT_NAME} 
        SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src_extern
        ${CMAKE_SOURCE_DIR}/src_extern/imgui
        ${CMAKE_SOURCE_DIR}/src_extern/imgui/backends
)

target_link_directories(
    ${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/lib
        ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
        ${OPENGL_LIBRARIES}
        glad::glad
        SDL3/SDL3
)