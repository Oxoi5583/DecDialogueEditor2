cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0135 NEW)

project(DecDialogueEditor LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
if (MSVC)
    add_compile_options(/utf-8)
endif()

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST OFF   CACHE BOOL "" FORCE)

set(SDL_RENDER OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)

set(SDL_VIDEO_OPENGL ON CACHE BOOL "" FORCE)
set(SDL_VIDEO_VULKAN OFF CACHE BOOL "" FORCE)
set(SDL_VIDEO_METAL OFF CACHE BOOL "" FORCE)

set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES interprocess)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

file(GLOB_RECURSE SOURCES src/*.c** src/*/*.c* src_extern/*.c** src_extern/*/*.c*)

add_subdirectory(third_party/SDL3)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(OpenGL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
    SimZip
    GIT_REPOSITORY https://github.com/L-Super/SimZip.git
    GIT_TAG "main"
)
FetchContent_MakeAvailable(SimZip)

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
    URL_MD5 893b5203b862eb9bbd08553e24ff146a
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(Boost)

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
        ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
        ${OPENGL_LIBRARIES}
        SDL3::SDL3
        glad::glad
        SimZip
        Boost::interprocess
)