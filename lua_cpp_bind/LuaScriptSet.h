#pragma once

#include <lua.hpp>

//Call a C++ method
int call_cpp(lua_State* L);

//Set a C++ value
int set_cpp(lua_State* L);

int get_cpp(lua_State* L);
