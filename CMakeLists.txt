cmake_minimum_required(VERSION 3.25)

project(lua_csound LANGUAGES CXX)


# Let the user choose a build tool style
option(USE_MINGW "Use MinGW Makefiles instead of default generator" OFF)

if(USE_MINGW)
    message(STATUS "Hint: configure with -G \"MinGW Makefiles\"")
elseif(MSVC)
    message(STATUS "Building with MSVC / Visual Studio")
else()
    message(STATUS "Default generator will be used")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules/)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC) 
    set(CMAKE_CXX_FLAGS "/W3")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

#############################################

## Build Luajit

############################################

add_subdirectory(LuaJIT)
if(NOT MSVC) 
    target_compile_options(libluajit PRIVATE -fPIC)
endif()

#############################################

## Build Luajit

############################################

find_package(Csound REQUIRED)
message(STATUS ${CSOUND_INCLUDE_DIRS})
add_library(lua_csound SHARED lua_csound.cpp mlua.hpp)

target_include_directories(lua_csound PRIVATE ${CSOUND_INCLUDE_DIRS} LuaJIT/src)

target_link_libraries(lua_csound PRIVATE libluajit ${CSOUND_LIBRAIRIES})

if(MSVC) 
    set_target_properties(lua_csound PROPERTIES PREFIX "")
endif()

if(NOT MSVC)
    target_compile_options(lua_csound PRIVATE -O3)
endif()
