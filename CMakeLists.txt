# detect non-Windows and use cross-mingw64
if (NOT WIN32 AND NOT CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain-cross-mingw64.cmake")
endif()

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.2)
cmake_policy(SET CMP0048 NEW)

project(StayAwake VERSION 0.4 LANGUAGES CXX)
set(PROJECT_AUTHOR "Richard Liebscher")
set(PROJECT_COPYRIGHT "Copyright (C) 2010, 2015  Richard Liebscher <r1tschy@yahoo.de>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Windows
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mwindows -municode -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -municode  -static-libgcc -static-libstdc++")

# UNICODE
add_definitions(-DUNICODE -D_UNICODE)

# Windows XP
add_definitions(-DNTDDI_VERSION=0x05010300 -D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -D_WIN32_IE=0x0700)

# Windows Vista
#add_definitions(-DNTDDI_VERSION=0x06000000 -D_WIN32_WINNT=0x0600 -DWINVER=0x0600 -D_WIN32_IE=0x0700)


# Warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
 -Wall \
 -Wextra \
 -Wnon-virtual-dtor \
 -Wcast-align \
 -Wunused \
 -Woverloaded-virtual \
 -Wcast-qual \
 -Wconversion \
")

set_property(GLOBAL PROPERTY CXX_STANDARD 14)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs)
add_subdirectory(src)