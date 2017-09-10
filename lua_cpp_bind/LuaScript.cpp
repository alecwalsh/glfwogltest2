#include <lua.hpp>
#include <iostream>
#include <cassert>

#include "LuaScript.h"
#include "LuaScriptSet.h"

LuaScript::LuaScript() {
    L = luaL_newstate();
    luaL_openlibs(L);
    
    SetupBinding();
}

LuaScript::LuaScript(std::string fileName) {
    L = luaL_newstate();
    luaL_openlibs(L);
    
    SetupBinding();
    
    luaL_dofile(L, fileName.c_str());
}

LuaScript::~LuaScript() {
    lua_close(L);
}

void LuaScript::exec(std::string code) {
    luaL_dostring(L, code.c_str());
}

void LuaScript::SetupBinding() {
    lua_pushlightuserdata(L, &propertyMap);
    lua_pushcclosure(L, set_cpp, 1);
    lua_setglobal(L, "set_cpp");
    
    lua_pushlightuserdata(L, &propertyMap);
    lua_pushcclosure(L, get_cpp, 1);
    lua_setglobal(L, "get_cpp");
    
    lua_newtable(L);
    lua_setglobal(L, "cpp");
    
    lua_newtable(L);
    lua_setglobal(L, "metacpp");
    
    luaL_dostring(L, "function metacpp.__newindex(table, key, value)\n"
                                                    "set_cpp(key, value)\n"
                                                    "end");
    
    luaL_dostring(L, "function metacpp.__index(table, key)\n"
                                                "return get_cpp(key)\n"
                                                "end");
    
    lua_getglobal(L, "cpp");
    lua_getglobal(L, "metacpp");
    
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
}
