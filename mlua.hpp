/*
    Copyright (C) 2019-2020 Johann Philippe

    This file is part of MIUP.

    The MIUP library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    MIUP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with MIUP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA

    As a special exception, if other files instantiate templates or
    use macros or inline functions from this file, this file does not
    by itself cause the resulting executable or library to be covered
    by the GNU Lesser General Public License. This exception does not
    however invalidate any other reasons why the library or executable
    file might be covered by the GNU Lesser General Public License.
*/


#ifndef LUA_HPP
#define LUA_HPP

#define LUA_OK 0

#if defined(__cplusplus) || defined(__GNUG__)
extern "C" {
#endif

#include"lua.h"
#include"lauxlib.h"
#include"lualib.h"
#include"luajit.h"
#include"luaconf.h"

#if defined(__cplusplus) || defined(__GNUG__)
}
#endif

#include<iostream>
#include"stdlib.h"
#include"stdio.h"
#include"string.h"

class Lua {
public:
    Lua() : lua(nullptr) {
        std::cout << "Lua --- Constructor " << std::endl;
        lua = luaL_newstate();
        std::cout << "State created " << std::endl;
        if(lua == nullptr) std::cout << "lua is nullptr" << std::endl;
        else std::cout << "lua is fine " << std::endl;
        luaL_openlibs(lua);
        std::cout << "Libs opened " << std::endl;
    }

    ~Lua() {
        lua_close(lua);
    }

    //execute something
    int loadString(std::string script) {
        int error = luaL_dostring(lua, script.c_str());
        isError(error);
        return error;
    }

    void jitMode(int mode) {
        luaJIT_setmode(lua, 0,mode);
    }

    void enableJIT()
    {
        luaJIT_setmode(lua,0, LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON);
    }

    int loadCString(char * script) {
        //int error = terra_dostring(lua, script);
        int error = luaL_loadstring(lua,script);
        //isError(error);
        return error;
    }

    int doFile(std::string path) {
        int error = luaL_dofile(lua, path.c_str());
        std::cout << "Lua --- Error = " << error << std::endl;
        isError(error);
        return error;
    }

    int doString(std::string str)
    {
        int error = luaL_dostring(lua, str.c_str());
        std::cout << "Lua ---Error " << error << std::endl;
        isError(error);
        return error;
    }


    void Lregister(const char *name, const luaL_Reg *module)
    {
        luaL_register(lua, name, module );
    }

    void Lopenlib(const char *name, const luaL_Reg *module) {
        luaL_openlib(lua, name,module, 0);
    }


    void registerFunction(const char *name, lua_CFunction func ) {
        lua_register(lua, name, func);
    }

    //put on top of stack
    void getGlobal(std::string global) {lua_getglobal(lua, global.c_str());}
    void getField(int index, std::string global) {lua_getfield(lua, index, global.c_str());}


    void call(int input_nbr, int output_nbr) {lua_call(lua,input_nbr, output_nbr);}
    bool pcall(int input_nbr, int output_nbr) {return (lua_pcall(lua,input_nbr,output_nbr,0 ) == 0);}

    void pushString(std::string val) {lua_pushstring(lua, val.c_str());}
    void pushInt(int val) {lua_pushinteger(lua, val);}
    void pushNumber(double val) {lua_pushnumber(lua,val);}
    void pushBool(bool val) {lua_pushboolean(lua,val);}
    void pushNil() {lua_pushnil(lua);}

    template<typename T>
    void pushLightUserData(T *ptr) {
        void *p=(void *)ptr;
        lua_pushlightuserdata(lua, p);
    }

    void createTable(int nargs, int nrec){
        lua_createtable(lua,nargs, nrec);
    }


    bool isString(int idx) {return (lua_isstring(lua,idx) == 1);}
    bool isNumber(int idx) {return (lua_isnumber(lua,idx) == 1);}
    int getType(int idx) {return lua_type(lua,idx);}

    std::string toString(int index) {return std::string(lua_tostring(lua, index));}
    char *toCString(int index) {return strdup(lua_tostring(lua,index));}

    const void *toPointer(int index) {return lua_topointer(lua,index);}
    void *toUserData(int index) {return lua_touserdata(lua,index);}

    double toNumber(int index) {return (double)lua_tonumber(lua,index);}
    int toInt(int index) {return (int)lua_tointeger(lua,index);}
    bool toBool(int index) {return (bool)lua_toboolean(lua,index);}

    void setGlobal(std::string name) {lua_setglobal(lua, name.c_str());}

    void setField(int idx, std::string name) {
        lua_setfield( lua, idx, name.c_str());
    }
    void setTable(int idx) {lua_settable(lua,idx);}
    void getTable(int idx) {lua_gettable(lua,idx);}


    void rawSeti(int stack_index, int array_index ) {lua_rawseti(lua, stack_index, array_index);}
    void rawGeti(int stack_index, int array_index) {return lua_rawgeti(lua, stack_index,array_index);}

    void newTable() {lua_newtable(lua);}


    void registerLibrary(std::string libname, luaL_Reg *l ) {luaL_register(lua, libname.c_str(), l);}

    int garbageCollector(int what, int data=0) {return lua_gc(lua,what,data);}
    int checkStack(int extra) {return lua_checkstack(lua,extra);}


    void pop(int number) {lua_pop(lua,number);}

    lua_State *getLuaState() {return lua;}

    //private methods
    void isError(int error) {
        if(error) std::cout << "Lua error in script. ERROR CODE = " << error << std::endl;
           if(error != LUA_OK) {
                   switch(error) {
                   case LUA_ERRRUN:
                       std::cout << "Lua runtime error" << std::endl;
                       break;
                   case LUA_ERRERR:
                       std::cout << "Lua handling function error " << std::endl;
                       break;
                   case LUA_ERRMEM:
                       std::cout << "Lua memory allocation error " << std::endl;
                       break;
                   case LUA_ERRFILE:
                       std::cout << "Lua Error File " << std::endl;
                       break;
                   case LUA_ERRSYNTAX:
                       std::cout << "Lua syntax error " << std::endl;
                       break;
                   case LUA_YIELD:
                       std::cout << "Lua yield error " << std::endl;
                       break;
                   }
           }

    }
private:


   //members
    lua_State *lua;
};

#endif // LUA_HPP
