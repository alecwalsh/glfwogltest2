#pragma once

#include <lua.hpp>

//TODO: LuaScriptSet.h should include LuaScript.h, not the other way around

//Use these at the beginning and end of function to make sure you left Lua's stack the way you found it
#ifdef DEBUG
#define LUA_STACK_CHECK_START int starttop = lua_gettop(L);
#else
#define LUA_STACK_CHECK_START
#endif

#ifdef DEBUG
#define LUA_STACK_CHECK_END assert(starttop == lua_gettop(L));
#else
#define LUA_STACK_CHECK_END
#endif

//Call a C++ method
int call_cpp(lua_State* L);

//Set a C++ value
int set_cpp(lua_State* L);

//Get a C++ value
int get_cpp(lua_State* L);
