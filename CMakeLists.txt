# detect non-Windows and use cross-mingw64
if (NOT WIN32 AND NOT CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain-cross-mingw64.cmake")
endif()


SET(CMAKE_AR gcc-ar)
SET(CMAKE_NM gcc-nm)
SET(CMAKE_RANLIB gcc-ranlib)

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.2)
cmake_policy(SET CMP0048 NEW)

project(StayAwake VERSION 0.4.1 LANGUAGES CXX)
set(PROJECT_AUTHOR "Richard Liebscher")
set(PROJECT_DESCRIPTION "status icon for blocking screensaver and standby in Windows")
set(PROJECT_DESCRIPTION_DE "Statusymbol zur Blockieren von Screensaver und Standby in Windows")
set(PROJECT_COPYRIGHT "Copyright (C) 2010,2015,2016 Richard Liebscher <r1tschy@yahoo.de>")
set(PROJECT_URL "https://github.com/R1tschY/StayAwake")
set(PROJECT_LONGVERSION "${PROJECT_VERSION}.0.0")
string(REGEX REPLACE "\\." "," PROJECT_VERSIONINFO "${PROJECT_LONGVERSION}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Debug mode
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g -D_DEBUG")

# Release mode
set(CMAKE_CXX_FLAGS_RELEASE "-s -O2 -DNDEBUG")

# Windows
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -municode -static-libgcc -static-libstdc++")

# UNICODE
add_definitions(-DUNICODE -D_UNICODE)

# Windows XP
#add_definitions(-DNTDDI_VERSION=0x05010300 -D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -D_WIN32_IE=0x0700)

# Windows Vista
add_definitions(-DNTDDI_VERSION=0x06000000 -D_WIN32_WINNT=0x0600 -DWINVER=0x0600 -D_WIN32_IE=0x0700)

# Boost options
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

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
