cmake_minimum_required(VERSION 3.19)

project(lua_csound LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules/)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#############################################

## Build Luajit

############################################

add_subdirectory(LuaJIT)
target_compile_options(libluajit PRIVATE -fPIC)

#############################################

## Build Luajit

############################################

find_package(Csound)
message(STATUS ${CSOUND_INCLUDE_DIRS})
add_library(lua_csound SHARED lua_csound.cpp mlua.hpp)
target_include_directories(lua_csound PRIVATE ${CSOUND_INCLUDE_DIRS} LuaJIT/src)
target_link_libraries(lua_csound PRIVATE libluajit)
target_compile_options(lua_csound PRIVATE -O3)
