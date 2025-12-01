# Install script for directory: C:/Users/johan/Documents/GitHub/lua_csound/LuaJIT/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/johan/Documents/GitHub/lua_csound/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/luajit" TYPE FILE FILES
    "C:/Users/johan/Documents/GitHub/lua_csound/LuaJIT/src/lauxlib.h"
    "C:/Users/johan/Documents/GitHub/lua_csound/LuaJIT/src/lua.h"
    "C:/Users/johan/Documents/GitHub/lua_csound/LuaJIT/src/luaconf.h"
    "C:/Users/johan/Documents/GitHub/lua_csound/out/build/x64-Debug/LuaJIT/src/luajit.h"
    "C:/Users/johan/Documents/GitHub/lua_csound/LuaJIT/src/lualib.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/johan/Documents/GitHub/lua_csound/out/build/x64-Debug/LuaJIT/src/luajit.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/johan/Documents/GitHub/lua_csound/out/build/x64-Debug/LuaJIT/src/luajit.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("C:/Users/johan/Documents/GitHub/lua_csound/out/build/x64-Debug/LuaJIT/src/CMakeFiles/luajit.dir/install-cxx-module-bmi-Debug.cmake" OPTIONAL)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/johan/Documents/GitHub/lua_csound/out/build/x64-Debug/LuaJIT/src/host/cmake/minilua/cmake_install.cmake")
  include("C:/Users/johan/Documents/GitHub/lua_csound/out/build/x64-Debug/LuaJIT/src/host/cmake/buildvm/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/johan/Documents/GitHub/lua_csound/out/build/x64-Debug/LuaJIT/src/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
