#include <lua.hpp>

#include "LuaScript.h"
#include "LuaScriptSet.h"

LuaScript::LuaScript() {
    L = luaL_newstate();
    luaL_openlibs(L);
    
    //TODO: move to separate function, call from both contructors
    lua_pushlightuserdata(L, &propertyMap);
    lua_pushcclosure(L, set_cpp, 1);
    lua_setglobal(L, "set_cpp");
    
    lua_pushlightuserdata(L, &propertyMap);
    lua_pushcclosure(L, get_cpp, 1);
    lua_setglobal(L, "get_cpp");
}

LuaScript::LuaScript(std::string fileName) {
    L = luaL_newstate();
    luaL_openlibs(L);
    
    luaL_dofile(L, fileName.c_str());
}

LuaScript::~LuaScript() {
    lua_close(L);
}
